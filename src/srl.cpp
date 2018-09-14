/**
 * @author Jackie Lo
 * @file srl.cpp semantic role labelers.
 *
 *
 * COMMENTS:
 *
 * Class implementation for the srl classes:
 *    - srl_t (wrapper class)
 *
 */
#include "srlgraph.h"
#include "srlutil.h"
#include "srlmate.h"
#include "srl.h"

using namespace yisi;
using namespace std;

srl_t::srl_t(){
  srl_p = new srlread_t();
}

srl_t::srl_t(const string name, const string path){
  if (name == "read"){
    srl_p = new srlread_t(path);
  } else if (name == "mate"){
    srl_p = new srlmate_t(path);
  } else if (name == ""){
    srl_p = new srltok_t();
  } else {
    cerr<<"ERROR: unknown srl model name " <<name<<endl;
  }
}

srl_t::~srl_t(){
  //cerr <<"Deleting srl..."<<endl;
  if (srl_p != NULL){
    delete srl_p;
    srl_p = NULL;
  }
}

srlgraph_t srl_t::parse(string sent){
  return srl_p->parse(sent);
}

vector<srlgraph_t> srl_t::parse(vector<string> sents){
  return srl_p->parse(sents);
}
