/**
 * @author Jackie Lo
 * @file yisiscorer_test.cpp
 * @brief Unit test of yisiscorer.
 *
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "util.h"
#include "yisiscorer.h"
#include "srl.h"

using namespace std;
using namespace yisi;

// globals

// main
int main(const int argc, const char* argv[])
{

  typedef cmdlp::options<yisi_options, phrasesim_options> options_type;

  options_type opt(argc,argv);

  yisiscorer_t<options_type> yisi(opt);

  string reffile("test_ref.txt");
  vector<string> sents = read_file(reffile);

  auto r1 = yisi.refsrlparse(sents);
  auto r2 = yisi.hypsrlparse(sents);
  for (size_t i=0; i<r1.size(); i++){
    //cerr <<"Building YiSi graph"<<endl;
    vector<srlgraph_t> rs;
    rs.push_back(r1[i]);
    yisigraph_t m = yisi.align(rs,r2[i]);

    cout << "YiSi score is:" << yisi.score(m) <<endl;
  }
}

