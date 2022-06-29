/**
 * @file yisiscorer.cpp
 * @brief YiSi scorer
 *
 * @author Jackie Lo
 *
 * Class definition of YiSi scorer class:
 *    - yisiscorer_t
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "yisiscorer.h"
#include <algorithm>

using namespace yisi;
using namespace std;

yisiscorer_t::yisiscorer_t() {}

yisiscorer_t::yisiscorer_t(yisi_options yisi_opt, phrasesim_options phrase_opt) {
   frameweight_name_m = yisi_opt.frameweight_name_m;
   alpha_m = yisi_opt.alpha_m;
   beta_m = yisi_opt.beta_m;
   gamma_m = yisi_opt.gamma_m;

   int i = 0;
   if (yisi_opt.labelconfig_path_m != "") {
      std::cerr << "Reading labelconfig from " << yisi_opt.labelconfig_path_m << " ... ";
      std::ifstream LBL(yisi_opt.labelconfig_path_m.c_str());
      if (!LBL) {
         std::cerr << "ERROR: Failed to open labelconfig. Exiting..." << std::endl;
         exit(1);
      }
      while (!LBL.eof()) {
         std::string line;
         getline(LBL, line);
         if (line != "") {
            std::istringstream iss(line);
            while (!iss.eof()) {
               std::string label;
               iss >> label;
               label_m[label] = i;
            }
            i++;
         }
      }
      LBL.close();
      std::cerr << "Done." << std::endl;
   }

   weightconfig_path_m = yisi_opt.weightconfig_path_m;
   if (weightconfig_path_m != ""
         && weightconfig_path_m != "lexweight"
         && weightconfig_path_m != "uniform") {
      std::cerr << "Reading weightconfig from " << yisi_opt.weightconfig_path_m << " ... ";
      std::ifstream W(weightconfig_path_m.c_str());
      if (!W) {
         std::cerr << "ERROR: Failed to open weightconfig. Exiting..." << std::endl;
         exit(1);
      }
      while (!W.eof()) {
         double w;
         W >> w;
         weight_m.push_back(w);
      }
      W.close();
      std::cerr << "Done." << std::endl;
      if ((int)weight_m.size() != i) {
         std::cerr << "ERROR: Number of weights in weightconfig does not match "
            << "with number of lines in labelconfig. Exiting..." << std::endl;
         exit(1);
      }
   } else {
      for (int j = 0; j < i; j++) {
         weight_m.push_back(1.0);
      }
   }

   phrasesim_p = new phrasesim_t(phrase_opt);
   hypsrl_p = new srl_t(yisi_opt.hypsrl_name_m, yisi_opt.hypsrl_path_m);
   hypsrl_name_m = yisi_opt.hypsrl_name_m;
   if (yisi_opt.refsrl_name_m != "") {
      refsrl_p = new srl_t(yisi_opt.refsrl_name_m, yisi_opt.refsrl_path_m);
   } else {
      refsrl_p = hypsrl_p;;
   }
   refsrl_name_m = yisi_opt.refsrl_name_m;
   inpsrl_p = new srl_t(yisi_opt.inpsrl_name_m, yisi_opt.inpsrl_path_m);
   inpsrl_name_m = yisi_opt.inpsrl_name_m;
}

yisiscorer_t::~yisiscorer_t() {
   if (phrasesim_p != NULL) {
      delete phrasesim_p;
      phrasesim_p = NULL;
   }
   if (inpsrl_p != NULL) {
      delete inpsrl_p;
      inpsrl_p = NULL;
   }
   if (hypsrl_p != NULL) {
      delete hypsrl_p;
      hypsrl_p = NULL;
      if (refsrl_name_m == "") {
         refsrl_p = NULL;
      }
   }
   if (refsrl_p != NULL) {
      delete refsrl_p;
      refsrl_p = NULL;
   }
}

void yisiscorer_t::estimate_weight(std::vector<srlgraph_t> srls) {
   for (auto it = srls.begin(); it != srls.end(); it++) {
      auto preds = it->get_preds();
      for (auto jt = preds.begin(); jt != preds.end(); jt++) {
         auto pred_label = it->get_role_label(*jt);
         if (label_m.find(pred_label) == label_m.end()) {
            std::cerr << "ERROR: Unknown predicate label '" << pred_label
               << "'. Check your labelconfig. Exiting..." << std::endl;
            exit(1);
         }
         weight_m[label_m[pred_label]] += 0.25;
         auto args = it->get_args(*jt);
         for (auto kt = args.begin(); kt != args.end(); kt++) {
            auto arg_label = it->get_role_label(*kt);
            if (label_m.find(arg_label) == label_m.end()) {
               std::cerr << "ERROR: Unknown argument label '" << arg_label
                  << "'. Check your labelconfig. Exiting..." << std::endl;
               exit(1);
            }
            weight_m[label_m[arg_label]] += 1.0;
         }
      }
   }
}

void yisiscorer_t::estimate_weight(std::vector<std::vector<srlgraph_t> > msrls) {
   for (auto it = msrls.begin(); it != msrls.end(); it++) {
      estimate_weight(*it);
   }
}

std::vector<srlgraph_t> yisiscorer_t::inpsrlparse(std::vector<sent_t*> inpsents) {
   if (phrasesim_p->inplexweight_name_m == "learn" && phrasesim_p->inplexweight_path_m == ""){
      cerr << "Learning inp lex weight ... ";
      vector<vector<string> > tokens;
      for (auto it=inpsents.begin(); it!=inpsents.end(); it++){
         if ((*it)->get_type() == "word"){
            tokens.push_back((*it)->get_tokens());
         } else {
            tokens.push_back((*it)->get_units());
         }
      }
      phrasesim_p->learn_inplexweight(tokens);
      cerr << "Done." <<endl;
   }
   //std::cerr << "Tokenizing/SRL-ing the input ...";
   std::vector<srlgraph_t> result = inpsrl_p->parse(inpsents);
   //std::cerr << "Done." << std::endl;
   if (weightconfig_path_m == "") {
      this->estimate_weight(result);
   }
   return result;
}

std::vector<std::vector<srlgraph_t> > yisiscorer_t::refsrlparse(std::vector<std::vector<sent_t*> > refsents) {
   if (phrasesim_p->reflexweight_name_m == "learn" && phrasesim_p->reflexweight_path_m == ""){
      cerr << "Learning ref lex weight ... ";
      vector<vector<string> > tokens;
      for (auto it=refsents.begin(); it!=refsents.end(); it++){
         for (auto jt=it->begin(); jt!=it->end(); jt++){
            if ((*jt)->get_type() == "word"){
               tokens.push_back((*jt)->get_tokens());
            } else {
               tokens.push_back((*jt)->get_units());
            }
         }
      }
      phrasesim_p->learn_reflexweight(tokens);
      cerr << "Done." <<endl;
   }
   //std::cerr << "Tokenizing/SRL-ing the references ... ";
   std::vector<std::vector<srlgraph_t> > result;
   for (size_t i=0; i<refsents.size(); i++){
      result.push_back(refsrl_p->parse(refsents[i]));
   }
   //std::cerr << "Done." << std::endl;
   if (weightconfig_path_m == "") {
      this->estimate_weight(result);
   }
   return result;
}

std::vector<srlgraph_t> yisiscorer_t::hypsrlparse(std::vector<sent_t*> hypsents) {
   if (phrasesim_p->hyplexweight_name_m == "learn" && phrasesim_p->hyplexweight_path_m==""){
      cerr << "Learning hyp lex weight ... ";
      vector<vector<string> > tokens;
      for (auto it=hypsents.begin(); it!=hypsents.end(); it++){
         if ((*it)->get_type() == "word"){
            tokens.push_back((*it)->get_tokens());
         } else {
            tokens.push_back((*it)->get_units());
         }
      }
      phrasesim_p->learn_hyplexweight(tokens);
      cerr << "Done." <<endl;
   } else if (phrasesim_p->hyplexweight_name_m == ""){
      phrasesim_p->set_hyplexweight();
   }
   //std::cerr << "Tokenizing/SRL-ing the hypotheses ... ";
   std::vector<srlgraph_t> result = hypsrl_p->parse(hypsents);
   //std::cerr << "Done." << std::endl;
   return result;
}

srlgraph_t yisiscorer_t::hypsrlparse(sent_t* hypsent) {
   //std::cerr <<"Tokenizing/SRL-ing the hypothesis ... ";
   srlgraph_t result = hypsrl_p->parse(hypsent);
   //std::cerr << "Done." << std::endl;
   return result;
}

yisigraph_t yisiscorer_t::align(const std::vector<srlgraph_t> refsrlgraph, const srlgraph_t hypsrlgraph) {
   //std::cerr << "Creating YiSi graph ... ";
   yisigraph_t result(refsrlgraph, hypsrlgraph);
   //std::cerr << "start aligning ... ";
   result.align(phrasesim_p);
   //result.print(std::cerr);
   //std::cerr << "Done." << std::endl;
   return result;
}

yisigraph_t yisiscorer_t::align(const std::vector<srlgraph_t> refsrlgraph,
                  const srlgraph_t hypsrlgraph, const srlgraph_t inpsrlgraph) {
   //std::cerr << "Creating YiSi graph with input... ";
   yisigraph_t result(refsrlgraph, hypsrlgraph, inpsrlgraph);
   //std::cerr << "start aligning ... ";
   result.align(phrasesim_p);
   //result.print(std::cerr);
   //std::cerr << "Done." << std::endl;
   return result;
}

double yisiscorer_t::score(std::string input, std::string hyp) {
      sent_t* sent_inp = new sent_t("word");
      sent_inp->set_tokens(tokenize(input));
      sent_t* sent_hyp = new sent_t("word");
      sent_hyp->set_tokens(tokenize(hyp));

      std::vector< srlgraph_t > dummy;  //no ref srl graph because we are in YiSi-2 interactive mode
      std::vector< srlgraph_t > inpsrlgraphs = inpsrlparse(std::vector< sent_t* >{sent_inp});
      srlgraph_t hypsrlgraphs = hypsrlparse(sent_hyp);
      yisigraph_t m = align(dummy, hypsrlgraphs, inpsrlgraphs.at(0));
      return score(m, "yisi");
}

double yisiscorer_t::score(yisigraph_t& yg, string yisi_mode) {
   double precision = score(yg, yisi::HYP_MODE);
   double recall = score(yg, yisi::REF_MODE);

   double yisi = 0.0;
   if (precision == 0.0 || recall == 0.0) {
      yisi = 0.0;
   } else if (yisi_mode == "yisi"){
      yisi = (precision * recall) / (alpha_m * precision + (1.0 - alpha_m) * recall);
   } else if (yisi_mode == "yisi+nlm"){
      // linear combination of semantic score and hyp normalized lm score weighted by gamma
      yisi = ((1.0-gamma_m) * ((precision * recall) / (alpha_m * precision + (1.0 - alpha_m) * recall))) +
         (gamma_m * yg.get_hypnormlmscore());
   } else if (yisi_mode == "yisi+lm"){
      // linear combination of semantic score and hyp normalized lm score weighted by gamma
      yisi = ((1.0-gamma_m) * ((precision * recall) / (alpha_m * precision + (1.0 - alpha_m) * recall))) +
         (gamma_m * yg.get_hyplmscore());
   } else if (yisi_mode == "yisi_lm"){
      double hyplmscore = yg.get_hyplmscore();
      double reflmscore = 1.0;
      if (yg.get_refsize() > 0) {
         reflmscore = yg.get_reflmscore();
      }
      double inplmscore = 1.0;
      if (yg.withinp()) {
         inplmscore = yg.get_inplmscore();
      }
      double denom = min(reflmscore, inplmscore);
      precision = (1.0-gamma_m) * precision + gamma_m * hyplmscore;
      recall = (1.0-gamma_m) * recall + gamma_m * (hyplmscore/max(denom, hyplmscore));
      yisi = (precision * recall) / (alpha_m * precision + (1.0 - alpha_m) * recall);
   }
   return yisi;
}

std::vector<double> yisiscorer_t::features(yisigraph_t& yg) {
   std::vector<double> result;
   //double flat =  yg.get_sentsim();
   //result.push_back(flat);
   //result.push_back(score(yg));
   std::vector<double> precision = features(yg, yisi::HYP_MODE);
   std::vector<double> recall = features(yg, yisi::REF_MODE);
   for (auto it = precision.begin(); it != precision.end(); it++) {
      result.push_back(*it);
   }
   for (auto it = recall.begin(); it != recall.end(); it++) {
      result.push_back(*it);
   }
   return result;
}

double yisiscorer_t::score(yisigraph_t yg, int mode) {
   //std::cerr <<"Scoring...";
   auto f = features(yg, mode);
   double structure = f[weight_m.size()];
   double flat = f[weight_m.size() + 1];
   //std::cerr <<"(" << beta_m <<"," <<structure <<"," <<flat <<")";
   //std::cerr <<"Done."<<std::endl;
   return beta_m * structure + (1 - beta_m) * flat;

}

std::vector<double> yisiscorer_t::features(yisigraph_t yg, int mode) {
   if (mode == yisi::REF_MODE) {
      return rfeatures(yg);
   } else {
      return pfeatures(yg);
   }
}

void yisiscorer_t::compute_features(yisigraph_t yg, std::vector<double> feats,
   double& structure, double& flat, int mode, int refid) {
   flat = yg.get_sentsim(mode, refid);

   double tfw = 0.0; // total frame weight
   //std::vector<double> tsim(weight_m.size(), 0.0); // total similarity by role type
   //std::vector<double> tcount(weight_m.size(), 0.0); // total count by role type
   double nom = 0.0;
   double denom = 0.0;

   auto preds = yg.get_preds(mode, refid);

   for (auto it = preds.begin(); it != preds.end(); it++) {
      std::vector<double> sim(weight_m.size(), 0.0);
      std::vector<double> count(weight_m.size(), 0.0);
      auto predid = *it;
      double sanity_check = yg.get_rolespanlength(predid, mode, refid);
      double predsim = yg.get_alignsim(predid, mode, refid);
      auto predlabel = yg.get_rolelabel(predid, mode, refid);
      double predweight = get_roleweight(yg, predid, mode, refid);

      if (sanity_check > 0) {
         //if (prfunc_name_m=="f" || prfunc_name_m=="max"){
         double fw = yg.get_rolespanlength(predid, mode, refid);
         double fn = 0.0;

         sim[label_m[predlabel]] += predsim;
         fn = predweight * predsim;

         double fd = predweight;
         count[label_m[predlabel]] += 1.0;

         auto args = yg.get_args(predid, mode, refid);
         for (auto jt = args.begin(); jt != args.end(); jt++) {
            auto argid = *jt;
            fw += yg.get_rolespanlength(argid, mode, refid);

            auto arglabel = yg.get_rolelabel(argid, mode, refid);
            double argsim = 0.0;
            yisigraph_t::label_type alignlabel;
            if (mode == yisi::HYP_MODE) {
               auto alignment = yg.get_hypalignment(argid);
               for (auto it = alignment.begin(); it != alignment.end(); it++) {
                  double s = (it->second).second;
                  int id = it->first;
                  yisigraph_t::label_type l;
                  if (id < (int)yg.get_refsize()) {
                     l = yg.get_rolelabel((it->second).first, yisi::REF_MODE, id);
                  } else {
                     l = yg.get_rolelabel((it->second).first, yisi::INP_MODE);
                  }
                  if (s > argsim && match(arglabel, l)) {
                     argsim = s;
                     alignlabel = l;
                  }
               }
            } else {
               alignlabel = yg.get_alignlabel(argid, mode, refid);
               argsim = yg.get_alignsim(argid, mode, refid);
            }

            double argweight = get_roleweight(yg, argid, mode, refid);

            sim[label_m[arglabel]] += argsim;
            fn += argweight * argsim;

            count[label_m[arglabel]] += 1.0;
            fd += argweight;
         }

         if (fn > 0 && fd > 0) {
            if (frameweight_name_m == "coverage") {
               nom += fw * (fn / fd);
            } else {
               nom += fn / fd;
            }
         }
         if (frameweight_name_m == "coverage") {
            denom += fw;
         } else {
            denom += 1;
         }

         for (size_t i = 0; i < feats.size(); i++) {
            if (count[i] > 0) {
               feats[i] += fw * (sim[i] / count[i]);
            }
         }
         tfw += fw;
      }
   }
   if (tfw > 0) {
      for (size_t i = 0; i < feats.size(); i++) {
         feats[i] /= tfw;
      }
   }

   //if (prfunc_name_m == "lexexp") {
   //   for (size_t i = 0; i < tsim.size(); i++) {
   //      if (tcount[i] > 0) {
   //         result[i] = tsim[i] / tcount[i];
   //      }
   //   }
   //}
   if (nom > 0 && denom > 0) {
      structure = nom / denom;
   }
}

std::vector<double> yisiscorer_t::pfeatures(yisigraph_t yg) {
   std::vector<double> result(weight_m.size(), 0.0);
   double structure = 0.0;
   double flat = 0.0;

   compute_features(yg, result, structure, flat, yisi::HYP_MODE);

   result.push_back(structure);
   result.push_back(flat);
   return result;
}

std::vector<double> yisiscorer_t::rfeatures(yisigraph_t yg) {
   std::vector<double> result(weight_m.size(), 0.0);
   double mflat = 0.0;
   double mstructure = 0.0;

   //for all reference
   for (size_t i = 0; i < yg.get_refsize(); i++) {
      std::vector<double> feats(weight_m.size(), 0.0);
      double structure = 0.0;
      double flat = 0.0;
      //std::cerr << "Computing recall features for reference #" << i << " ... ";
      compute_features(yg, feats, structure, flat, yisi::REF_MODE, i);
      //std::cerr << "Done." << std::endl;
      if (structure > mstructure) {
         mstructure = structure;
         result = feats;
      }
      if (flat > mflat) {
         mflat = flat;
      }
   }

   //input
   if (yg.withinp()) {
      std::vector<double> feats(weight_m.size(), 0.0);
      double structure = 0.0;
      double flat = 0.0;
      //std::cerr << "Computing recall features for input ... ";
      compute_features(yg, feats, structure, flat, yisi::INP_MODE);
      //std::cerr << "Done." << std::endl;
      if (structure > mstructure) {
         mstructure = structure;
         result = feats;
      }
      if (flat > mflat) {
         mflat = flat;
      }
   }

   result.push_back(mstructure);
   result.push_back(mflat);
   return result;
}

bool yisiscorer_t::match(std::string label1, std::string label2) {
   if (label1 == "U" || label2 == "U") {
      return false;
   } else {
      if (label_m.find(label1) == label_m.end()) {
         std::cerr << "ERROR: Unknown srl label '" << label1 << "' in YiSi for matching label 1. "
            << "Check your labelconfig. Exiting..." << std::endl;
         exit(1);
      }
      if (label_m.find(label2) == label_m.end()) {
         std::cerr << "ERROR: unknown srl label '" << label2 << "' in yisi for matching label 2. "
            << "Check your labelconfig. Exiting..." << std::endl;
         exit(1);
      }
      return (label_m[label1] == label_m[label2]);
   }
}

double yisiscorer_t::get_roleweight(yisigraph_t yg, size_t roleid, int mode, int refid) {
   if (weightconfig_path_m == "lexweight") {
      auto fillers = yg.get_role_filler_units(roleid, mode, refid);
      return phrasesim_p->get_lexweight(fillers, mode);
   } else {
      std::string label = yg.get_rolelabel(roleid, mode, refid);
      if (label_m.find(label) == label_m.end()) {
         std::cerr << "ERROR: Unknown srl label '" << label << "' in yisi for get_weight. "
            << "Check your labelconfig. Exiting..." << std::endl;
         exit(1);
      }
      return weight_m[label_m[label]];
   }
}
