tmpfile=$(mktemp ./XXXXXX)

cat - \
    > $tmpfile

/home/loc982/u/tools/Stanford_Chinese_segmenter/stanford-segmenter-2015-12-09/segment.sh -k ctb $tmpfile UTF-8 0

rm $tmpfile
