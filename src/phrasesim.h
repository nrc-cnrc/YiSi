/**
 * @file phrasesim.h
 * @brief Phrasal similarity
 *
 * @author Jackie Lo
 *
 * Class definition of phrasal similarity classes:
 *    - phrasesim_options (command line options for phrasesim)
 *    - phrasesim_t
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#ifndef PHRASESIM_H
#define PHRASESIM_H

#include "cmdlp/cmdlp.h"
#include "lexsim.h"
#include "lexweight.h"
#include "maxmatching.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <utility>
#include <algorithm>

#define DEFAULT_LEXSIM_NAME "exact"
#define DEFAULT_INPLEXWEIGHT_NAME "uniform"
#define DEFAULT_REFLEXWEIGHT_NAME "uniform"
#define DEFAULT_PHRASESIM_NAME "nwpr"
#define DEFAULT_NGRAM 0

namespace yisi {

   struct phrasesim_options {
      std::string lexsim_name_m = DEFAULT_LEXSIM_NAME;
      std::string outlexsim_path_m;
      std::string inplexsim_path_m;
      std::string inplexweight_name_m = DEFAULT_INPLEXWEIGHT_NAME;
      std::string inplexweight_path_m;
      std::string reflexweight_name_m = DEFAULT_REFLEXWEIGHT_NAME;
      std::string reflexweight_path_m;
      std::string hyplexweight_name_m;
      std::string hyplexweight_path_m;
      std::string phrasesim_name_m = DEFAULT_PHRASESIM_NAME;

      int n_m = DEFAULT_NGRAM;

      void init(com::masaers::cmdlp::parser& p) {
         using namespace com::masaers::cmdlp;

         p.add(make_knob(lexsim_name_m))
               .fallback(DEFAULT_LEXSIM_NAME)
               .desc("Type of lex sim model: [exact(default)|ibm1|w2v|ibmw2v]")
               .name("lexsim-type")
               ;
         p.add(make_knob(outlexsim_path_m))
               .fallback("")
               .desc("Path to lex sim model file in output language")
               .name("outlexsim-path")
               ;
         p.add(make_knob(inplexsim_path_m))
               .fallback("")
               .desc("Path to lex sim model file in input language")
               .name("inplexsim-path")
               ;
         p.add(make_knob(inplexweight_name_m))
               .fallback(DEFAULT_INPLEXWEIGHT_NAME)
               .desc("Type of input lex weight model: [uniform(default)|file|learn]")
               .name("inplexweight-type")
               ;
         p.add(make_knob(inplexweight_path_m))
               .fallback("")
               .desc("[file: path to input lex weight model file "
                     "| learn: monolingual corpus in input language to learn]")
               .name("inplexweight-path")
               ;
         p.add(make_knob(reflexweight_name_m))
               .fallback(DEFAULT_REFLEXWEIGHT_NAME)
               .desc("Type of reference lex weight model: [uniform(default)|file|learn]")
               .name("lexweight-type")
               .name("reflexweight-type")
               ;
         p.add(make_knob(reflexweight_path_m))
               .fallback("")
               .desc("[file: path to reference lex weight model file "
                     "| learn: monolingual corpus in reference language to learn]")
               .name("lexweight-path")
               .name("reflexweight-path")
               ;
         p.add(make_knob(hyplexweight_name_m))
               .fallback("")
               .desc("Type of hypotheses lex weight model: [uniform|file|learn] "
                     "(default: same as reflexweight-type")
               .name("hyplexweight-type")
               ;
         p.add(make_knob(hyplexweight_path_m))
               .fallback("")
               .desc("[file: path to hypotheses lex weight model file "
                     "| learn: monolingual corpus in hypothesis language to learn]")
               .name("hyplexweight-path")
               ;
         p.add(make_knob(phrasesim_name_m))
               .fallback(DEFAULT_PHRASESIM_NAME)
               .desc("Type of phrase sim model: [nwpf: n-gram idf-weighted precision/recall]")
               .name("psname")
               .name("phrasesim-type")
               ;
         p.add(make_knob(n_m))
               .fallback(DEFAULT_NGRAM)
               .desc("N-gram size")
               .name("ngram-size")
               .name("n")
               ;
      }
   }; // struct phrasesim_options

#ifndef IGNORE_CACHE
   static std::map<std::string, std::map<std::string, std::pair<double, double> > > mpscache_m;
   static std::map<std::string, std::map<std::string, std::pair<double, double> > > xpscache_m;
#endif

   class phrasesim_t {
   public:

      phrasesim_t();
      phrasesim_t(phrasesim_options opt);
      phrasesim_t(phrasesim_t& rhs);
      ~phrasesim_t();
      double get_lexweight(std::vector<std::string>& tokens, int mode);
      std::pair<double, double> operator()(std::vector<std::string> s1tokens,
                                           std::vector<std::string>& hyptokens, int mode);
      std::pair<double, double> operator()(std::vector<std::string> s1tokens,
                                           std::vector<std::string>& hyptokens,
                                           std::vector<std::vector<double> > s1embs,
                                           std::vector<std::vector<double> > hypembs, int mode);
      std::pair<double, double> ngram(std::vector<std::string>& s1tokens,
                                      std::vector<std::string>& hyptokens, int mode);
      std::pair<double, double> ngram(std::vector<std::string>& s1tokens,
                                      std::vector<std::string>& hyptokens,
                                      std::vector<std::vector<double> > s1embs,
                                      std::vector<std::vector<double> > hypembs,
                                      int mode);
      double ngramlw(std::vector<std::string>& tokens, int mode);
      std::pair<double, double> nwpr(std::vector<std::string>& s1tokens,
                                     std::vector<std::string>& hyptokens, int mode);
      std::pair<double, double> nwpr(std::vector<std::string>& s1tokens,
                                     std::vector<std::string>& hyptokens,
                                     std::vector<std::vector<double> > s1embs,
                                     std::vector<std::vector<double> > hypembs,
                                     int mode);
      double exact(std::vector<std::string>& reftokens, std::vector<std::string>& hyptokens);
      double mean(std::vector<std::string>& reftokens, std::vector<std::string>& hyptokens);

   private:
      lexsim_t* lexsim_p;
      lexweight_t* inplexweight_p;
      lexweight_t* reflexweight_p;
      lexweight_t* hyplexweight_p;
      std::string hyplexweight_name_m;
      std::string inplexweight_name_m;
      std::string phrasesim_name_m;
      int n_m;
   }; // class phrasesim_t

} // yisi

#endif
