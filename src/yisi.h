/**
 * @file yisi.h
 * @brief Main program for YiSi.
 *
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2019, Her Majesty in Right of Canada /
 * Copyright 2019, Sa Majeste la Reine du Chef du Canada
 */



#ifndef YISI_H
#define YISI_H

#include "cmdlp/options.h"
#include "yisiscorer.h"
#include "phrasesim.h"

#define DEFAULT_REF_TYPE "word"
#define DEFAULT_HYP_TYPE "word"
#define DEFAULT_INP_TYPE "word"
#define DEFAULT_CONTEXT_CONFIG "multi_cased_L-12_H-768_A-12:-4:512:8:0"
#define DEFAULT_UNIT_DELIMITER "##U"    // BERT unit separator
#define DEFAULT_MODE "yisi"

namespace yisi {
   struct eval_options {
      std::string ref_type_m = DEFAULT_REF_TYPE;
      std::string hyp_type_m = DEFAULT_HYP_TYPE;
      std::string inp_type_m = DEFAULT_INP_TYPE;
      std::string context_config_m = DEFAULT_CONTEXT_CONFIG;

      std::string ref_file_m;
      std::string hyp_file_m;
      std::string inp_file_m;
      std::string inp_unit_delim_m = DEFAULT_UNIT_DELIMITER;
      std::string ref_unit_delim_m = DEFAULT_UNIT_DELIMITER;
      std::string hyp_unit_delim_m = DEFAULT_UNIT_DELIMITER;
      std::string inpidemb_file_m;
      std::string refidemb_file_m;
      std::string hypidemb_file_m;

      std::string sntscore_file_m;
      std::string docscore_file_m;

      std::string mode_m = DEFAULT_MODE;

      void init(com::masaers::cmdlp::parser& p) {
         using namespace com::masaers::cmdlp;
         p.add(make_knob(ref_type_m))
            .fallback(DEFAULT_REF_TYPE)
            .desc("Type of reference sentences. [word(default)|unit|uemb|contextual]")
            .name("ref-type")
            ;
         p.add(make_knob(hyp_type_m))
            .fallback(DEFAULT_HYP_TYPE)
            .desc("Type of hypothesis sentences. [word(default)|unit|uemb|contextual]")
            .name("hyp-type")
            ;
         p.add(make_knob(inp_type_m))
            .fallback(DEFAULT_INP_TYPE)
            .desc("Type of input. [word(default)|unit|uemb|contextual]")
            .name("inp-type")
            ;
	 p.add(make_knob(context_config_m))
            .fallback(DEFAULT_CONTEXT_CONFIG)
            .desc("Configuration for extracting features from pretrained context model separated by ':'.")
            .name("context-config")
            ;


         p.add(make_knob(ref_file_m))
            .fallback("")
            .desc("Filenames of references (words/subword units) separated by ':'. (in surface word form for SRL.)")
            .name("ref-file")
            ;
         p.add(make_knob(hyp_file_m))
            .fallback("")
            .desc("Filename of hypotheses (words/subword units). (in surface word form for SRL.)")
            .name("hyp-file")
            ;
         p.add(make_knob(inp_file_m))
            .fallback("")
            .desc("Filename of input (words/subword units). (in surface word form for SRL.)")
            .name("inp-file")
            ;

         p.add(make_knob(sntscore_file_m))
            .fallback("")
            .desc("Filename of sentence score output. (default: <hyp-file>.scores)")
            .name("sntscore-file")
            ;
         p.add(make_knob(docscore_file_m))
            .fallback("")
            .desc("Filename of document score output. (default: <sntscore-file>.doc")
            .name("docscore-file")
            ;

         p.add(make_knob(inp_unit_delim_m))
            .fallback(DEFAULT_UNIT_DELIMITER)
            .desc("Unit delimiter for input segmented in subword units. (default: ##U)")
            .name("inp-unit-delim")
            ;
         p.add(make_knob(hyp_unit_delim_m))
            .fallback(DEFAULT_UNIT_DELIMITER)
            .desc("Unit delimiter for hypotheses segmented in subword units. (default: ##U)")
            .name("hyp-unit-delim")
            ;
         p.add(make_knob(ref_unit_delim_m))
            .fallback(DEFAULT_UNIT_DELIMITER)
            .desc("Unit delimiter for reference segmented in subword units separated by ':'. (default: ##U)")
            .name("ref-unit-delim")
            ;

         p.add(make_knob(inpidemb_file_m))
            .fallback("")
            .desc("Filename to input subword units with contextual embeddings: one unit per line, "
                  "empty line separates sentences [unitid<TAB>tokenid<TAB>space_sep_emb].")
            .name("inpidemb-file")
            ;
         p.add(make_knob(hypidemb_file_m))
            .fallback("")
            .desc("Filename to hypotheses subword units with contextual embeddings: one unit per line, "
                  "empty line separates sentences [unitid<TAB>tokenid<TAB>space_sep_emb].")
            .name("hypidemb-file")
            ;
         p.add(make_knob(refidemb_file_m))
            .fallback("")
            .desc("Filename to reference subword units with contextual embeddings separated by ':': one "
                  "unit per line, empty line separates sentences [unitid<TAB>tokenid<TAB>space_sep_emb].")
            .name("refidemb-file")
            ;

         p.add(make_knob(mode_m))
            .fallback(DEFAULT_MODE)
            .desc("Output mode of YiSi [yisi(default): print score only "
                  "| features: print feature weights and scores separated by white space]")
            .name("mode")
            ;
      }
   }; // struct eval_options

   extern "C" int eval(eval_options eval_opt, yisi_options yisi_opt, phrasesim_options phrasesim_opt);
}

#endif
