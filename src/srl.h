/**
 * @author Jackie Lo
 * @file srl.h semantic role labelers.
 *
 *
 * COMMENTS:
 *
 * Class definition of srl classes:
 *    - srl_t (wrapper class)
 *    - srlmodel_t (abstract base class of different semantic role labelers)
 * and the declaration of some utility funcions working on it.
 *
 *
 * Technologies langagieres interactives / Interactive Language Technologies
 * Inst. de technologie de l'information / Institute for Information Technology
 * Conseil national de recherches Canada / National Research Council Canada
 * Copyright 2005, Sa Majeste la Reine du Chef du Canada /
 * Copyright 2005, Her Majesty in Right of Canada
 */

#ifndef SRL_H
#define SRL_H

#include "srlgraph.h"
#include "srlutil.h"
#include <string>
#include <vector> 
#include <iostream>

namespace yisi {

  class srl_t {
  public:
    srl_t();
    srl_t(const std::string name, const std::string path="");
    ~srl_t();
    srlgraph_t parse(std::string sent);
    std::vector<srlgraph_t> parse(std::vector<std::string> sents);
  private:
    srlmodel_t* srl_p;
  };

} // yisi

#endif
