/**
 * @author Jackie Lo
 * @file srlmate_test.cpp
 * @brief Unit test of srl mate.
 *
 */

#include <iostream>
#include <fstream>

#include "srlutil.h"

using namespace std;
using namespace yisi;

// globals

// main
int main(const int argc, const char* argv[])
{

  auto s = read_conll09batch(argv[1]);
  cout << s[0];
  
  return 0;
}

