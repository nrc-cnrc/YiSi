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

.PHONY: all
# "make all" should create the .gitignore file
all: gitignore
	@echo All tests PASSED.

test: all

SIMPLE_TEST_PROGS := cmdlp_test
SIMPLE_TEST_PROGS += util_test
#SIMPLE_TEST_PROGS += lw_count
SIMPLE_TEST_PROGS += phrasesim_test
SIMPLE_TEST_PROGS += lexweight_test
#SIMPLE_TEST_PROGS += lexsim_test
#SIMPLE_TEST_PROGS += biw2v_test
#SIMPLE_TEST_PROGS += w2v_test
SIMPLE_TEST_PROGS += maxmatching_test
#SIMPLE_TEST_PROGS += srlutil_test
SIMPLE_TEST_PROGS += srlgraph_test
#SIMPLE_TEST_PROGS += srl_test
SIMPLE_TEST_PROGS += yisiscorer_test

ifdef WITH_SRLMATE
   SIMPLE2_TEST_PROGS += srlmate_test
endif

all: $(SIMPLE_TEST_PROGS)
all: $(SIMPLE2_TEST_PROGS)
all: test_yisi

TMP_FILES += $(addsuffix .out, $(SIMPLE_TEST_PROGS) $(SIMPLE2_TEST_PROGS))
TMP_FILES += test_yisi_[0-2]*.out test_hyp.docyisi[0-2]* test_hyp.sntyisi[0-2]*

compare.%: %
	diff $< ref/$< -q

SUB:="s/^[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*  */00.00.000 /"

compare2.%: %
	diff <(cat $< | sed -e $(SUB)) <(cat ref/$< | sed -e $(SUB)) -q

.PHONY: $(SIMPLE_TEST_PROGS) $(SIMPLE2_TEST_PROGS)
$(SIMPLE_TEST_PROGS): %: compare.%.out
$(SIMPLE2_TEST_PROGS): %: compare2.%.out

cmdlp_test.out: ARGS = --beta 0 --help
#srl_test.out: ARGS = mate ../src/en.mplsconfig
srlmate_test.out: ARGS = ../src/en.mplsconfig <<<'Hello there'

$(addsuffix .out, $(SIMPLE_TEST_PROGS) $(SIMPLE2_TEST_PROGS)): %.out:
	../bin/$* $(ARGS) &> $@

YSFX_NOSRL := 0 1 2
#YSFX_SRL := 1_srl 2_srl
YSFX_SRL := 1_srl

.PHONY: test_yisi $(foreach n,$(YSFX_NOSRL) $(YSFX_SRL),test_yisi_$n)
test_yisi: $(foreach n,$(YSFX_NOSRL) $(YSFX_SRL),test_yisi_$n)

$(foreach n,$(YSFX_NOSRL),test_yisi_$n): test_yisi_%: compare.test_yisi_%.out
$(foreach n,$(YSFX_NOSRL),compare.test_yisi_$n.out): compare.test_yisi_%.out: compare.test_hyp.docyisi% compare.test_hyp.sntyisi%

$(foreach n,$(YSFX_SRL),test_yisi_$n): test_yisi_%: compare2.test_yisi_%.out
$(foreach n,$(YSFX_SRL),compare2.test_yisi_$n.out): compare2.test_yisi_%.out: compare.test_hyp.docyisi% compare.test_hyp.sntyisi%

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
