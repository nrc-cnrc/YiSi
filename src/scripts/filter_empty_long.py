#!/usr/bin/env python
# -*- coding: utf-8 -*-

# @file filter_empty_long.py
# @brief Filter out long sentence pairs (length > 1000 on either side).
#
# @author Jackie Lo
#
# Traitement multilingue de textes / Multilingual Text Processing
# Centre de recherche en technologies numériques / Digital Technologies Research Centre
# Conseil national de recherches Canada / National Research Council Canada
# Copyright 2018, Sa Majeste la Reine du Chef du Canada /
# Copyright 2018, Her Majesty in Right of Canada

import sys

SRC=open(sys.argv[1],"r").readlines()
REF=open(sys.argv[2],"r").readlines()
SOUT=open(sys.argv[3],"w")
ROUT=open(sys.argv[4],"w")

for i in range(len(SRC)):
    s=SRC[i].strip()
    r=REF[i].strip()
    ls = len(s)
    lr = len(r)
    if ((ls>0 and lr>0) and (ls <1000 and lr<1000)):
        SOUT.write(SRC[i])
        ROUT.write(REF[i])
