/**
 * @file util_test.cpp
 * @brief Unit test for util module.
 *
 * @author Darlene Stewart
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include <iostream>
#include <vector>
#include <string>

#include "util.h"

using namespace std;
using namespace yisi;


void test_tokenize()
{
  vector<string> toks;

  toks = tokenize("");
  cout << "tokenize(\"\"): " << toks.size() << " /" << join(toks, "/") << "/" << endl;
  toks = tokenize("a b");
  cout << "tokenize(\"a b\"): " << toks.size() << " /" << join(toks, "/") << "/" << endl;
  toks = tokenize("", ':');
  cout << "tokenize(\"\", ':'): " << toks.size() << " /" << join(toks, "/") << "/" << endl;
  toks = tokenize("a", ':');
  cout << "tokenize(\"a\", ':'): " << toks.size() << " /" << join(toks, "/") << "/" << endl;
  toks = tokenize("a:b", ':');
  cout << "tokenize(\"a:b\", ':'): " << toks.size() << " /" << join(toks, "/") << "/" << endl;
  toks = tokenize("a:", ':');
  cout << "tokenize(\"a:\", ':'): " << toks.size() << " /" << join(toks, "/") << "/" << endl;
  toks = tokenize(":b", ':');
  cout << "tokenize(\":b\", ':'): " << toks.size() << " /" << join(toks, "/") << "/" << endl;
  toks = tokenize(":", ':');
  cout << "tokenize(\":\", ':'): " << toks.size() << " /" << join(toks, "/") << "/" << endl;

  toks = tokenize("", ' ', true);
  cout << "tokenize(\"\", ' ', true): " << toks.size() << " /" << join(toks, "/") << "/" << endl;
  toks = tokenize("a b", ' ', true);
  cout << "tokenize(\"a b\", ' ', keep_empty=true): " << toks.size() << " /" << join(toks, "/") << "/" << endl;
  toks = tokenize("", ':', true);
  cout << "tokenize(\"\", ':', true): " << toks.size() << " /" << join(toks, "/") << "/" << endl;
  toks = tokenize("a", ':', true);
  cout << "tokenize(\"a\", ':', true): " << toks.size() << " /" << join(toks, "/") << "/" << endl;
  toks = tokenize("a:b", ':', true);
  cout << "tokenize(\"a:b\", ':', true): " << toks.size() << " /" << join(toks, "/") << "/" << endl;
  toks = tokenize("a:", ':', true);
  cout << "tokenize(\"a:\", ':', true): " << toks.size() << " /" << join(toks, "/") << "/" << endl;
  toks = tokenize(":b", ':', true);
  cout << "tokenize(\":b\", ':', true): " << toks.size() << " /" << join(toks, "/") << "/" << endl;
  toks = tokenize(":", ':', true);
  cout << "tokenize(\":\", ':', true): " << toks.size() << " /" << join(toks, "/") << "/" << endl;
}

int main(const int argc, const char* argv[])
{
  vector<string> s0;
  vector<string> s1{"France"};
  vector<string> s2{"Italy"};
  vector<string> s3{"French", "president"};
  vector<string> s4{"Italian", "president"};

  cout << join(s0).length() << " join(s0): " << join(s0) << endl;
  cout << join(s1).length() << " join(s1): " << join(s1) << endl;
  cout << join(s2).length() << " join(s2): " << join(s2) << endl;
  cout << join(s3).length() << " join(s3): " << join(s3) << endl;
  cout << join(s4).length() << " join(s4): " << join(s4) << endl;

  test_tokenize();
}
