/**
 * @author Jackie Lo
 * @file srlmate_test.cpp
 * @brief Unit test of srl mate.
 *
 */

#include <iostream>
#include <fstream>

#include "srlmate.h"

using namespace std;
using namespace yisi;

// globals

// main
int main(const int argc, const char* argv[])
{

  srlmate_t mate(argv[1]);
  string sent;
  while (getline(cin, sent)){
    string mateout = mate.jrun(sent);
    cout<<mateout<<endl<<endl;
    srlgraph_t result = read_conll09(mateout);
    cerr<<result<<endl;
  }
  
  return 0;
}

