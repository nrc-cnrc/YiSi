/**
 * @file yisigraph.h
 * @brief YiSi graph
 *
 * @author Jackie Lo
 *
 * Class definition of one class:
 *    - yisigraph_t
 * and the function definition for the YiSi alignment algorithm.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#ifndef YISIGRAPH_H
#define YISIGRAPH_H

#include "srlgraph.h"
#include "phrasesim.h"
#include "maxmatching.h"

#include <utility>
#include <string>
#include <vector>
#include <iostream>

namespace yisi {

   class yisigraph_t {
   public:
      typedef srlgraph_t::span_type span_type;
      typedef srlgraph_t::label_type label_type;
      typedef srlgraph_t::srlnode_type srlnode_type;
      typedef srlgraph_t::srledge_type srledge_type;
      typedef srlgraph_t::srlnid_type srlnid_type;
      typedef srlgraph_t::srleid_type srleid_type;
      typedef std::pair<srlnid_type, double> alignment_type;

      yisigraph_t() {}
      yisigraph_t(const std::vector<srlgraph_t> refsrlgraph, const srlgraph_t hypsrlgraph);
      yisigraph_t(const std::vector<srlgraph_t> refsrlgraph, const srlgraph_t hypsrlgraph, const srlgraph_t inpsrlgraph);
      yisigraph_t(const yisigraph_t& rhs);
      void operator = (const yisigraph_t& rhs);

      void align(phrasesim_t* phrasesim);

      bool withinp();
      size_t get_refsize();
      // double get_sentlength(int mode, int refid=-1);
      double get_sentsim(int mode, int refid=-1);
      std::vector<srlnid_type> get_preds(int mode, int refid=-1);
      std::vector<srlnid_type> get_args(srlnid_type roleid, int mode, int refid=-1);
      // std::vector<std::string>& get_sentence(int mode, int refid=-1);
      std::vector<std::string> get_role_filler_units(srlnid_type roleid, int mode, int refid=-1);
      double get_rolespanlength(srlnid_type roleid, int mode, int refid=-1);
      label_type get_rolelabel(srlnid_type roleid, int mode, int refid=-1);
      std::vector<std::pair<int, alignment_type> > get_hypalignment(srlnid_type roleid);
      double get_alignsim(srlnid_type roleid, int mode, int refid=-1);
      label_type get_alignlabel(srlnid_type roleid, int mode, int refid=-1);
      double get_refnormlmscore();
      double get_hypnormlmscore();
      double get_inpnormlmscore();
      double get_reflmscore();
      double get_hyplmscore();
      double get_inplmscore();

      void print(std::ostream& os);

   private:
      double spanlength(span_type span);
      std::vector<srlgraph_t> refsrlgraph_m;
      srlgraph_t hypsrlgraph_m;
      srlgraph_t inpsrlgraph_m;

      std::vector<std::map<srlnid_type, alignment_type> > refalignment_m;
      std::map<srlnid_type, std::vector<std::pair<int, alignment_type> > > hypalignment_m;
      std::map<srlnid_type, alignment_type> inpalignment_m;
      bool inp_b;
  }; // class yisigraph_t


   std::ostream& operator<<(std::ostream& os, const yisi::yisigraph_t& m);

} // yisi



#endif
