/**
 * @author Jackie Lo
 * @file lexsim_test.cc
 * @brief Unit test of srlgraph.
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
  lexsim_t w2vtxt("biw2v", argv[1], argv[2]);

  while (true) {
    cout << "Intput two string:" <<endl;
    string s1;
    string s2;
    cin >> s1;
    cin >> s2;
    cout << "Sim = " << w2vtxt.get_sim(s1, s2, yisi::INP_MODE)<<endl;
  }
  return 0;
}

