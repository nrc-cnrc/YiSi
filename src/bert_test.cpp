/**
 * @file bert_test.cpp
 * @brief Unit test for bert_t class.
 *
 * @author Darlene Stewart
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2020, Her Majesty in Right of Canada /
 * Copyright 2020, Sa Majeste la Reine du Chef du Canada
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "bert.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

using namespace std;

int main(const int argc, const char* argv[])
{
  string usage("Usage: bert_test <in-file>\nwrites output to <in-file>.emb");

  if (argc < 2 || argc > 2) {
    cerr << "ERROR: bert_test requires 1 argument, but got " << argc-1 << endl;
    cerr << usage << endl;
    exit(1);
  }

  if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) {
    cerr << usage << endl;
    exit(0);
  }

  string in_file(argv[1]);

  string output_path(in_file + ".emb2");
  ofstream fout(output_path);
  if (! fout) {
    cerr << "ERROR: Failed to open output file (" << output_path << "). Exiting..." << endl;
    exit(EXIT_FAILURE);
  }

  yisi::bert_t *bert_p = new yisi::bert_t("");
  size_t bert_id = bert_p->apply_model(in_file);
  size_t num_sents = bert_p->get_size(bert_id);

  vector<string> sent_units;
  vector<vector<double> > sent_emb;


  for (size_t sent_idx = 0; sent_idx < num_sents; ++sent_idx) {
    sent_units = bert_p->get_units(bert_id, sent_idx);
    sent_emb = bert_p->get_embeddings(bert_id, sent_idx);

    // Derive the toks from the subword units.
    // We assume subword continuation is marked by a '##' prefix.
    // This is so we can generate the same output as call_bert_test.
    vector<string> sent_toks;
    string tok;
    for (auto uit = sent_units.begin(); uit != sent_units.end(); uit++) {
      if (uit->size() > 1 && uit->substr(0, 2) == "##")
        tok += uit->substr(2);
      else {
        if (! tok.empty())
          sent_toks.push_back(tok);
        tok = *uit;
      }
    }
    if (! tok.empty())
      sent_toks.push_back(tok);

    cerr << endl;
    cerr << "Sentence " << sent_idx << " lengths: units=" << sent_units.size()
      << " toks=" << sent_toks.size() << " embeddings=" << sent_emb.size() << endl;

    cerr << "  Units:";
    for (auto uit = sent_units.begin(); uit != sent_units.end(); uit++) {
      cerr << " " << *uit;
    }
    cerr << endl;

    cerr << "  Toks:";
    for (auto it = sent_toks.begin(); it != sent_toks.end(); it++) {
      cerr << " " << *it;
    }
    cerr << endl;

    // Output embeddings to fout in the .emb file format, and with extra
    // details to cerr.
    size_t tid = 0;
    size_t emb_idx = 0;
    for (auto eit = sent_emb.begin(); eit != sent_emb.end(); eit++, emb_idx++) {
      if (sent_units[emb_idx].size() < 2 || sent_units[emb_idx].substr(0, 2) != "##") {
        if (emb_idx > 0) tid += 1;
      }
      cerr << "Unit embedding " << emb_idx << ": " << tid << " len: " << eit->size() << ": ";
      fout << emb_idx << "\t" << tid << "\t";
      cerr << fixed << setprecision(4);
      fout << fixed << setprecision(6);
      for (size_t i = 0; i < min(eit->size(), (size_t)16); ++i) {
        double value = eit->at(i);
        if (i < 8)
          cerr << " " << value;
        if (i != 0)
          fout << " ";
        fout << value;
      }
      cerr << endl;
      fout << endl;
    }
    fout << endl;
  }

  return 0;
}
