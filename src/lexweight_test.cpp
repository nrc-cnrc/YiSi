/**
 * @author Jackie Lo
 * @file lexweight_test.cpp
 * @brief Unit test of lexweight.
 *
 */

#include "lexweight.h"
#include "util.h"

#include <iostream>
#include <fstream>
#include <vector>


using namespace std;
using namespace yisi;

// globals

// main

int main(int argc, char* argv[])
{

  if (argc == 1){
    vector<vector<string > > tokens;
    tokens.push_back(tokenize("Wondering where you left your umbrella ?"));
    tokens.push_back(tokenize("Your laptop ?"));
    tokens.push_back(tokenize("Your wallet ?"));
    tokens.push_back(tokenize("Not to worry ."));
    tokens.push_back(tokenize("Toronto Pearson offers a secure Lost and Found service for all passengers and employees ."));
  
    lexweight_t idf(tokens);
    double w1 = idf("?");
    double w2 = idf("offers");
    cout <<"The idf for '?' is "<<w1<<endl;
    cout <<"The idf for 'offers' is "<<w2<<endl;
  } else {
    lexweight_t idf("learn", argv[1]);
    if (argc == 2){
      idf.write(cout);
    } else {
      ofstream fout;
      fout.open(argv[2]);
      idf.write(fout);
    }
  }
  return 0;
}

