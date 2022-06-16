/**
 * @file sent.cpp
 * @brief Sentence
 *
 * @author Jackie Lo, Darlene Stewart
 *
 * Class implementation for the classes:
 *    - sent_t
 * and the definitions of some utility functions working on it.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2019-2020, Her Majesty in Right of Canada /
 * Copyright 2019-2020, Sa Majeste la Reine du Chef du Canada
 */

#include "bert.h"
#include "sent.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>

using namespace yisi;
using namespace std;


sent_t::sent_t() {
   sent_type_m = "word";
}

sent_t::sent_t(string sent_type) {
   sent_type_m = sent_type;
}

sent_t::sent_t(const sent_t& rhs) {
   sent_type_m = rhs.sent_type_m;
   token_m = rhs.token_m;
   unit_m = rhs.unit_m;
   emb_m = rhs.emb_m;
   tid2uspan_m = rhs.tid2uspan_m;
   uid2tid_m = rhs.uid2tid_m;
}

void sent_t::operator=(const sent_t& rhs) {
   sent_type_m = rhs.sent_type_m;
   token_m = rhs.token_m;
   unit_m = rhs.unit_m;
   emb_m = rhs.emb_m;
   tid2uspan_m = rhs.tid2uspan_m;
   uid2tid_m = rhs.uid2tid_m;
}

string sent_t::get_type() {
   return sent_type_m;
}

vector<string> sent_t::get_tokens(span_type tspan) {
   vector<string> result;
   for (size_t i = tspan.first; i < tspan.second; i++) {
      result.push_back(token_m[i]);
   }
   /*
   cerr << "In get_tokens(" << tspan.first << "," << tspan.second << "): ";
   for (auto it = result.begin(); it != result.end(); it++) {
      cerr << *it << " ";
   }
   cerr << endl;
   */
   return result;
}

vector<string> sent_t::get_tokens() {
   // cerr << "In get_tokens(): " << endl;
   return token_m;
}

vector<string> sent_t::get_units(span_type uspan) {
   vector<string> result;
   if (sent_type_m == "word") {
      for (size_t i = uspan.first; i < uspan.second; i++) {
         result.push_back(token_m[i]);
      }
   } else {
      for (size_t i = uspan.first; i < uspan.second; i++) {
         if (i < unit_m.size()) {
            result.push_back(unit_m[i]);
         }
      }
   }
   return result;
}

vector<string> sent_t::get_units() {
   return unit_m;
} 

vector<vector<double> > sent_t::get_embs(span_type uspan) {
   if (sent_type_m == "uemb" || sent_type_m == "bert" || sent_type_m == "contextual") {
      vector<vector<double> > result;
      for (size_t i = uspan.first; i < uspan.second; i++) {
         result.push_back(emb_m[i]);
      }
      return result;
   } else {
      cerr << "ERROR: sentence type (" << sent_type_m << ") "
           << "does not provide contextual embeddings. Exiting..." << endl;
      exit(EXIT_FAILURE);
   }
}

double sent_t::get_normlmscore(){
   // return the normalized lm prob 
   double result = lmscore_m / unit_m.size();
   
   return exp2(-result);
}

double sent_t::get_lmscore(){
  return exp2(-lmscore_m);
}

sent_t::span_type sent_t::tspan2uspan(span_type tspan) {
   if (sent_type_m == "word") {
      return tspan;
   } else {
      //cerr << tid2uspan_m.size();
      if (tspan.first < tid2uspan_m.size() && (tspan.second-1) < tid2uspan_m.size()) {
         return span_type(tid2uspan_m[tspan.first].first, tid2uspan_m[tspan.second-1].second);
      } else {
         return tspan;
      }
   }
}

sent_t::span_type sent_t::uspan2tspan(span_type uspan) {
   if (sent_type_m == "word") {
      return uspan;
   } else {
      return span_type(uid2tid_m[uspan.first], uid2tid_m[uspan.second-1]+1);
   }
}

void sent_t::set_tokens(vector<string> t) {
   token_m = t;
   /*
   cerr << "In set_tokens(t): ";
   for (auto it = token_m.begin(); it != token_m.end(); it++) {
      cerr << *it << " ";
   }
   cerr << endl;
   */
}

void sent_t::set_units(vector<string> u ) {
   unit_m = u;
}

void sent_t::set_embs(vector<vector<double> > e) {
   emb_m = e;
}

void sent_t::set_lmscore(double s){
   lmscore_m = s;
}

void sent_t::set_tid2uspan(vector<span_type> t2u) {
   tid2uspan_m = t2u;
}

void sent_t::set_uid2tid(vector<size_t> u2t) {
   uid2tid_m = u2t;
}

size_t sent_t::get_token_size() {
   return token_m.size();
}

vector<sent_t*> yisi::read_sent(
      string sent_type,
      string file_path,
      string unit_delim,
      string idemb_path,
      string context_config,
      contextual_t* contextual_p,
      int mode) {
   vector<sent_t*> result;
   vector<vector<double> > sent_emb;
   vector<sent_t::span_type> sent_t2u;
   vector<size_t> sent_u2t;

   // Validate the sent_type
   if (sent_type == "word") {
      if (file_path.empty() or ! idemb_path.empty()) {
         cerr << "ERROR: sent_type '" << sent_type << "' requires file_path ("
            << file_path << ") and no idemb_path (" << idemb_path << "). Exiting..." << endl;
         exit(EXIT_FAILURE);
      }
   } else if (sent_type == "unit" || sent_type == "bert" || sent_type == "contextual") {
      if (file_path.empty() or unit_delim.empty() or ! idemb_path.empty()) {
         cerr << "ERROR: sent_type '" << sent_type << "' requires file_path ("
            << file_path << ") and unit_delim (" << unit_delim
            << ") and no idemb_path (" << idemb_path << "). Exiting..." << endl;
         exit(EXIT_FAILURE);
      }
   } else if (sent_type == "uemb") {
      if (file_path.empty() or unit_delim.empty() or idemb_path.empty()) {
         cerr << "ERROR: sent_type '" << sent_type << "' requires file_path ("
            << file_path << "), and unit_delim (" << unit_delim
            << ") and idemb_path (" << idemb_path << "). Exiting..." << endl;
         exit(EXIT_FAILURE);
      }
   } else {
      cerr << "ERROR: bad sent_type '" << sent_type << "'. "
         << "sent_type must be one of: word, unit, uemb, bert, contextual. Exiting..." << endl;
      exit(EXIT_FAILURE);
   }

   // Validate the unit_delim, removing the alphabetic character; set pfx_delim
   bool pfx_delim(true);      // true iff the delimiter prefixes units
   bool unt_delim(true);      // true iff the delimiter specifies begin or end of units 
   {
      const char* alphabet = "uwUW";
      string orig_delim(unit_delim);
      size_t txt_idx = unit_delim.find_first_of(alphabet);
      if (txt_idx == string::npos || txt_idx == unit_delim.size()-1) {
         pfx_delim = true;
         if (unit_delim[txt_idx] == 'u' || unit_delim[txt_idx] == 'U'){
            unt_delim = true;
         } else {
            unt_delim = false;
         }
         unit_delim = unit_delim.substr(0, txt_idx);
      } else if (txt_idx == 0) {
         pfx_delim = false;
         if (unit_delim[txt_idx] == 'u' || unit_delim[txt_idx] == 'U'){
            unt_delim = true;
         } else {
            unt_delim = false;
         }
         unit_delim = unit_delim.substr(1);
      }
      /*
         txt_idx = unit_delim.find_first_of(alphabet);  // should have no alphabetic left
         if (txt_idx != string::npos) {
         cerr << "ERROR: unit_delim (" << orig_delim << ") must contain at most 1 "
         << "alphabetic character at either the start or end. Exiting..." << endl;
         exit(EXIT_FAILURE);
         }
         */
   }

   size_t num_sents;

   // For sent_type == "bert"
   bert_t *bert_p = NULL;
   size_t bert_id = 0;

   // For sent_type != "bert"
   vector<string> file_strs;
   ifstream idemb_fin;
   size_t idemb_lineno = 0;

   if( sent_type == "bert") {
      bert_p = new bert_t(context_config);
      bert_id = bert_p->apply_model(file_path);
      num_sents = bert_p->get_size(bert_id);
   } else {
      file_strs = read_file(file_path);
      num_sents = file_strs.size();
      if (! idemb_path.empty()) {
         idemb_fin.open(idemb_path);
         if (! idemb_fin) {
            cerr << "ERROR: Failed to open idemb file (" << idemb_path << "). Exiting..." << endl;
            exit(EXIT_FAILURE);
         }
      }
   }

   vector<double> unit_emb;
   size_t emb_dim = 0;
   result.reserve(num_sents);

   auto check_value = [&](size_t actual, size_t expected, string desc) {
      if (actual != expected) {
         cerr << "ERROR: " << desc << " mismatch at line " << idemb_lineno
            << " (sentence " << result.size() << ") in embeddings file ("
            << idemb_path << ")." << endl;
         cerr << "Expected " << expected << ", but found " << actual
            << ". Exiting..." << endl;
         exit(EXIT_FAILURE);
      }
   };

   for (size_t sent_idx = 0; sent_idx < num_sents; ++ sent_idx) {

      sent_t *sent_p = new sent_t(sent_type);
      vector<string> sent_tokens;
      vector<string> sent_units;
      contextualfeatures_t features;
      if (sent_type == "bert") {
         sent_units = bert_p->get_units(bert_id, sent_idx);
      } else if (sent_type == "contextual"){
         if (mode==yisi::INP_MODE){
            features = contextual_p->get_proj_features(file_strs.at(sent_idx));
         } else {
            features = contextual_p->get_features(file_strs.at(sent_idx));
         }
         sent_units = features.units_m;
         sent_p->set_lmscore(features.lmscore_m);
      } else {
         sent_tokens = tokenize(file_strs.at(sent_idx));
         if (sent_type != "word"){
            sent_units.swap(sent_tokens);
         }
      }

      // Recover sent_tokens from sent_units by merging according to unit_delim.
      // Set tid2uspan (sent_t2u) and uid2tid (sent_u2t) in the process.
      if (sent_type != "word") {
         sent_p->set_units(sent_units);
         sent_tokens.reserve(sent_units.size());  // about the same size
         size_t unit_idx = 0;
         bool unit_cont = false;
         for (auto ut = sent_units.begin(); ut != sent_units.end(); ut++, unit_idx++) {
            if (pfx_delim) {
               if ((ut->rfind(unit_delim, 0) == string::npos) == unt_delim) { //XOR
                  // prefix delim not found XOR word delim
                  // start of new token
                  if (unt_delim){
                     sent_tokens.push_back(*ut);
                  } else {
                     sent_tokens.push_back(ut->substr(unit_delim.size()));
                  }
                  sent_t2u.push_back(sent_t::span_type(unit_idx, unit_idx+1));
               } else {
                  // continuation of previous token
                  if (unit_idx == 0) {
                     if (unt_delim){
                        cerr << "Warning: First unit starts with continuation delimiter ("
                           << unit_delim << ") at line " << sent_idx + 1
                           << " in units file (" << file_path << ")." << endl;
                     } else {
                        cerr << "Warning: First unit does not start with word beginning delimiter ("
                           << unit_delim << ") at line " << sent_idx + 1
                           << " in units file (" << file_path << ")." << endl;
                     }
                     sent_tokens.push_back("");
                     sent_t2u.push_back(sent_t::span_type(unit_idx, unit_idx));
                  }
                  if (unt_delim){
                     sent_tokens.back() += ut->substr(unit_delim.size());
                  } else {
                     sent_tokens.back() += *ut;
                  }
                  ++sent_t2u.back().second;
               }
               sent_u2t.push_back(sent_t2u.size()-1);
            } else {
               auto delim_idx = ut->rfind(unit_delim);
               if (delim_idx + unit_delim.size() < ut->size()){  // delim must end the unit
                  delim_idx = string::npos;
               }
               string unit_txt = delim_idx == string::npos ? *ut : ut->substr(0, delim_idx+1);
               if (! unit_cont) {
                  // start of new token
                  sent_tokens.push_back(unit_txt);
                  sent_t2u.push_back(sent_t::span_type(unit_idx, unit_idx+1));
               } else {
                  // continuation of previous token
                  sent_tokens.back() += unit_txt;
                  ++sent_t2u.back().second;
               }
               unit_cont = ((delim_idx != string::npos) == unt_delim);
               if (unit_cont && ut+1 == sent_units.end()) {
                  if (unt_delim){
                     cerr << "Warning: Last unit ends with continuation delimiter ("
                        << unit_delim << ") at line " << sent_idx + 1
                        << " in units file (" << file_path << ")." << endl;
                  } else {
                     cerr << "Warning: Last unit does not end with word ending delimiter ("
                        << unit_delim << ") at line " << sent_idx + 1
                        << " in units file (" << file_path << ")." << endl;
                  }
               }
            }
         } // for ut
         sent_p->set_tid2uspan(sent_t2u);
         sent_p->set_uid2tid(sent_u2t);
         sent_t2u.clear();
         sent_u2t.clear();
      } // if (sent_type != "word")

      sent_p->set_tokens(sent_tokens);

      // Determine the embeddings for the sentence.
      if (sent_type == "bert") {
         sent_emb = bert_p->get_embeddings(bert_id, sent_idx);
      } else if (sent_type == "contextual"){
         sent_emb = features.embeddings_m;
      } else if (sent_type == "uemb") {
         string emb_line;
         while (getline(idemb_fin, emb_line)) {  // one line per unit
            ++idemb_lineno;
            if (lstrip(emb_line).empty())    // empty line signals end-of-sentence
               break;
            istringstream emb_iss(emb_line);

            size_t uid, tid;
            emb_iss >> uid >> tid;
            // Sanity check: validate uid and tid
            check_value(uid, sent_emb.size(), "uid");
            check_value(tid, sent_p->uspan2tspan(sent_t::span_type(uid, uid+1)).first, "tid");

            // Retrieve the embedding values for the current unit.
            unit_emb.clear();
            unit_emb.reserve(emb_dim);
            while (!emb_iss.eof()) {
               double v;
               emb_iss >> v;
               unit_emb.push_back(v);
            }

            sent_emb.push_back(unit_emb);
         } // while getline
      }

      // Normalize and set the embeddings: sent_p->set_embs(sent_emb)
      if (sent_type == "uemb" || sent_type == "bert" || sent_type == "contextual") {
         for (auto ue_it = sent_emb.begin(); ue_it != sent_emb.end(); ue_it++) {
            if (emb_dim == 0)
               emb_dim = ue_it->size();
            check_value(ue_it->size(), emb_dim, "Embeddings dimension");
            double len = 0.0;
            for (size_t i = 0; i < ue_it->size(); i++) {
               len += ue_it->at(i) * ue_it->at(i);
            }
            len = sqrt(len);
            for (size_t i = 0; i < ue_it->size(); i++) {
               ue_it->at(i) /= len;
            }
         }
         check_value(sent_emb.size(), sent_units.size(), "Number of embeddings");
         sent_p->set_embs(sent_emb);
         sent_emb.clear();
      }

      result.push_back(sent_p);
   } // for ft

   if (idemb_fin)
      idemb_fin.close();
   return result;
}
