/**
 * @file yisi.cpp
 * @brief Main program for YiSi.
 *
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "yisi.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;
using namespace yisi;


extern "C" int eval(eval_options eval_opt, yisi_options yisi_opt, phrasesim_options phrasesim_opt)
{
   if (phrasesim_opt.reflexweight_name_m == "learn" && phrasesim_opt.reflexweight_path_m == "") {
      if (eval_opt.ref_type_m == "word") {
         phrasesim_opt.reflexweight_path_m = eval_opt.ref_file_m;
      } else {
         phrasesim_opt.reflexweight_path_m = eval_opt.refunit_file_m;
      }
   }
   if (phrasesim_opt.hyplexweight_name_m == "learn" && phrasesim_opt.hyplexweight_path_m == "") {
      if (eval_opt.hyp_type_m == "word") {
         phrasesim_opt.hyplexweight_path_m = eval_opt.hyp_file_m;
      } else {
         phrasesim_opt.hyplexweight_path_m = eval_opt.hypunit_file_m;
      }
   }
   if (phrasesim_opt.inplexweight_name_m == "learn" && phrasesim_opt.inplexweight_path_m == "") {
      if (eval_opt.inp_type_m == "word") {
         phrasesim_opt.inplexweight_path_m = eval_opt.inp_file_m;
      } else {
         phrasesim_opt.inplexweight_path_m = eval_opt.inpunit_file_m;
      }
   }

   yisiscorer_t yisi(yisi_opt, phrasesim_opt);

   if (eval_opt.sntscore_file_m == "") {
      eval_opt.sntscore_file_m = eval_opt.hyp_file_m + ".sntyisi";
   }

   if (eval_opt.docscore_file_m == "") {
      eval_opt.docscore_file_m = eval_opt.sntscore_file_m + ".docyisi";
   }

   ofstream SNTOUT;
   open_ofstream(SNTOUT, eval_opt.sntscore_file_m);

   cerr << "Reading hyp sents... ";
   vector<sent_t*> hypsents = read_sent(eval_opt.hyp_type_m, eval_opt.hyp_file_m, eval_opt.hypunit_file_m, eval_opt.hypidemb_file_m);
   cerr << "Done." << endl;

   vector < vector<sent_t*> > refsents;
   if (eval_opt.ref_file_m != "") {
      cerr << "Reading ref sents... ";
      auto reffiles = tokenize(eval_opt.ref_file_m, ':');
      auto refunits = tokenize(eval_opt.refunit_file_m, ':');
      auto refidemb = tokenize(eval_opt.refidemb_file_m, ':');
      size_t i = 0;
      vector<sent_t*> rs;
      if (reffiles.size() == refunits.size()) {
         rs = read_sent(eval_opt.ref_type_m, reffiles[i], refunits[i], refidemb[i]);
      } else {
         rs = read_sent(eval_opt.ref_type_m, reffiles[i]);
      }
      if (rs.size() == hypsents.size()) {
         for (auto jt = rs.begin(); jt != rs.end(); jt++) {
            vector<sent_t*> ref;
            ref.push_back(*jt);
            refsents.push_back(ref);
         }
         i++;
         for (; i < reffiles.size(); i++) {
            rs = read_sent(eval_opt.ref_type_m, reffiles[i], refunits[i], refidemb[i]);
            if (rs.size() == hypsents.size()) {
               for (size_t j = 0; j < rs.size(); j++) {
                  refsents[j].push_back(rs[j]);
               }
            } else {
               cerr << "ERROR: No. of sentences in ref-file (" << rs.size()
                  << ") does not match with no. of sentences in hyp-file ("
                  << hypsents.size() << "). Check your input! Exiting ..."
                  << endl;
               exit(1);
            }
         }
      } else {
         cerr << "ERROR: No. of sentences in ref-file (" << rs.size()
            << ") does not match with no. of sentences in hyp-file ("
            << hypsents.size() << "). Check your input! Exiting ..." << endl;
         exit(1);
      }
      cerr << "Done." << endl;
   }

   vector<sent_t*> inpsents;
   if (eval_opt.inp_file_m != "") {
      cerr << "Reading inp sents... ";
      inpsents = read_sent(eval_opt.inp_type_m, eval_opt.inp_file_m, eval_opt.inpunit_file_m, eval_opt.inpidemb_file_m);
      if (inpsents.size() != hypsents.size()) {
         cerr << "ERROR: No. of sentences in inp-file (" << inpsents.size()
            << ") does not match with no. of sentences in hyp-file ("
            << hypsents.size() << "). Check your input! Exiting..." << endl;
         exit(1);
      }
      cerr << "Done." << endl;
   }

   cerr << "Creating hyp srlgraphs... ";
   vector<srlgraph_t> hypsrlgraphs = yisi.hypsrlparse(hypsents);
   cerr << "Done." << endl;
   vector < vector<srlgraph_t> > refsrlgraphs;

   for (size_t i = 0; i < hypsrlgraphs.size(); i++) {
      refsrlgraphs.push_back(vector<srlgraph_t>());
   }

   if (refsents.size() > 0) {
      cerr << "Creating ref srlgraphs... ";
      for (size_t i = 0; i < hypsrlgraphs.size(); i++) {
         refsrlgraphs[i] = yisi.refsrlparse(refsents[i]);
      }
      cerr << "Done." << endl;
   }

   vector<srlgraph_t> inpsrlgraphs;
   if (inpsents.size() > 0) {
      cerr << "Creating inp srlgraphs... ";
      inpsrlgraphs = yisi.inpsrlparse(inpsents);
      cerr << "Done." << endl;
   }

   double docscore = 0.0;

   for (size_t i = 0; i < hypsrlgraphs.size(); i++) {
      cout << "Evaluating line " << i + 1 << endl;
      yisigraph_t m;
      if (eval_opt.inp_file_m != "") {
         /*
          cerr<<"inpsrlgraph:"<<endl;
          inpsrlgraphs[i].print(cout, i);
          cerr<<"hypsrlgraph:"<<endl;
          hypsrlgraphs[i].print(cout, i);
          cerr<<"yisigraph:"<<endl;
          */
         m = yisi.align(refsrlgraphs[i], hypsrlgraphs[i], inpsrlgraphs[i]);
         // m.print(cout);
      } else {
         // hypsrlgraphs[i].print(cout, i);
         m = yisi.align(refsrlgraphs[i], hypsrlgraphs[i]);
         // m.print(cout);
      }
      if (eval_opt.mode_m != "features") {
         double s = yisi.score(m);
         SNTOUT << s << endl;
         docscore += s;
      } else {
         auto f = yisi.features(m);
         for (auto it = f.begin(); it != f.end(); it++) {
            SNTOUT << *it << " ";
         }
         SNTOUT << endl;
      }
   }
   SNTOUT.close();

   if (eval_opt.mode_m != "features") {
      ofstream DOCOUT;
      open_ofstream(DOCOUT, eval_opt.docscore_file_m);
      docscore /= hypsents.size();
      DOCOUT << docscore << endl;
      DOCOUT.close();
   }

   for (auto it = hypsents.begin(); it != hypsents.end(); it++) {
      delete *it;
      *it = NULL;
   }
   for (auto it = refsents.begin(); it != refsents.end(); it++) {
      for (auto jt = it->begin(); jt != it->end(); jt++) {
         delete *jt;
         *jt = NULL;
      }
   }
   for (auto it = inpsents.begin(); it != inpsents.end(); it++) {
      delete *it;
      *it = NULL;
   }

   return 0;
}

int main(const int argc, const char* argv[])
{
   typedef com::masaers::cmdlp::options<eval_options, yisi_options, phrasesim_options> options_type;

   options_type opt(argc, argv);
   if (!opt) {
      return opt.exit_code();
   }

   return eval(opt, opt, opt);
}
