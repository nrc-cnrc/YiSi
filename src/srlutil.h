/**
 * @author Jackie Lo
 * @file srlutil.h SRL utility functions.
 *
 *
 * COMMENTS:
 *
 * The declaration of some utility class and funcions working on srl.
 *
 */

#ifndef SRLUTIL_H
#define SRLUTIL_H

#include "srlgraph.h"

#include <string>
#include <vector> 
#include <map>

namespace yisi {

  std::vector<srlgraph_t> read_srl(std::vector<std::string> sents, std::string parsefile);
  srlgraph_t read_conll09(std::string parse);
  void resolve_arg_span(std::map<int, std::vector<int> > child, int curid, srlgraph_t::srlnid_type pid, size_t& b, size_t&e);
  std::vector<srlgraph_t> read_conll09batch(std::string filename);
  
  class srlmodel_t {
  public:
    srlmodel_t(){};
    virtual ~srlmodel_t(){};
    virtual srlgraph_t parse(std::string){
      std::cerr<<"ERROR: type of semantic role labeler does not support individual sentence parsing. Exiting..."<<std::endl;
      exit(1);
    };
    virtual std::vector<srlgraph_t> parse(std::vector<std::string>)=0;
  };

  class srlread_t:public srlmodel_t{
  public:
    srlread_t(){};
    srlread_t(std::string parsefile);
    virtual std::vector<srlgraph_t> parse(std::vector<std::string> sents);
  private:
    std::string parsefile_m;
  };

  class srltok_t:public srlmodel_t{
  public:
    srltok_t(){};
    virtual srlgraph_t parse(std::string sent);
    virtual std::vector<srlgraph_t> parse(std::vector<std::string> sents);
  private:
  };


} // yisi

#endif
