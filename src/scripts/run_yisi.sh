
################
TESTCODE=`$YISIBIN/testbin`
if [[ $TESTCODE != "0" ]]; then
    echo "ERROR: fail to run YiSi test program. Exiting..."
    exit 1
fi

yisiflags=`$YISIBIN/resolve_yisicmd.sh $1`
echo "Running: $YISIBIN/yisi $yisiflags"
# date +%s > $docscorefile.bt 
$YISIBIN/yisi $yisiflags
# date +%s > $docscorefile.et
