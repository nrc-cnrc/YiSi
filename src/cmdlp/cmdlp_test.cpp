/**
 * @author Jackie Lo
 * @file cmdlp_test.cpp Unit test for cmdlp class.
 *
 *
 * COMMENTS:
 *
 * Unit test of the command line parser class.
 *
 */

#include "cmdlp.h"
#include <iostream>
#include <string>

int main(const int argc, const char** argv) {
  using namespace std;
  int alpha = 10;
  bool flag;
  bool neg_flag = true;
  char* path;

  cmdlp::parser p;
  p.add(cmdlp::value_option<int>(alpha))
    .desc("The alpha value.")
    .name('a', "alpha")
    .name("ALPHA")
    // .name('a')
    .fallback()
    ;
  p.add(cmdlp::value_option<bool>(flag))
    .desc("A flag.")
    .name('f')
    .name("flag")
    ;
  p.add(cmdlp::value_option<bool>(neg_flag))
    .desc("A negative flag.")
    .name("neg_flag")
    .name('F')
    .name("FLAG")
    ;
  p.add(cmdlp::value_option<char*>(path))
    .desc("The path name.")
    .name('p', "path")
    .name("PATH")
    .fallback()
    ;
  
    
  cout << p.help() << endl;
  p.parse(argc, argv);
  cout << p.help() << endl;
  
  return EXIT_SUCCESS;
}


