/**
 * @author Jackie Lo
 * @file srlgraph_test.cpp
 * @brief Unit test of srlgraph.
 *
 */

#include "srlutil.h"

#include <iostream>
#include <fstream>
#include <string>

#include <vector>

using namespace std;
using namespace yisi;

// globals

// main

int main(int argc, char* argv[])
{
  ifstream txtstr(argv[1], ifstream::in);

  vector<string> sents;
  string line;
  
  
  while (getline(txtstr, line)){
    sents.push_back(line);
  }
  
  cout<<"reading ASSERT format parse file"<<endl;

  vector<srlgraph_t> srls = read_srl(sents, string(argv[2]));
  cout<<"print srlparses"<<endl;
  
  for (vector<srlgraph_t>::iterator it = srls.begin(); it != srls.end(); it++){
    cout << (*it);
  }
  
  return 0;
}

