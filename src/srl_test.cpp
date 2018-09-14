/**
 * @author Jackie Lo
 * @file srl_test.cpp
 * @brief Unit test of srl.
 *
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "srl.h"

using namespace std;
using namespace yisi;

// globals

// main
int main(const int argc, const char* argv[])
{

  if (argc == 1){
    srl_t mate("mate", "/home/loc/tools/MATE/srl-20131216/scripts/parse_full_es.sh");
    
    vector<string> sents;
    
    ifstream IN("test_es.txt");
    while (!IN.eof()){
      string line;
      getline(IN, line);
      if (line != ""){
	sents.push_back(line);
      }
    }
    auto r = mate.parse(sents);
    cout<<"Done parsing "<< r.size() <<" srlgraphes."<<endl;
    for (auto it=r.begin(); it!=r.end(); it++){
      cout<<*it;
    }
  } else {
    srl_t parser(argv[1], argv[2]);
    vector<string> sents;
    ifstream IN(argv[3]);
    while(!IN.eof()){
      string line;
      getline(IN, line);
      if (line !=""){
	sents.push_back(line);
      }
    }
    IN.close();
    ofstream OUT(argv[4]);
    auto r=parser.parse(sents);
    cout<<"Done parsing "<< r.size() <<" srlgraphes."<<endl;
    for (int i=0; i<(int)r.size(); i++){
      r[i].print(OUT, i);
    }
    OUT.close();
  }
    
  return 0;
}

