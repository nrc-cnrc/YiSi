/**
 * @author Jackie Lo
 * @file srlmate.h SRL MATE.
 *
 *
 * COMMENTS:
 *
 * Class definition of srlmate (wrapper class for MATE srl tool)
 * ***IMPORTANT***: the path for the constructor here is the config file of MATE for the specific language.
 * Please edit the config file in MATE correctly for calling the right model.
 *
 */

#ifndef SRLMATE_H
#define SRLMATE_H

#include "srlgraph.h"
#include "srlutil.h"
#include <jni.h>

#include <string>
#include <vector> 

namespace yisi {

  class srlmate_t:public srlmodel_t{
  public:
    srlmate_t(){};
    srlmate_t(std::string path);
    ~srlmate_t();
    std::string jrun(std::string sent);
    srlgraph_t parse(std::string sent);
    virtual std::vector<srlgraph_t> parse(std::vector<std::string> sents);
  private:
    std::string noparse(std::vector<std::string> tokens);
    JavaVM* jvm_m;
    JNIEnv* jen_m;
    jclass mate_class_m;
    jobject mate_object_m;
  };

} // yisi

#endif
