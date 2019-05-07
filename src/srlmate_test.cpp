/**
 * @file srlmate_test.cpp
 * @brief Unit test for srlmate.
 *
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include <iostream>
#include <fstream>

#include "srlmate.h"

using namespace std;
using namespace yisi;

int main(const int argc, const char* argv[])
{
   srlmate_t mate(argv[1]);
   string sent;

   while (getline(cin, sent)) {
     sent_t* s = new sent_t("word");
     s->set_tokens(tokenize(sent));
     string mateout = mate.jrun(s);
     cout << mateout << endl << endl;
     srlgraph_t result = read_conll09(mateout, s);
     cerr << result << endl;
     delete s;
     s = NULL;
   }

   return 0;
}

