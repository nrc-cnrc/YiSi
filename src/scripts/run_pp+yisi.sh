
################
TESTCODE=`$YISIBIN/testbin`
if [[ $TESTCODE != "0" ]]; then
    echo "ERROR: fail to run YiSi test program. Exiting..."
    exit 1
fi

yisiflags=`$YISIBIN/resolve_yisicmd.sh $1`
source $1
reffile_raw=`echo $reffile | sed "s/yisi_tok/raw/g"`
hypfile_raw=`echo $hypfile | sed "s/yisi_tok/raw/g"`
inpfile_raw=`echo $inpfile | sed "s/yisi_tok/raw/g"`
cd ~/u/tmp
reffile_tmp=`mktemp`
hypfile_tmp=`mktemp`
inpfile_tmp=`mktemp`
cd -
echo "Running: $YISIBIN/yisi $yisiflags"
date +%s > $docscorefile.bt 
if [ "$reffile" != "" ]; then
    cat $reffile_raw | $YISIBIN/pp.sh > ~/u/tmp/$reffile_tmp
fi
if [ "$hypfile" != "" ]; then
    cat $hypfile_raw | $YISIBIN/pp.sh > ~/u/tmp/$hypfile_tmp
fi
if [ "$inpfile" != "" ]; then
    cat $inpfile_raw | $YISIBIN/pp.sh > ~/u/tmp/$inpfile_tmp
fi

$YISIBIN/yisi $yisiflags

date +%s > $docscorefile.et
rm ~/u/tmp/$reffile_tmp ~/u/tmp/$hypfile_tmp ~/u/tmp/$inpfile_tmp