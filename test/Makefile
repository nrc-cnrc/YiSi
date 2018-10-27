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

.PHONY: all
# "make all" should create the .gitignore file
all: gitignore
	@echo All tests PASSED.

test: all

all: test_yisi

TMP_FILES = test_yisi_[0-2].out test_hyp.docyisi[0-2] test_hyp.sntyisi[0-2]

compare.%: %
	diff $< ref/$< -q

.PHONY: test_yisi $(foreach n,0 1 2,test_yisi_$n)
test_yisi: $(foreach n,0 1 2,test_yisi_$n)

$(foreach n,0 1 2,test_yisi_$n): test_yisi_%: compare.test_yisi_%.out
$(foreach n,0 1 2,compare.test_yisi_$n.out): compare.test_yisi_%.out: compare.test_hyp.docyisi% compare.test_hyp.sntyisi%

test_yisi_%.out: yisi-%.config
	../bin/run_yisi.sh $< &> $@

test_hyp.docyisi% test_hyp.sntyisi%: test_yisi_%.out
	:

########################################
.PHONY: gitignore
gitignore: 
	echo "${TMP_FILES} ${TMP_DIRS} core core.* .gitignore !src/* !ref/* !in/* !data/*" | tr ' ' '\n' > .gitignore

.PHONY: clean clean.tmp_files
clean: clean.tmp_files
clean.tmp_files:
	${RM} ${TMP_FILES} core core.* .gitignore

ifdef TMP_DIRS
.PHONY: clean.tmp_dirs
clean: clean.tmp_dirs
clean.tmp_dirs:
	${RM} -r ${TMP_DIRS}
endif
