cat - \
    | sed -f $YISIBIN/norm_punc.sed \
    | $YISIBIN/tok.pl
