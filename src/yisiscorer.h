/**
 * @file yisiscorer.h
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

#ifndef YISISCORER_H
#define YISISCORER_H

#include "srlgraph.h"
#include "yisigraph.h"
#include "srl.h"
#include <string>
#include <vector>
#include <map>
// #include <fstream>
// #include <sstream>
// #include <iostream>
// #include <stdlib.h>

#define DEFAULT_ALPHA 0.5
#define DEFAULT_BETA 0.0
#define DEFAULT_FRAMEWEIGHT_NAME "coverage"


namespace yisi {

   struct yisi_options {
      std::string inpsrl_name_m;
      std::string inpsrl_path_m;
      std::string refsrl_name_m;
      std::string refsrl_path_m;
      std::string hypsrl_name_m;
      std::string hypsrl_path_m;

      std::string labelconfig_path_m;
      std::string weightconfig_path_m;
      std::string frameweight_name_m = DEFAULT_FRAMEWEIGHT_NAME;

      double alpha_m = DEFAULT_ALPHA;
      double beta_m = DEFAULT_BETA;

      void init(com::masaers::cmdlp::parser& p) {
         using namespace com::masaers::cmdlp;

         p.add(make_knob(inpsrl_name_m))
	   .fallback("")
	   .desc("Type of input language SRL: [read|mate]")
	   .name("inpsrl-type")
	   ;
         p.add(make_knob(inpsrl_path_m))
	   .fallback("")
	   .desc("[read: path to assert formated parse of input sentences "
	         "| mate: full path and filename of <srclang>.mplsconfig]")
	   .name("inpsrl-path")
	   ;
         p.add(make_knob(hypsrl_name_m))
	   .fallback("")
	   .desc("Type of output language SRL: [read|mate]")
	   .name("outsrl-type")
	   .name("hypsrl-type")
	   .name("srl-type")
	   ;
         p.add(make_knob(hypsrl_path_m))
	   .fallback("")
	   .desc("[read: path to assert formatted parse output "
	         "| mate: full path and filename of <tgtlang>.mplsconfig]")
	   .name("outsrl-path")
	   .name("hypsrl-path")
	   .name("srl-path")
    ;
         p.add(make_knob(refsrl_name_m))
           .fallback("")
           .desc("Type of reference SRL (specify only if it is different from "
                 "the hypothesis SRL): [read|mate]")
           .name("refsrl-type")
           ;
         p.add(make_knob(refsrl_path_m))
           .fallback("")
           .desc("[read: path to assert formatted parse reference "
                 "| mate: full path and filename of <tgtlang>.mplsconfig]")
           .name("refsrl-path")
           ;
         p.add(make_knob(labelconfig_path_m))
	   .fallback("")
	   .desc("Path to YiSi SRL role label config file")
	   .name("labelconfig-path")
	   ;
         p.add(make_knob(weightconfig_path_m))
	   .fallback("")
	   .desc("Path to YiSi SRL role label config file (default: "
	         "<empty string> to use YiSi unsupervised estimation of weight")
	   .name("weightconfig-path")
	   ;
         p.add(make_knob(frameweight_name_m))
	   .fallback(DEFAULT_FRAMEWEIGHT_NAME)
	   .desc("Type of frame weight function: [uniform|coverage(default)]")
	   .name("frameweight-type")
	   ;
         p.add(make_knob(beta_m))
	   .fallback(DEFAULT_BETA)
	   .desc("Beta value of YiSi [0.0(default)]")
	   .name("beta")
	   ;
         p.add(make_knob(alpha_m))
	   .fallback(DEFAULT_ALPHA)
	   .desc("Ratio of precision & recall in YiSi")
	   .name("alpha")
	   ;
      }
   }; // struct yisi_options

   class yisiscorer_t {
   public:
      yisiscorer_t();
      yisiscorer_t(yisi_options yisi_opt, phrasesim_options phrase_opt);
      ~yisiscorer_t();
      void estimate_weight(std::vector<srlgraph_t> srls);
      void estimate_weight(std::vector<std::vector<srlgraph_t> > msrls);
      std::vector<srlgraph_t> inpsrlparse(std::vector<sent_t*> inpsents);
      std::vector<srlgraph_t> refsrlparse(std::vector<sent_t*> refsents);
      std::vector<srlgraph_t> hypsrlparse(std::vector<sent_t*> hypsents);
      srlgraph_t hypsrlparse(sent_t* hypsent);
      yisigraph_t align(const std::vector<srlgraph_t> refsrlgraph, const srlgraph_t hypsrlgraph);
      yisigraph_t align(const std::vector<srlgraph_t> refsrlgraph,
                        const srlgraph_t hypsrlgraph, const srlgraph_t inpsrlgraph);
      double score(std::string input, std::string hyp);
      double score(yisigraph_t& yg);
      std::vector<double> features(yisigraph_t& yg);

   private:
      double score(yisigraph_t yg, int mode);
      std::vector<double> features(yisigraph_t yg, int mode);
      void compute_features(yisigraph_t yg, std::vector<double> feats,
         double& structure, double& flat, int mode, int refid = -1);
      std::vector<double> pfeatures(yisigraph_t yg);
      std::vector<double> rfeatures(yisigraph_t yg);
      bool match(std::string label1, std::string label2);
      double get_roleweight(yisigraph_t yg, size_t roleid, int mode, int refid = -1);

      phrasesim_t* phrasesim_p;
      srl_t* inpsrl_p;
      srl_t* refsrl_p;
      srl_t* hypsrl_p;

      std::string hypsrl_name_m;
      std::string refsrl_name_m;
      std::string inpsrl_name_m;
      std::string weightconfig_path_m;
      //std::string predweight_name_m;
      std::string frameweight_name_m;
      //std::string prfunc_name_m;

      std::map<std::string, int> label_m;
      std::vector<double> weight_m;
      double alpha_m;
      double beta_m;
   }; // class yisiscorer_t

} // yisi

#endif
