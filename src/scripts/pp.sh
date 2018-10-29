#!/bin/bash

# @file pp.sh
# @brief Preprocess text by normalizing punctuation and tokenizing.
#
# @author Jackie Lo
#
# Traitement multilingue de textes / Multilingual Text Processing
# Centre de recherche en technologies numériques / Digital Technologies Research Centre
# Conseil national de recherches Canada / National Research Council Canada
# Copyright 2018, Sa Majeste la Reine du Chef du Canada /
# Copyright 2018, Her Majesty in Right of Canada

YISIBIN=/path/to/your/yisi/bin

cat - \
    | sed -f $YISIBIN/norm_punc.sed \
    | $YISIBIN/tok.pl
