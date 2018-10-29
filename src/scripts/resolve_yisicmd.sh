#!/bin/bash

# @file resolve_yisicmd.sh
# @brief Convert YiSi config file to YiSi command arguments.
#
# @author Jackie Lo
#
# Traitement multilingue de textes / Multilingual Text Processing
# Centre de recherche en technologies numériques / Digital Technologies Research Centre
# Conseil national de recherches Canada / National Research Council Canada
# Copyright 2018, Sa Majeste la Reine du Chef du Canada /
# Copyright 2018, Her Majesty in Right of Canada

source <(cat $1 | sed -e 's/-\(.*=\)/\1/g;')

if [[ $reffile != "" ]]; then
    yisiflags="$yisiflags --ref-file $reffile"
fi

if [[ $hypfile != "" ]]; then
    yisiflags="$yisiflags --hyp-file $hypfile"
fi

if [[ $inpfile != "" ]]; then
    yisiflags="$yisiflags --inp-file $inpfile"
fi

if [[ $sntscorefile != "" ]]; then
    yisiflags="$yisiflags --sntscore-file $sntscorefile"
fi

if [[ $docscorefile != "" ]]; then
    yisiflags="$yisiflags --docscore-file $docscorefile"
fi

if [[ $outsrlname != "" ]]; then
    yisiflags="$yisiflags --outsrl-name $outsrlname"
fi

if [[ $outsrlpath != "" ]]; then
    yisiflags="$yisiflags --outsrl-path $outsrlpath"
fi

if [[ $inpsrlname != "" ]]; then
    yisiflags="$yisiflags --inpsrl-name $inpsrlname"
fi

if [[ $inpsrlpath != "" ]]; then
    yisiflags="$yisiflags --inpsrl-path $inpsrlpath"
fi

if [[ $labelconfig != "" ]]; then
    yisiflags="$yisiflags --labelconfig-path $labelconfig"
fi

if [[ $weightconfig != "" ]]; then
    yisiflags="$yisiflags --weightconfig-path $weightconfig"
fi

if [[ $frameweightname != "" ]]; then
    yisiflags="$yisiflags --frameweight-name $frameweightname"
fi

if [[ $lexsimname != "" ]]; then
    yisiflags="$yisiflags --lexsim-name $lexsimname"
fi

if [[ $inplexsimpath != "" ]]; then
    yisiflags="$yisiflags --inplexsim-path $inplexsimpath"
fi

if [[ $outlexsimpath != "" ]]; then
    yisiflags="$yisiflags --outlexsim-path $outlexsimpath"
fi

if [[ $reflexweightname != "" ]]; then
    yisiflags="$yisiflags --reflexweight-name $reflexweightname"
fi

if [[ $reflexweightpath != "" ]]; then
    yisiflags="$yisiflags --reflexweight-path $reflexweightpath"
fi

if [[ $hyplexweightname != "" ]]; then
    yisiflags="$yisiflags --hyplexweight-name $hyplexweightname"
fi

if [[ $hyplexweightpath != "" ]]; then
    yisiflags="$yisiflags --hyplexweight-path $hyplexweightpath"
fi

if [[ $inplexweightname != "" ]]; then
    yisiflags="$yisiflags --inplexweight-name $inplexweightname"
fi

if [[ $inplexweightpath != "" ]]; then
    yisiflags="$yisiflags --inplexweight-path $inplexweightpath"
fi

if [[ $phrasesimname != "" ]]; then
    yisiflags="$yisiflags --phrasesim-name $phrasesimname"
fi

if [[ $alpha != "" ]]; then
    yisiflags="$yisiflags --alpha $alpha"
fi

if [[ $beta != "" ]]; then
    yisiflags="$yisiflags --beta $beta"
fi

if [[ $mode != "" ]]; then
    yisiflags="$yisiflags --mode $mode"
fi

if [[ $ngramsize != "" ]]; then
    yisiflags="$yisiflags --ngram-size $ngramsize"
elif [[ $n != "" ]]; then
    yisiflags="$yisiflags --n $n"
fi

echo "$yisiflags" | sed "s/^ //"
