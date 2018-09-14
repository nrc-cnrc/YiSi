/**
 * @author Jackie Lo
 * @file lexsim_test.cc
 * @brief Unit test of lexsim.
 *
 */

#include "lexsim.h"

#include <iostream>

using namespace std;
using namespace yisi;

// globals

// main

int main(int argc, char* argv[])
{
  string lsname=argv[1];
  string lspath;
  if (argc > 2){
    lspath=argv[2];
  }
  lexsim_t l(lsname, lspath);
  string s1, s2;
  while (true){
    cout <<"Input two strings:"<<endl;
    cin >> s1;
    cin >> s2;

    cout <<lsname <<" similarity of `"<< s1 << "' and `"<<s2 <<"': "<< l.get_sim(s1, s2, yisi::REF_MODE)<<endl;
  }

  /*
  cout <<"Constructing ibm"<<endl;
  lexsim_t ibm("ibm", ibm_path.c_str());
  cout<<"IBM score of `olive' and `olivo':"<< ibm("olive", "olivo")<<endl;
  */

  /*
  cout <<"Constructing exact"<<endl;
  lexsim_t ex("exact");
  cout <<"Exact similarity of `France' and `Italy':" <<ex("France", "Italy")<<endl;
  cout <<"Exact similarity of `France' and `France':" <<ex("France", "France")<<endl;
  */

  return 0;
}

