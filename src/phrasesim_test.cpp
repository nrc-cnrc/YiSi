/**
 * @author Jackie Lo
 * @file phrasesim_test.cpp
 * @brief Unit test of phrasesim.
 *
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "phrasesim.h"

using namespace std;
using namespace yisi;

// globals

// main
int main(const int argc, const char* argv[])
{

  typedef cmdlp::options<phrasesim_options> options_type;

  options_type opt(argc,argv);

  phrasesim_t<options_type> phrasesim(opt);
  
  vector<string> s1;
  s1.push_back("France");
  vector<string> s2;
  s2.push_back("Italy");
  vector<string> s3;
  s3.push_back("French");
  s3.push_back("president");
  vector<string> s4;
  s4.push_back("Italian");
  s4.push_back("president");
  auto p1 = phrasesim(s1, s2, yisi::REF_MODE);
  auto p2 = phrasesim(s3, s4, yisi::REF_MODE);
  cout <<"The similarity of 'France' and 'Italy' is: (" << p1.first <<","<< p1.second<<")"<<endl;
  cout <<"The similarity of 'French president' and 'Italian president' is: (" << p2.first <<","<<p2.second<<")" <<endl;

}

