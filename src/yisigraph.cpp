/**
 * @file yisigraph.cpp
 * @brief YiSi graph
 *
 * @author Jackie Lo
 *
 * Class implementation of the class:
 *    - yisigraph_t
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "yisigraph.h"

#include <fstream>
#include <sstream>
#include <algorithm>

using namespace yisi;
using namespace std;

yisigraph_t::yisigraph_t(const vector<srlgraph_t> refsrlgraph,
    const srlgraph_t hypsrlgraph) {
  refsrlgraph_m = refsrlgraph;
  hypsrlgraph_m = hypsrlgraph;
  inp_b = false;

  //cout << endl;
  //cout << "refsrlgraph:" << endl;
  //cout << refsrlgraph_m;
  //cout << "hypsrlgraph:" << endl;
  //cout << hypsrlgraph_m;
  //cout<<"Done."<<endl;
}

yisigraph_t::yisigraph_t(const vector<srlgraph_t> refsrlgraph,
    const srlgraph_t hypsrlgraph, const srlgraph_t inpsrlgraph) {
  refsrlgraph_m = refsrlgraph;
  hypsrlgraph_m = hypsrlgraph;
  inpsrlgraph_m = inpsrlgraph;
  inp_b = true;

  //cout << endl;
  //cout << "refsrlgraph:" << endl;
  //cout << refsrlgraph_m;
  //cout << "hypsrlgraph:" << endl;
  //cout << hypsrlgraph_m;
  //cout<<"Done."<<endl;
}

yisigraph_t::yisigraph_t(const yisigraph_t& rhs) {
  refsrlgraph_m = rhs.refsrlgraph_m;
  hypsrlgraph_m = rhs.hypsrlgraph_m;
  refalignment_m = rhs.refalignment_m;
  hypalignment_m = rhs.hypalignment_m;
  inp_b = rhs.inp_b;
  if (rhs.inp_b) {
    inpsrlgraph_m = rhs.inpsrlgraph_m;
    inpalignment_m = rhs.inpalignment_m;
  }
}

void yisigraph_t::operator=(const yisigraph_t& rhs) {
  refsrlgraph_m = rhs.refsrlgraph_m;
  hypsrlgraph_m = rhs.hypsrlgraph_m;
  refalignment_m = rhs.refalignment_m;
  hypalignment_m = rhs.hypalignment_m;
  inp_b = rhs.inp_b;
  if (rhs.inp_b) {
    inpsrlgraph_m = rhs.inpsrlgraph_m;
    inpalignment_m = rhs.inpalignment_m;
  }
}

bool yisigraph_t::withinp() {
  return inp_b;
}

size_t yisigraph_t::get_refsize() {
  return refsrlgraph_m.size();
}

/*
   double yisigraph_t::get_sentlength(int mode, int refid) {
   switch (mode) {
   case yisi::INP_MODE:
   if (inp_b) {
   return spanlength(inpsrlgraph_m.get_role_span(inpsrlgraph_m.get_root()));
   } else {
   return 0.0;
   }
   break;
   case yisi::HYP_MODE:
   return spanlength(hypsrlgraph_m.get_role_span(hypsrlgraph_m.get_root()));
   break;
   case yisi::REF_MODE:
   if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
   return spanlength(refsrlgraph_m[refid].get_role_span(refsrlgraph_m[refid].get_root()));
   } else {
   cerr << "ERROR: refid (" << refid << ") out of range [0,"
   << refsrlgraph_m.size() << "]. Sent length undefined. Exiting..." << endl;
   exit(1);
   }
   break;
   default:
   cerr << "ERROR: Unknown mode in sent length. Contact Jackie. Exiting..." << endl;
   exit(1);
   }
   }
   */

double yisigraph_t::get_sentsim(int mode, int refid) {
  double result = 0.0;

  switch (mode) {
    case yisi::INP_MODE:
      if (inp_b) {
        return inpalignment_m[inpsrlgraph_m.get_root()].second;
      } else {
        return 0.0;
      }
      break;
    case yisi::HYP_MODE:
      for (auto it = hypalignment_m[hypsrlgraph_m.get_root()].begin();
          it != hypalignment_m[hypsrlgraph_m.get_root()].end(); it++) {
        double s = (it->second).second;
        if (s > result) {
          result = s;
        }
      }
      return result;
      break;
    case yisi::REF_MODE:
      if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
        return refalignment_m[refid][refsrlgraph_m[refid].get_root()].second;
      } else {
        cerr << "ERROR: refid (" << refid << ") out of range [0,"
          << refsrlgraph_m.size() << "]. Sent sim undefined. Exiting..." << endl;
        exit(1);
      }
      break;
    default:
      cerr << "ERROR: Unknown mode in sent sim. Contact Jackie. Exiting..." << endl;
      exit(1);
  }
}

vector<yisigraph_t::srlnid_type> yisigraph_t::get_preds(int mode, int refid) {
  switch (mode) {
    case yisi::INP_MODE:
      if (inp_b) {
        return inpsrlgraph_m.get_preds();
      } else {
        cerr << "ERROR: YiSi graph with no input sentence. "
          << "Failed to get input predicates. Exiting..." << endl;
        exit(1);
      }
      break;
    case yisi::HYP_MODE:
      return hypsrlgraph_m.get_preds();
      break;
    case yisi::REF_MODE:
      if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
        return refsrlgraph_m[refid].get_preds();
      } else {
        cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
          << "]. Failed to get reference predicates. Exiting..." << endl;
        exit(1);
      }
      break;
    default:
      cerr << "ERROR: Unknown mode in get preds. Contact Jackie. Exiting..." << endl;
      exit(1);
  }
}

vector<yisigraph_t::srlnid_type> yisigraph_t::get_args(srlnid_type roleid, int mode, int refid) {
  switch (mode) {
    case yisi::INP_MODE:
      if (inp_b) {
        return inpsrlgraph_m.get_args(roleid);
      } else {
        cerr << "ERROR: YiSi graph with no input sentence. "
          << "Failed to get input roles. Exiting..." << endl;
        exit(1);
      }
      break;
    case yisi::HYP_MODE:
      return hypsrlgraph_m.get_args(roleid);
      break;
    case yisi::REF_MODE:
      if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
        return refsrlgraph_m[refid].get_args(roleid);
      } else {
        cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
          << "]. Failed to get reference roles. Exiting..." << endl;
        exit(1);
      }
      break;
    default:
      cerr << "ERROR: Unknown mode in get args. Contact Jackie. Exiting..." << endl;
      exit(1);
  }
}

/*
   vector<string>& yisigraph_t::get_sentence(int mode, int refid) {
   switch (mode) {
   case yisi::INP_MODE:
   if (inp_b) {
   return inpsrlgraph_m.get_sentence();
   } else {
   cerr << "ERROR: YiSi graph with no input sentence. "
   << "Failed to get input sentence. Exiting..." << endl;
   exit(1);
   }
   break;
   case yisi::HYP_MODE:
   return hypsrlgraph_m.get_sentence();
   break;
   case yisi::REF_MODE:
   if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
   return refsrlgraph_m[refid].get_sentence();
   } else {
   cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
   << "]. Failed to get reference sentence. Exiting..." << endl;
   exit(1);
   }
   break;
   default:
   cerr << "ERROR: Unknown mode in get sentence. Contact Jackie. Exiting..." << endl;
   exit(1);
   }
   }
   */

vector<string> yisigraph_t::get_role_filler_units(srlnid_type roleid, int mode, int refid) {
  switch (mode) {
    case yisi::INP_MODE:
      if (inp_b) {
        return inpsrlgraph_m.get_role_filler_units(roleid);
      } else {
        cerr << "ERROR: YiSi graph with no input sentence. "
          << "Failed to get input role fillers. Exiting..." << endl;
        exit(1);
      }
      break;
    case yisi::HYP_MODE:
      return hypsrlgraph_m.get_role_filler_units(roleid);
      break;
    case yisi::REF_MODE:
      if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
        return refsrlgraph_m[refid].get_role_filler_units(roleid);
      } else {
        cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
          << "]. Failed to get reference role fillers. Exiting..." << endl;
        exit(1);
      }
      break;
    default:
      cerr << "ERROR: Unknown mode in get role fillers. Contact Jackie. Exiting..." << endl;
      exit(1);
  }
}

double yisigraph_t::get_rolespanlength(srlnid_type roleid, int mode, int refid) {
  switch (mode) {
    case yisi::INP_MODE:
      if (inp_b) {
        return spanlength(inpsrlgraph_m.get_role_span(roleid));
      } else {
        cerr << "ERROR: YiSi graph with no input sentence. "
          << "Failed to get input role span length. Exiting..." << endl;
        exit(1);
      }
      break;
    case yisi::HYP_MODE:
      return spanlength(hypsrlgraph_m.get_role_span(roleid));
      break;
    case yisi::REF_MODE:
      if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
        return spanlength(refsrlgraph_m[refid].get_role_span(roleid));
      } else {
        cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
          << "]. Failed to get reference role span length. Exiting..." << endl;
        exit(1);
      }
      break;
    default:
      cerr << "ERROR: Unknown mode in get role span length. Contact Jackie. Exiting..." << endl;
      exit(1);
  }
}

yisigraph_t::label_type yisigraph_t::get_rolelabel(srlnid_type roleid, int mode, int refid) {
  switch (mode) {
    case yisi::INP_MODE:
      if (inp_b) {
        return inpsrlgraph_m.get_role_label(roleid);
      } else {
        cerr << "ERROR: YiSi graph with no input sentence. "
          << "Failed to get input role label. Exiting..." << endl;
        exit(1);
      }
      break;
    case yisi::HYP_MODE:
      return hypsrlgraph_m.get_role_label(roleid);
      break;
    case yisi::REF_MODE:
      if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
        return refsrlgraph_m[refid].get_role_label(roleid);
      } else {
        cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
          << "]. Failed to get reference role label. Exiting..." << endl;
        exit(1);
      }
      break;
    default:
      cerr << "ERROR: Unknown mode in get role label. Contact Jackie. Exiting..." << endl;
      exit(1);
  }
}

vector<pair<int, yisigraph_t::alignment_type> > yisigraph_t::get_hypalignment(srlnid_type roleid) {
  vector<pair<int, yisigraph_t::alignment_type> > result;
  if (hypalignment_m.find(roleid) != hypalignment_m.end()) {
    result = hypalignment_m[roleid];
  }
  return result;
}

double yisigraph_t::get_alignsim(srlnid_type roleid, int mode, int refid) {
  double result = 0.0;
  switch (mode) {
    case yisi::INP_MODE:
      if (inp_b) {
        if (inpalignment_m.find(roleid) != inpalignment_m.end()) {
          return inpalignment_m[roleid].second;
        } else {
          return 0.0;
        }
      } else {
        cerr << "ERROR: YiSi graph with no input sentence. "
          << "Failed to get input alignment sim. Exiting..." << endl;
        exit(1);
      }
      break;
    case yisi::HYP_MODE:
      // return the maximum sim of all reference and input
      // better not to use this because it lost align label information
      if (hypalignment_m.find(roleid) != hypalignment_m.end()) {
        for (auto it = hypalignment_m[roleid].begin();
            it != hypalignment_m[roleid].end(); it++) {
          double s = (it->second).second;
          if (s > result) {
            result = s;
          }
        }
      }
      return result;
      break;
    case yisi::REF_MODE:
      if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
        if (refalignment_m[refid].find(roleid) != refalignment_m[refid].end()) {
          return refalignment_m[refid][roleid].second;
        } else {
          return 0.0;
        }
      } else {
        cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
          << "]. Failed to get reference alignment sim. Exiting..." << endl;
        exit(1);
      }
      break;
    default:
      cerr << "ERROR: Unknown mode in get align sim. Contact Jackie. Exiting..." << endl;
      exit(1);
  }
}

yisigraph_t::label_type yisigraph_t::get_alignlabel(srlnid_type roleid, int mode, int refid) {
  double m = 0.0;
  label_type result;

  switch (mode) {
    case yisi::INP_MODE:
      if (inp_b) {
        if (inpalignment_m.find(roleid) != inpalignment_m.end()) {
          return hypsrlgraph_m.get_role_label(inpalignment_m[roleid].first);
        } else {
          return label_type("U");
        }
      } else {
        cerr << "ERROR: YiSi graph with no input sentence. "
          << "Failed to get input alignment role label. Exiting..." << endl;
        exit(1);
      }
      break;
    case yisi::HYP_MODE:
      // return the role label of the max align sim of all reference and input
      // better not to use this
      if (hypalignment_m.find(roleid) != hypalignment_m.end()) {
        for (auto it = hypalignment_m[roleid].begin();
            it != hypalignment_m[roleid].end(); it++) {
          double s = (it->second).second;
          if (s > m) {
            int id = it->first;
            if (id < (int)refsrlgraph_m.size()) {
              result = refsrlgraph_m[id].get_role_label((it->second).first);
            } else {
              result = inpsrlgraph_m.get_role_label((it->second).first);
            }
          }
        }
      }
      return result;
      break;
    case yisi::REF_MODE:
      if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
        if (refalignment_m[refid].find(roleid) != refalignment_m[refid].end()) {
          return hypsrlgraph_m.get_role_label(refalignment_m[refid][roleid].first);
        } else {
          return label_type("U");
        }
      } else {
        cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
          << "]. Failed to get reference alignment role label. Exiting..." << endl;
        exit(1);
      }
      break;
    default:
      cerr << "ERROR: Unknown mode in get align label. Contact Jackie. Exiting..." << endl;
      exit(1);
  }
}

double yisigraph_t::get_refnormlmscore(){
  vector<double> s;
  // return the min lm score of multiple references
  for (auto it=refsrlgraph_m.begin(); it != refsrlgraph_m.end(); it++){
    s.push_back(it->get_normlmscore());
  }
  if (s.size() > 0){
    return *min_element(s.begin(), s.end());
  } else return 0.0;
}

double yisigraph_t::get_hypnormlmscore(){
  return hypsrlgraph_m.get_normlmscore();
}

double yisigraph_t::get_inpnormlmscore(){
  return inpsrlgraph_m.get_normlmscore();
}

double yisigraph_t::get_reflmscore(){
  vector<double> s;
  // return the min lm score of multiple references
  for (auto it=refsrlgraph_m.begin(); it != refsrlgraph_m.end(); it++){
    s.push_back(it->get_lmscore());
  }
  if (s.size() > 0){
    return *min_element(s.begin(), s.end());
  } else return 0.0;
}

double yisigraph_t::get_hyplmscore(){
  return hypsrlgraph_m.get_lmscore();
}

double yisigraph_t::get_inplmscore(){
  return inpsrlgraph_m.get_lmscore();
}

double yisigraph_t::spanlength(span_type span) {
  return span.second - span.first;
}

void yisigraph_t::print(ostream& os) {
  string h = yisi::join(hypsrlgraph_m.get_role_filler_units(hypsrlgraph_m.get_root()), " ");
  //os << h <<endl;
  for (size_t i = 0; i < refalignment_m.size(); i++) {
    string r = yisi::join(refsrlgraph_m[i].get_role_filler_units(refsrlgraph_m[i].get_root()), " ");
    //os << r <<endl;
    for (auto jt = refalignment_m[i].begin(); jt != refalignment_m[i].end(); jt++) {
      auto refnid = jt->first;
      auto hypnid = (jt->second).first;
      double sim = (jt->second).second;
      r = yisi::join(refsrlgraph_m[i].get_role_filler_units(refnid), " ");
      h = yisi::join(hypsrlgraph_m.get_role_filler_units(hypnid), " ");
      os << r << "\t" << h << "\t" << sim << endl;
    }
  }
  if (inp_b) {
    string inp = yisi::join(inpsrlgraph_m.get_role_filler_units(inpsrlgraph_m.get_root()), " ");
    os << inp << endl;
    for (auto kt = inpalignment_m.begin(); kt != inpalignment_m.end(); kt++) {
      auto inpnid = kt->first;
      auto hypnid = (kt->second).first;
      double sim = (kt->second).second;
      inp = yisi::join(inpsrlgraph_m.get_role_filler_units(inpnid), " ");
      h = yisi::join(hypsrlgraph_m.get_role_filler_units(hypnid), " ");
      os << inp << "\t" << h << "\t" << sim << endl;
    }
  }
}

void yisigraph_t::align(phrasesim_t* phrasesim) {
  //yisi alignment algorithm goes here
  //loop all references and input
  for (size_t refid = 0; refid < refsrlgraph_m.size(); refid++) {
    //std::cerr << "first align the sentence node of ref" << refid << std::endl;
    auto refroot = refsrlgraph_m[refid].get_root();
    auto hyproot = hypsrlgraph_m.get_root();

    auto ru = refsrlgraph_m[refid].get_role_filler_units(refroot);
    //std::cerr << "Got r " << ru.size() << std::endl;
    auto hu = hypsrlgraph_m.get_role_filler_units(hyproot);
    //std::cerr << "Got h " << hu.size() << std::endl;
    std::pair<double, double> sentsim;
    if ((refsrlgraph_m[refid].get_sent_type() != "uemb" ||  hypsrlgraph_m.get_sent_type() != "uemb") && 
        (refsrlgraph_m[refid].get_sent_type() != "bert" ||  hypsrlgraph_m.get_sent_type() != "bert") &&
        (refsrlgraph_m[refid].get_sent_type() != "contextual" ||  hypsrlgraph_m.get_sent_type() != "contextual")){
      //std::cerr<<"computing sentsim on word"<<std::endl;
      sentsim = (*phrasesim)(ru, hu, yisi::REF_MODE);
    } else {
      auto remb = refsrlgraph_m[refid].get_role_filler_embs(refroot);
      auto hemb = hypsrlgraph_m.get_role_filler_embs(hyproot);
      sentsim = (*phrasesim)(ru, hu, remb, hemb, yisi::REF_MODE);
    }

    //std::cerr << "sentsim = (" << sentsim.first << "," << sentsim.second << ")";
    //std::cerr << "refroot = " << refroot << std::endl;
    //std::cerr << "hyproot = " << hyproot << std::endl;
    refalignment_m.push_back(std::map<srlnid_type, alignment_type>());
    //std::cerr << "Done creating refalignment map" << std::endl;
    refalignment_m[refid][refroot] = alignment_type(hyproot, sentsim.second);
    //std::cerr << "Done adding ref alignment edge" << std::endl;
    if (hypalignment_m.find(hyproot) == hypalignment_m.end()) {
      hypalignment_m[hyproot] = std::vector<std::pair<int, alignment_type> >();
    }
    hypalignment_m[hyproot].push_back(std::make_pair(refid, alignment_type(refroot, sentsim.first)));
    //std::cerr << "Done adding hyp alignment edge" << std::endl;
    //std::cerr << "prepare the pred maxmatching matrix"<<std::endl;
    auto refpreds = refsrlgraph_m[refid].get_preds();
    auto hyppreds = hypsrlgraph_m.get_preds();
    maxmatching_t refpredmatch;
    maxmatching_t hyppredmatch;
    for (auto it = refpreds.begin(); it != refpreds.end(); it++) {
      auto refpredid = *it;
      auto refpredspan = refsrlgraph_m[refid].get_role_span(refpredid);
      if (refpredspan.first != refpredspan.second) {
        auto refpredphrase = refsrlgraph_m[refid].get_role_filler_units(refpredid);
        for (auto jt = hyppreds.begin(); jt != hyppreds.end(); jt++) {
          auto hyppredid = *jt;
          auto hyppredspan = hypsrlgraph_m.get_role_span(hyppredid);
          if (hyppredspan.first != hyppredspan.second) {
            auto hyppredphrase = hypsrlgraph_m.get_role_filler_units(hyppredid);
            std::pair<double, double> predsim;
            if ((refsrlgraph_m[refid].get_sent_type() != "uemb" ||  hypsrlgraph_m.get_sent_type() != "uemb") &&
                (refsrlgraph_m[refid].get_sent_type() != "bert" ||  hypsrlgraph_m.get_sent_type() != "bert") &&
                (refsrlgraph_m[refid].get_sent_type() != "contextual" ||  hypsrlgraph_m.get_sent_type() != "contextual")) {
              predsim = (*phrasesim)(refpredphrase, hyppredphrase, yisi::REF_MODE);
            } else {
              auto rpredemb = refsrlgraph_m[refid].get_role_filler_embs(refpredid);
              auto hpredemb = hypsrlgraph_m.get_role_filler_embs(hyppredid);
              predsim = (*phrasesim)(refpredphrase, hyppredphrase, rpredemb, hpredemb, yisi::REF_MODE);
            }
            refpredmatch.add_weight(refpredid, hyppredid, predsim.second);
            hyppredmatch.add_weight(refpredid, hyppredid, predsim.first);
          }
        } // for jt
      }
    }  // for it
    //std::cerr <<"run maxmatch on preds" <<std::endl;
    auto rpr = refpredmatch.run();
    auto hpr = hyppredmatch.run();
    //std::cerr <<"for each aligned pred" <<std::endl;
    for (size_t i = 0; i < rpr.size(); i++) {
      auto aligned_ref_pred = rpr[i].first.first;
      auto aligned_hyp_pred = rpr[i].first.second;
      auto psim = rpr[i].second;
      refalignment_m[refid][aligned_ref_pred] = alignment_type(aligned_hyp_pred, psim);
      //hypalignment_m[aligned_hyp_pred] = alignment_type(aligned_ref_pred, psim);
      //std::cerr<<"prepare the arg maxmatching matrix"<<std::endl;
      auto refargs = refsrlgraph_m[refid].get_args(aligned_ref_pred);
      auto hypargs = hypsrlgraph_m.get_args(aligned_hyp_pred);
      maxmatching_t argmatch;
      for (auto it = refargs.begin(); it != refargs.end(); it++) {
        auto refargid = *it;
        auto refargphrase = refsrlgraph_m[refid].get_role_filler_units(refargid);
        for (auto jt = hypargs.begin(); jt != hypargs.end(); jt++) {
          auto hypargid = *jt;
          auto hypargphrase = hypsrlgraph_m.get_role_filler_units(hypargid);
          std::pair<double, double> argsim;
          if ((refsrlgraph_m[refid].get_sent_type() != "uemb" ||  hypsrlgraph_m.get_sent_type() != "uemb") && 
              (refsrlgraph_m[refid].get_sent_type() != "bert" ||  hypsrlgraph_m.get_sent_type() != "bert") &&
              (refsrlgraph_m[refid].get_sent_type() != "contextual" ||  hypsrlgraph_m.get_sent_type() != "contextual")){
            argsim = (*phrasesim)(refargphrase, hypargphrase, yisi::REF_MODE);
          } else {
            auto rargemb = refsrlgraph_m[refid].get_role_filler_embs(refargid);
            auto hargemb = hypsrlgraph_m.get_role_filler_embs(hypargid);
            argsim = (*phrasesim)(refargphrase, hypargphrase, rargemb, hargemb, yisi::REF_MODE);
          }
          argmatch.add_weight(refargid, hypargid, argsim.second);
        } // for jt
      } // for it
      //std::cerr << "run maxmatch on args" <<std::endl;
      auto ar = argmatch.run();
      //std::cerr << "for each aligned arg" <<std::endl;
      for (size_t j = 0; j < ar.size(); j++) {
        auto aligned_ref_arg = ar[j].first.first;
        auto aligned_hyp_arg = ar[j].first.second;
        auto asim = ar[j].second;
        refalignment_m[refid][aligned_ref_arg] = alignment_type(aligned_hyp_arg, asim);
        //hypalignment_m[aligned_hyp_arg] = alignment_type(aligned_ref_arg, asim);
      } // for j
    } // for i
    for (size_t i = 0; i < hpr.size(); i++) {
      auto aligned_ref_pred = hpr[i].first.first;
      auto aligned_hyp_pred = hpr[i].first.second;
      auto psim = hpr[i].second;
      if (hypalignment_m.find(aligned_hyp_pred) == hypalignment_m.end()) {
        hypalignment_m[aligned_hyp_pred] = std::vector<std::pair<int, alignment_type> >();
      }
      hypalignment_m[aligned_hyp_pred].push_back(std::make_pair(refid,
            alignment_type(aligned_ref_pred, psim)));
      auto refargs = refsrlgraph_m[refid].get_args(aligned_ref_pred);
      auto hypargs = hypsrlgraph_m.get_args(aligned_hyp_pred);
      maxmatching_t argmatch;
      for (auto it = refargs.begin(); it != refargs.end(); it++) {
        auto refargid = *it;
        auto refargphrase = refsrlgraph_m[refid].get_role_filler_units(refargid);
        for (auto jt = hypargs.begin(); jt != hypargs.end(); jt++) {
          auto hypargid = *jt;
          auto hypargphrase = hypsrlgraph_m.get_role_filler_units(hypargid);
          std::pair<double, double> argsim;
          if ((refsrlgraph_m[refid].get_sent_type() != "uemb" ||  hypsrlgraph_m.get_sent_type() != "uemb") &&
              (refsrlgraph_m[refid].get_sent_type() != "bert" ||  hypsrlgraph_m.get_sent_type() != "bert") &&
              (refsrlgraph_m[refid].get_sent_type() != "contextual" ||  hypsrlgraph_m.get_sent_type() != "contextual")) {
            argsim = (*phrasesim)(refargphrase, hypargphrase, yisi::REF_MODE);
          } else {
            auto rargemb = refsrlgraph_m[refid].get_role_filler_embs(refargid);
            auto hargemb = hypsrlgraph_m.get_role_filler_embs(hypargid);
            argsim = (*phrasesim)(refargphrase, hypargphrase, rargemb, hargemb, yisi::REF_MODE);
          }
          argmatch.add_weight(refargid, hypargid, argsim.first);
        } // for jt
      } // for it
      auto ar = argmatch.run();
      for (size_t j = 0; j < ar.size(); j++) {
        auto aligned_ref_arg = ar[j].first.first;
        auto aligned_hyp_arg = ar[j].first.second;
        auto asim = ar[j].second;
        if (hypalignment_m.find(aligned_hyp_arg) == hypalignment_m.end()) {
          hypalignment_m[aligned_hyp_arg] =
            std::vector<std::pair<int, alignment_type> >();
        }
        hypalignment_m[aligned_hyp_arg].push_back(std::make_pair(refid,
              alignment_type(aligned_ref_arg, asim)));
      }  // for j
    } // for i
  } // for refid
  //input
  if (inp_b) {
    //std::cerr << "first align the sentence node of inp: ";
    auto inproot = inpsrlgraph_m.get_root();
    auto hyproot = hypsrlgraph_m.get_root();
    auto r = inpsrlgraph_m.get_role_filler_units(inproot);
    //std::cerr<< r.size();
    auto h = hypsrlgraph_m.get_role_filler_units(hyproot);
    //std::cerr<< h.size();
    std::pair<double, double> sentsim;
    if ((inpsrlgraph_m.get_sent_type() != "uemb" ||  hypsrlgraph_m.get_sent_type() != "uemb") && 
        (inpsrlgraph_m.get_sent_type() != "bert" ||  hypsrlgraph_m.get_sent_type() != "bert") &&
        (inpsrlgraph_m.get_sent_type() != "contextual" ||  hypsrlgraph_m.get_sent_type() != "contextual")) {
      sentsim = (*phrasesim)(r, h, yisi::INP_MODE);
    } else {
      auto remb = inpsrlgraph_m.get_role_filler_embs(inproot);
      auto hemb = hypsrlgraph_m.get_role_filler_embs(hyproot);
      //std::cerr<< remb.size() <<" " <<hemb.size();
      sentsim = (*phrasesim)(r, h, remb, hemb, yisi::INP_MODE);
    }
    //std::cerr << "sentsim = (" << sentsim.first << "," << sentsim.second << ")";
    inpalignment_m[inproot] = alignment_type(hyproot, sentsim.second);
    if (hypalignment_m.find(hyproot) == hypalignment_m.end()) {
      hypalignment_m[hyproot] = std::vector<std::pair<int, alignment_type> >();
    }
    hypalignment_m[hyproot].push_back(std::make_pair((int)refsrlgraph_m.size(),
          alignment_type(inproot, sentsim.first)));
    auto inppreds = inpsrlgraph_m.get_preds();
    auto hyppreds = hypsrlgraph_m.get_preds();
    maxmatching_t inppredmatch;
    maxmatching_t hyppredmatch;
    for (auto it = inppreds.begin(); it != inppreds.end(); it++) {
      auto inppredid = *it;
      auto inppredspan = inpsrlgraph_m.get_role_span(inppredid);
      if (inppredspan.first != inppredspan.second) {
        auto inppredphrase = inpsrlgraph_m.get_role_filler_units(inppredid);
        for (auto jt = hyppreds.begin(); jt != hyppreds.end(); jt++) {
          auto hyppredid = *jt;
          auto hyppredspan = hypsrlgraph_m.get_role_span(hyppredid);
          if (hyppredspan.first != hyppredspan.second) {
            auto hyppredphrase = hypsrlgraph_m.get_role_filler_units(hyppredid);
            std::pair<double, double> predsim;
            if ((inpsrlgraph_m.get_sent_type() != "uemb" ||  hypsrlgraph_m.get_sent_type() != "uemb") && 
                (inpsrlgraph_m.get_sent_type() != "bert" ||  hypsrlgraph_m.get_sent_type() != "bert") &&
                (inpsrlgraph_m.get_sent_type() != "contextual" ||  hypsrlgraph_m.get_sent_type() != "contextual")) {
              predsim = (*phrasesim)(inppredphrase, hyppredphrase, yisi::INP_MODE);
            } else {
              auto ipredemb = inpsrlgraph_m.get_role_filler_embs(inppredid);
              auto hpredemb = hypsrlgraph_m.get_role_filler_embs(hyppredid);
              predsim = (*phrasesim)(inppredphrase, hyppredphrase, ipredemb, hpredemb, yisi::INP_MODE);
            }
            inppredmatch.add_weight(inppredid, hyppredid, predsim.second);
            hyppredmatch.add_weight(inppredid, hyppredid, predsim.first);
          }
        }
      }
    }
    auto ipr = inppredmatch.run();
    auto hpr = hyppredmatch.run();
    for (size_t i = 0; i < ipr.size(); i++) {
      auto aligned_inp_pred = ipr[i].first.first;
      auto aligned_hyp_pred = ipr[i].first.second;
      auto psim = ipr[i].second;
      inpalignment_m[aligned_inp_pred] = alignment_type(aligned_hyp_pred, psim);
      auto inpargs = inpsrlgraph_m.get_args(aligned_inp_pred);
      auto hypargs = hypsrlgraph_m.get_args(aligned_hyp_pred);
      maxmatching_t argmatch;
      for (auto it = inpargs.begin(); it != inpargs.end(); it++) {
        auto inpargid = *it;
        auto inpargphrase = inpsrlgraph_m.get_role_filler_units(inpargid);
        for (auto jt = hypargs.begin(); jt != hypargs.end(); jt++) {
          auto hypargid = *jt;
          auto hypargphrase = hypsrlgraph_m.get_role_filler_units(hypargid);
          std::pair<double, double> argsim;
          if ((inpsrlgraph_m.get_sent_type() != "uemb" ||  hypsrlgraph_m.get_sent_type() != "uemb") && 
              (inpsrlgraph_m.get_sent_type() != "bert" ||  hypsrlgraph_m.get_sent_type() != "bert") &&
              (inpsrlgraph_m.get_sent_type() != "contextual" ||  hypsrlgraph_m.get_sent_type() != "contextual")) {
            argsim = (*phrasesim)(inpargphrase, hypargphrase, yisi::INP_MODE);
          } else {
            auto iargemb = inpsrlgraph_m.get_role_filler_embs(inpargid);
            auto hargemb = hypsrlgraph_m.get_role_filler_embs(hypargid);
            argsim = (*phrasesim)(inpargphrase, hypargphrase, iargemb, hargemb, yisi::INP_MODE);
          }
          argmatch.add_weight(inpargid, hypargid, argsim.second);
        }
      }
      auto ar = argmatch.run();
      for (size_t j = 0; j < ar.size(); j++) {
        auto aligned_inp_arg = ar[j].first.first;
        auto aligned_hyp_arg = ar[j].first.second;
        auto asim = ar[j].second;
        inpalignment_m[aligned_inp_arg] = alignment_type(aligned_hyp_arg, asim);
      }
    }
    for (size_t i = 0; i < hpr.size(); i++) {
      auto aligned_inp_pred = hpr[i].first.first;
      auto aligned_hyp_pred = hpr[i].first.second;
      auto psim = hpr[i].second;
      if (hypalignment_m.find(aligned_hyp_pred) == hypalignment_m.end()) {
        hypalignment_m[aligned_hyp_pred] =
          std::vector<std::pair<int, alignment_type> >();
      }
      hypalignment_m[aligned_hyp_pred].push_back(std::make_pair((int)refsrlgraph_m.size(),
            alignment_type(aligned_inp_pred, psim)));
      auto inpargs = inpsrlgraph_m.get_args(aligned_inp_pred);
      auto hypargs = hypsrlgraph_m.get_args(aligned_hyp_pred);
      maxmatching_t argmatch;
      for (auto it = inpargs.begin(); it != inpargs.end(); it++) {
        auto inpargid = *it;
        auto inpargphrase = inpsrlgraph_m.get_role_filler_units(inpargid);
        for (auto jt = hypargs.begin(); jt != hypargs.end(); jt++) {
          auto hypargid = *jt;
          auto hypargphrase = hypsrlgraph_m.get_role_filler_units(hypargid);
          std::pair<double, double> argsim;
          if ((inpsrlgraph_m.get_sent_type() != "uemb" ||  hypsrlgraph_m.get_sent_type() != "uemb") &&
              (inpsrlgraph_m.get_sent_type() != "bert" ||  hypsrlgraph_m.get_sent_type() != "bert") &&
              (inpsrlgraph_m.get_sent_type() != "contextual" ||  hypsrlgraph_m.get_sent_type() != "contextual")) {
            argsim = (*phrasesim)(inpargphrase, hypargphrase, yisi::INP_MODE);
          } else {
            auto iargemb = inpsrlgraph_m.get_role_filler_embs(inpargid);
            auto hargemb = hypsrlgraph_m.get_role_filler_embs(hypargid);
            argsim = (*phrasesim)(inpargphrase, hypargphrase, iargemb, hargemb, yisi::INP_MODE);
          }
          argmatch.add_weight(inpargid, hypargid, argsim.first);
        }
      }
      auto ar = argmatch.run();
      for (size_t j = 0; j < ar.size(); j++) {
        auto aligned_inp_arg = ar[j].first.first;
        auto aligned_hyp_arg = ar[j].first.second;
        auto asim = ar[j].second;
        if (hypalignment_m.find(aligned_hyp_arg) == hypalignment_m.end()) {
          hypalignment_m[aligned_hyp_arg] =
            std::vector<std::pair<int, alignment_type> >();
        }
        hypalignment_m[aligned_hyp_arg].push_back(std::make_pair((int)refsrlgraph_m.size(),
              alignment_type(aligned_inp_arg, asim)));
      }
    }
  }
} // align
