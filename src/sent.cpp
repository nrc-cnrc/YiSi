/**
 * @file sent.cpp
 * @brief Sentence
 *
 * @author Jackie Lo with updates by Darlene Stewart
 *
 * Class implementation for the classes:
 *    - sent_t
 * and the definitions of some utility functions working on it.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2019, Her Majesty in Right of Canada /
 * Copyright 2019, Sa Majeste la Reine du Chef du Canada
 */

#include "sent.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <regex>

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

vector<vector<double> > sent_t::get_embs(span_type uspan) {
   if (sent_type_m == "uemb") {
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
      return span_type(uid2tid_m[uspan.first], uid2tid_m[uspan.second-1]);
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

void sent_t::set_tid2uspan(vector<span_type> t2u) {
   tid2uspan_m = t2u;
}

void sent_t::set_uid2tid(vector<size_t> u2t) {
   uid2tid_m = u2t;
}

size_t sent_t::get_token_size() {
   return token_m.size();
}

vector<sent_t*> yisi::read_sent(string sent_type, string token_path, string unit_path, string idemb_path) {
   vector<sent_t*> result;
   vector<vector<double> > sent_emb;
   vector<sent_t::span_type> sent_t2u;
   vector<size_t> sent_u2t;

   // Validate the sent_type
   if (sent_type == "word") {
      if (token_path.empty() or ! unit_path.empty() or ! idemb_path.empty()) {
         cerr << "ERROR: sent_type '" << sent_type << "' requires token_path ("
            << token_path << "), and no unit_path (" << unit_path
            << ") and no idemb_path (" << idemb_path << "). Exiting..." << endl;
         exit(EXIT_FAILURE);
      }
   } else if (sent_type == "unit") {
      // Currently need idemb file for "unit" to set sent_u2t and sent_t2u
      if (token_path.empty() or unit_path.empty() or idemb_path.empty()) {
         cerr << "ERROR: sent_type '" << sent_type << "' requires token_path ("
            << token_path << "), and unit_path (" << unit_path
            << ") and idemb_path (" << idemb_path << "). Exiting..." << endl;
         exit(EXIT_FAILURE);
      }
   } else if (sent_type == "uemb") {
      if (token_path.empty() or unit_path.empty() or idemb_path.empty()) {
         cerr << "ERROR: sent_type '" << sent_type << "' requires token_path ("
            << token_path << "), and unit_path (" << unit_path
            << ") and idemb_path (" << idemb_path << "). Exiting..." << endl;
         exit(EXIT_FAILURE);
      }
   } else {
      cerr << "ERROR: bad sent_type '" << sent_type << "'. "
         << "sent_type must be one of: word, unit, uemb. Exiting..." << endl;
      exit(EXIT_FAILURE);
   }

   auto token_strs = read_file(token_path);
   vector<string> unit_strs;
   if (! unit_path.empty()) {
      unit_strs = read_file(unit_path);
      if (unit_strs.size() != token_strs.size()) {
         cerr << "ERROR: Number of sentences mismatch in units file " << unit_path << endl;
         cerr << "Expected " << token_strs.size() << " sentences, but found "
            << unit_strs.size() << ". Exiting..." << endl;
         exit(EXIT_FAILURE);
      }
   }
   auto ut = unit_strs.begin();
   ifstream idemb_fin;
   if (! idemb_path.empty()) {
      idemb_fin.open(idemb_path);
      if (! idemb_fin) {
         cerr << "ERROR: Failed to open idemb file (" << idemb_path << "). Exiting..." << endl;
         exit(EXIT_FAILURE);
      }
   }

   vector<double> unit_emb;
   size_t emb_dim = 0;
   size_t idemb_lineno = 0;
   result.reserve(token_strs.size());

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

   for (auto tt = token_strs.begin(); tt != token_strs.end(); tt++) {
      sent_t* sent_p = new sent_t(sent_type);
      vector<string> sent_tokens(tokenize(*tt));
      sent_p->set_tokens(sent_tokens);
      vector<string> sent_units;
      if (! unit_path.empty()) {
         sent_units = tokenize(*ut);
         sent_p->set_units(sent_units);
      }
      size_t currtid = (size_t)-1;
      string emb_line;
      while (getline(idemb_fin, emb_line)) {  // one line per unit
         ++idemb_lineno;
         emb_line = regex_replace(emb_line, regex("^\\s+"), ""); // trim leading whitespace
         if (emb_line.empty())    // empty line signals end-of-sentence
            break;
         istringstream emb_iss(emb_line);
         size_t uid, tid;
         emb_iss >> uid >> tid;
         // Set u2t and t2u for the current unit.
         sent_u2t.push_back(tid);
         check_value(uid, sent_u2t.size()-1, "uid");
         if (tid != currtid) {
            sent_t2u.push_back(sent_t::span_type(uid, uid + 1));
            currtid = tid;
         } else {
            sent_t2u.back().second = uid + 1;
         }
         check_value(tid, sent_t2u.size()-1, "tid");
         // Retrieve and normalize the embedding values for the current unit.
         unit_emb.clear();
         unit_emb.reserve(emb_dim);
         double len = 0.0;
         while (!emb_iss.eof()) {
            double v;
            emb_iss >> v;
            unit_emb.push_back(v);
            len += v * v;
         }
         len = sqrt(len);
         for (size_t i = 0; i < unit_emb.size(); i++) {
            unit_emb[i] /= len;
         }
         if (emb_dim == 0)
            emb_dim = unit_emb.size();
         check_value(unit_emb.size(), emb_dim, "Embeddings dimension");
         sent_emb.push_back(unit_emb);
         unit_emb.clear();
      }
      if (! idemb_path.empty()) {
         sent_p->set_embs(sent_emb);
         sent_p->set_tid2uspan(sent_t2u);
         sent_p->set_uid2tid(sent_u2t);
         // Sanity checks
         check_value(sent_u2t.size(), sent_units.size(), "Number of units");
         check_value(sent_t2u.size(), sent_tokens.size(), "Number of tokens");
         check_value(sent_emb.size(), sent_units.size(), "Number of embeddings");
      }

      result.push_back(sent_p);
      sent_emb.clear();
      sent_t2u.clear();
      sent_u2t.clear();
      if (! unit_path.empty()) {
         ++ut;          // advance ut in lock-step with tt.
      }
   }

   idemb_fin.close();
   return result;
}
