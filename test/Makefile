#!/usr/bin/make

# @file Makefile
# @brief Makefile for the YiSi unit test suite
#
# @author Darlene Stewart
#
# Multilingual Text Processing / Traitement multilingue de textes
# Digital Technologies Research Centre / Centre de recherche en technologies numériques
# National Research Council Canada / Conseil national de recherches Canada
# Copyright 2018, Her Majesty in Right of Canada /
# Copyright 2018, Sa Majeste la Reine du Chef du Canada

SHELL:=/bin/bash

.NOTPARALLEL:
.SECONDARY:

# Override the value of MATEPLUS_HOME with a command line definition, or
# consider defining MATEPLUS_HOME in your .profile, for example:
#   export MATEPLUS_HOME=~/u/sandboxes/mateplus
MATEPLUS_HOME ?= ~/u/tools/MATE/mateplus-master/src

MATEPLUS_PATH ?= $(MATEPLUS_HOME)/mateplus.jar

ifneq ("$(wildcard $(MATEPLUS_PATH))","")
   WITH_SRLMATE ?= True
else
   $(info *** mateplus.jar not found)
endif

BERT_STATUS != python3 -c "import bert" |& grep -o ModuleNotFoundError
ifneq ($(BERT_STATUS)$(wildcard $(BERT_HOME)),ModuleNotFoundError)
   WITH_BERT ?= True
else
   $(info *** bert python module not found)
endif

.PHONY: all
# "make all" should create the .gitignore file
all: gitignore
	@echo All tests PASSED.

test: all

CMDLP_TEST_PROGS := cmdlp_test.1 cmdlp_test.2
SIMPLE_TEST_PROGS := util_test
SIMPLE_TEST_PROGS += phrasesim_test
SIMPLE_TEST_PROGS += lexweight_test
SIMPLE_TEST_PROGS += lexsim_test
SIMPLE_TEST_PROGS += biw2v_test
SIMPLE_TEST_PROGS += w2v_test
SIMPLE_TEST_PROGS += maxmatching_test
SIMPLE_TEST_PROGS += srlutil_test
SIMPLE_TEST_PROGS += srlgraph_test
SIMPLE_TEST_PROGS += yisiscorer_test
SIMPLE_TEST_PROGS += read_sent_test.1 read_sent_test.2 read_sent_test.3

ifdef WITH_SRLMATE
   SIMPLE_SRL_TEST_PROGS := srl_test
   SIMPLE_SRL_TEST_PROGS += srlmate_test
endif

ifdef WITH_BERT
   BERT_TEST_PROGS := call_bert_test
   BERT_TEST_PROGS += bert_test
   BERT_TEST_PROGS += read_sent_test.4
endif

ALL_SIMPLE_TEST_PROGS := $(CMDLP_TEST_PROGS) $(SIMPLE_TEST_PROGS)
ALL_SIMPLE_TEST_PROGS += $(SIMPLE_SRL_TEST_PROGS) $(BERT_TEST_PROGS)

all: $(ALL_SIMPLE_TEST_PROGS)
all: test_yisi

TMP_FILES += $(addsuffix .out, $(ALL_SIMPLE_TEST_PROGS))
TMP_FILES += test_yisi_[0-2]*.out test_hyp.docyisi[0-2]* test_hyp.sntyisi[0-2]*
TMP_FILES += test_ref.en.srl
TMP_FILES += test_hyp.en.bert.tok.emb*

TMP_DIRS += tmp[0-9a-z]*

compare.%: %
	diff $< ref/$< -q

C6_2_SUB := "s/\(-\?[0-9]\.[0-9][0-9]\)[0-9][0-9][0-9][0-9]\b/\1/g;"

compare_2.%: %
	diff <(cat $< | sed -e $(C6_2_SUB)) <(cat ref/$< | sed -e $(C6_2_SUB)) -q

JL_SUB := "s/^[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*  */00.00.000 /"

compare_w_java_log.%: %
	diff <(cat $< | grep -v '^Loading\|^Setting up' | sed -e $(JL_SUB)) \
	     <(cat ref/$< |  grep -v '^Loading\|^Setting up' | sed -e $(JL_SUB)) -q

compare_w_alt.%: %
	diff $< ref/$< -q || diff -s $< ref/$<.alt -q

BL_SUB := "s/\.\/tmp[a-z0-9_]\+/.\/tmp00000000/g; \
           s/ 0x[0-9a-f]\+/ 0x00000000/g; \
           s/[0-9]\+-[0-9]\+-[0-9]\+ [0-9:.]\+/2020-01-01 00:00:00.000000/g; \
           s/Python version: 3.*/Python version: 3./g; \
           s/CPU Frequency: 2[0-9]\+ Hz/CPU Frequency: 2300000000 Hz/g; \
           s/with [0-9][0-9][0-9][0-9][0-9] MB memory/with 10780 MB memory/g;"

C4_2_SUB := "s/\(-\?[0-9]\.[0-9][0-9]\)[0-9][0-9]\b/\1/g;"

C7_3_SUB := "s/\(-\?[0-9]\.[0-9][0-9][0-9]\)[0-9][0-9][0-9][0-9]\b/\1/g;"

compare_w_bert_log.%: %
	diff <(cat $< | grep -v '^\*\*\* Timer:' | sed -e $(BL_SUB) -e $(C4_2_SUB) -e $(C7_3_SUB)) \
	     <(cat ref/$< |  grep -v '^\*\*\* Timer:' | sed -e $(BL_SUB) -e $(C4_2_SUB) -e $(C7_3_SUB)) -q

# Simple tests

.PHONY: $(ALL_SIMPLE_TEST_PROGS)
$(CMDLP_TEST_PROGS): %: compare.%.out
$(SIMPLE_TEST_PROGS): %: compare.%.out
ifdef SIMPLE_SRL_TEST_PROGS
$(SIMPLE_SRL_TEST_PROGS): %: compare_w_java_log.%.out
endif
ifdef BERT_TEST_PROGS
$(BERT_TEST_PROGS): %: compare_w_bert_log.%.out
endif

cmdlp_test.1.out: ARGS = --beta 0 --settings <(echo hello) --refs test_ref.en --help
cmdlp_test.2.out: ARGS = --beta 0 --settings <(echo hello) --refs /tmp/cmdlp_test.2.empty
util_test.out: ARGS =
phrasesim_test.out: ARGS = --lexsim-type w2v --outlexsim-path mini.d300.en \
   --reflexweight-type learn --reflexweight-path test_ref.en --phrasesim-type nwpr --ngram-size 3
lexweight_test.out: ARGS =
lexsim_test.out: ARGS = w2v mini.d300.en EU UK
biw2v_test.out: ARGS = mini.d300.de mini.d300.en . .
w2v_test.out: ARGS = mini.d300.en EU UK
maxmatching_test.out: ARGS =
srlutil_test.out: ARGS = test_ref.en.conll09
srlgraph_test.out: ARGS = test_ref.en test_ref.en.assert
yisiscorer_test.out: ARGS = --lexsim-type w2v --outlexsim-path mini.d300.en \
   --reflexweight-type learn --reflexweight-path test_ref.en --phrasesim-type nwpr --ngram-size 3
srl_test.out: ARGS = mate ../src/en.mplsconfig test_ref.en test_ref.en.srl
srlmate_test.out: ARGS = ../src/en.mplsconfig <<<'Hello there'

# The BERT test files test_hyp.en.bert.{tok,unit,emb} were created as follows:
#    ../src/BERT_wrapper.py test_hyp.en
#    mv test_hyp.en.bert.emb test_hyp.en.bert.emb.orig
#    paste <(cut -f1-2 test_hyp.en.bert.emb.orig) <(cut -f3 test_hyp.en.bert.emb.orig \
#       | cut -d' ' -f1-16) > test_hyp.en.bert.emb
# BERT produces embeddings vectors of size 768; for testing read_sent we keep
# only the first 16 entries.
read_sent_test.1.out: ARGS = word ../test/test_hyp.en.bert.tok
read_sent_test.2.out: ARGS = uemb ../test/test_hyp.en.bert.unit '\#\#u' ../test/test_hyp.en.bert.emb
read_sent_test.3.out: ARGS = unit ../test/test_hyp.en.bert.unit
read_sent_test.4.out: ARGS = bert ../test/test_hyp.en.bert.tok

call_bert_test.out: ARGS = ../test/test_hyp.en.bert.tok

bert_test.out: ARGS = ../test/test_hyp.en.bert.tok

$(CMDLP_TEST_PROGS): BIN = ../src/cmdlp/build/bin
$(filter-out $(CMDLP_TEST_PROGS),$(ALL_SIMPLE_TEST_PROGS)): BIN = ../bin

# call_bert_test produces an extra output file.
compare_w_bert_log.call_bert_test.out: compare_2.test_hyp.en.bert.tok.emb

test_hyp.en.bert.tok.emb: call_bert_test.out ;

# bert_test produces an extra output file.
compare_w_bert_log.bert_test.out: compare_2.test_hyp.en.bert.tok.emb2

test_hyp.en.bert.tok.emb2: bert_test.out ;

# srl_test produces an extra output file.
compare_w_java_log.srl_test.out: compare_w_alt.test_ref.en.srl

test_ref.en.srl: srl_test.out ;

# cmdlp_test.2 requires a file of known size with a fixed absolute path.
cmdlp_test.2.out: | /tmp/cmdlp_test.2.empty

# Note: There is a rather rare race condition where the following touch command
# could fail if two users run this simultaneously. In that case simply running
# make again will succeed.
/tmp/cmdlp_test.2.empty:
	touch $@
	chmod -f o+r $@

$(addsuffix .out, $(ALL_SIMPLE_TEST_PROGS)): %.out:
	$(BIN)/$(firstword $(subst ., ,$*)) $(ARGS) &> $@

# YiSi tests

YSFX_NOSRL := 0 1 2
YSFX_SRL := 1_srl 2_srl

.PHONY: test_yisi
.PHONY: $(foreach n,$(YSFX_NOSRL),test_yisi_$n)
test_yisi: $(foreach n,$(YSFX_NOSRL),test_yisi_$n)

$(foreach n,$(YSFX_NOSRL),test_yisi_$n): test_yisi_%: compare.test_yisi_%.out
$(foreach n,$(YSFX_NOSRL),compare.test_yisi_$n.out): compare.test_yisi_%.out: compare.test_hyp.docyisi% compare.test_hyp.sntyisi%

.PHONY: $(foreach n,$(YSFX_SRL),test_yisi_$n)
ifdef WITH_SRLMATE
test_yisi: $(foreach n,$(YSFX_SRL),test_yisi_$n)
endif

# YiSi SRL tests have alternate reference files too because mateplus may return
# an alternate parse.

$(foreach n,$(YSFX_SRL),test_yisi_$n): test_yisi_%: compare_w_java_log.test_yisi_%.out
$(foreach n,$(YSFX_SRL),compare_w_java_log.test_yisi_$n.out): compare_w_java_log.test_yisi_%.out: compare_w_alt.test_hyp.docyisi% compare_w_alt.test_hyp.sntyisi%

test_yisi_%.out: yisi-%.config
	../bin/yisi --config $< &> $@

test_hyp.docyisi% test_hyp.sntyisi%: test_yisi_%.out
	:

########################################
.PHONY: gitignore
gitignore:
	echo "$(TMP_FILES) $(TMP_DIRS) core core.* .gitignore !src/* !ref/* !in/* !data/*" | tr ' ' '\n' > .gitignore

.PHONY: clean clean.tmp_files
clean: clean.tmp_files
clean.tmp_files:
	$(RM) $(TMP_FILES) core core.* .gitignore

ifdef TMP_DIRS
.PHONY: clean.tmp_dirs
clean: clean.tmp_dirs
clean.tmp_dirs:
	$(RM) -r $(TMP_DIRS)
endif
