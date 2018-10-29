#!/bin/bash

# @file run_pp+matein+yisi.sh
# @brief Run preprocessing, srlmate, and YiSi, given a config file.
#
# @author Jackie Lo
#
# Traitement multilingue de textes / Multilingual Text Processing
# Centre de recherche en technologies numériques / Digital Technologies Research Centre
# Conseil national de recherches Canada / National Research Council Canada
# Copyright 2018, Sa Majeste la Reine du Chef du Canada /
# Copyright 2018, Her Majesty in Right of Canada

################

YISIBIN=/path/to/your/yisi/bin

# Check if YiSi is on the PATH.
if [[ $(which yisi 2> /dev/null) ]]; then
   YISIBIN_PFX=""
else
   YISIBIN_PFX="${YISIBIN_PFX}"
fi

TESTCODE=`${YISIBIN_PFX}testbin`
if [[ $TESTCODE != "0" ]]; then
    echo "ERROR: fail to run YiSi test program. Exiting..."
    exit 1
fi

yisiflags=`${YISIBIN_PFX}resolve_yisicmd.sh $1`
source $1
reffile_raw=`echo $reffile | sed "s/yisi_tok/raw/g"`
hypfile_raw=`echo $hypfile | sed "s/yisi_tok/raw/g"`
inpfile_raw=`echo $inpfile | sed "s/yisi_tok/raw/g"`
cd ~/u/tmp
reffile_tmp=`mktemp`
hypfile_tmp=`mktemp`
inpfile_tmp=`mktemp`
inpfile_pas=`mktemp`
cd -
echo "Running: ${YISIBIN_PFX}yisi $yisiflags"
date +%s > $docscorefile.bt 
if [ "$reffile" != "" ]; then
    cat $reffile_raw | ${YISIBIN_PFX}pp.sh > ~/u/tmp/$reffile_tmp
fi
if [ "$hypfile" != "" ]; then
    cat $hypfile_raw | ${YISIBIN_PFX}pp.sh > ~/u/tmp/$hypfile_tmp
fi
if [ "$inpfile" != "" ]; then
    cat $inpfile_raw | ${YISIBIN_PFX}pp.sh > ~/u/tmp/$inpfile_tmp
    ${YISIBIN_PFX}srlmate_test $YISIBIN/../src/$srclang.mplsconfig < $inpfile > ~/u/tmp/$inpfile_pas
fi

${YISIBIN_PFX}yisi $yisiflags

date +%s > $docscorefile.et
rm ~/u/tmp/$reffile_tmp ~/u/tmp/$hypfile_tmp ~/u/tmp/$inpfile_tmp ~/u/tmp/$inpfile_pas
