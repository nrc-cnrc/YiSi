cat - \
    | perl -ple "s/([^ -~\t][^ -~\t][^ -~\t])/ \1 /g;s/  / /g;s/^ //"
