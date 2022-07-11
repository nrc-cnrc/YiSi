/**
 * @file read_sent_test.cpp
 * @brief Unit test for read_sent.
 *
 * @author Darlene Stewart
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2019, Her Majesty in Right of Canada /
 * Copyright 2019, Sa Majeste la Reine du Chef du Canada
 */

#include <iostream>
#include <string>
#include <assert.h>

#include "sent.h"

using namespace std;
using namespace yisi;

string span2str(sent_t::span_type span) {
  return "[" + to_string(span.first) + ", " + to_string(span.second) + ")";
}

int main(const int argc, const char* argv[])
{
  if ( argc < 3 || argc > 5) {
    cerr << "ERROR: read_sent_test requires 2-4 arguments, but got " << argc-1 << endl;
    cerr << "Usage: read_sent_test sent_type tok_or_unit_file [unit_delim [emb_file]]" << endl;
    exit (1);
  }

  string sent_type = argv[1];
  string tok_or_unit_file = argv[2];
  string unit_delim;
  string emb_file;
  if (argc > 3)
    unit_delim = argv[3];
  if (argc > 4)
    emb_file = argv[4];

  vector<sent_t*> sents;
  if (argc > 4)
    sents = read_sent(sent_type, tok_or_unit_file, unit_delim, emb_file);
  else if (argc > 3)
    sents = read_sent(sent_type, tok_or_unit_file, unit_delim);
  else
    sents = read_sent(sent_type, tok_or_unit_file);

  for (auto sent_it = sents.begin(); sent_it != sents.end(); sent_it++) {
    sent_t* sent(*sent_it);
    vector<string> toks(sent->get_tokens());
    auto sent_uspan(sent->tspan2uspan(sent_t::span_type(0, toks.size())));
    for(size_t i = sent_uspan.first; i < sent_uspan.second; i++) {
      auto uspan(sent_t::span_type(i, i+1));
      auto tspan(sent->uspan2tspan(uspan));
      auto units = sent->get_units(uspan);
      auto toks = sent->get_tokens(tspan);
      assert(units.size() == 1);
      assert(toks.size() == 1);
      if (sent_type == "word") {
        assert(units[0] == toks[0]);
      }
      cout << units[0] << "\t" << toks[0] << "\t";
      if (sent_type == "uemb" || sent_type == "bert") {
        auto embs = sent->get_embs(uspan);
        assert(embs.size() == 1);
        cout << "\t";
        if (sent_type == "bert")
          assert(embs[0].size() == 768);
        for (auto emb_it = embs[0].begin(); emb_it != embs[0].end(); emb_it++) {
          // For bert, limit the output to the first 16 embedding values.
          if (sent_type != "bert" || emb_it - embs[0].begin() < 16) {
            if (emb_it != embs[0].begin())
              cout << " ";
            cout << *emb_it;
          }
          assert(*emb_it >= -1.0 && *emb_it <= 1.0);
        }
      }
      cout << endl;
    }
    cout << endl;
  }

  return 0;
}
