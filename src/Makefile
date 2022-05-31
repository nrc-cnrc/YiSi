# @file Makefile
# @brief Makefile for building YiSi
#
# @author Jackie Lo and Darlene Stewart
#
# Multilingual Text Processing / Traitement multilingue de textes
# Digital Technologies Research Centre / Centre de recherche en technologies numériques
# National Research Council Canada / Conseil national de recherches Canada
# Copyright 2018, Her Majesty in Right of Canada /
# Copyright 2018, Sa Majeste la Reine du Chef du Canada

# Override the value of MATEPLUS_HOME with a command line definition, or
# consider defining MATEPLUS_HOME in your .profile, for example:
#   export MATEPLUS_HOME=~/u/sandboxes/mateplus

MATEPLUS_HOME ?=
MATETOOLS_HOME ?= $(MATEPLUS_HOME)

JAVA_HOME ?= /space/group/nrc_ict/pkgs/centos6/gcc-4.9.3/jdk1.8.0_131

MATEPLUS_PATH ?= $(MATEPLUS_HOME)/mateplus.jar
MATETOOLS_SRL_PATH ?= $(MATETOOLS_HOME)/srl.jar

ifneq ("$(wildcard $(MATEPLUS_PATH))$(wildcard $(MATETOOLS_SRL_PATH))","")
   WITH_SRLMATE ?= True
else
   $(info *** mateplus.jar and srl.jar not found)
endif

ifneq (clean, $(MAKECMDGOALS))
ifneq (cleaner, $(MAKECMDGOALS))
ifdef WITH_SRLMATE
   $(info Building with SRLMATE...)
else
   $(info Building without SRLMATE...)
endif
endif
endif

CXXFLAGS += -Wall -pedantic -std=c++11 -g -O3 -Icmdlp/build/include
JFLAGS += -cp ${MATEPLUS_PATH}

CMDLP_LIB = cmdlp/build/lib/libcmdlp.a
LDFLAGS += -Lcmdlp/build/lib
LIBRARIES += -Wl,-Bstatic -lcmdlp -Wl,-Bdynamic

# Flags for embedding Python code
PYTHON_CONFIG ?= python3-config --embed
EMBED_EXIST := $(shell ${PYTHON_CONFIG} > /dev/null ; echo $$?)
ifneq ($(EMBED_EXIST),0)
   PYTHON_CONFIG = python3-config
endif
CXXFLAGS += $(shell ${PYTHON_CONFIG} --cflags)
LDFLAGS += $(filter-out $(shell ${PYTHON_CONFIG} --libs), $(shell ${PYTHON_CONFIG} --ldflags))
LIBRARIES += $(shell ${PYTHON_CONFIG} --libs)

PROG_NAMES := yisi
TEST_NAMES := \
   bert_test \
   biw2v_test \
   call_bert_test \
   contextual_test \
   emap_test \
   lexsim_test \
   lexweight_test \
   maxmatching_test \
   ngram_test \
   oov_test \
   overlapvocab_test \
   phrasesim_test \
   read_sent_test \
   srl_test \
   srlgraph_test \
   srlutil_test \
   testbin \
   util_test \
   w2v_test \
   yisiscorer_test
CMDLP_TEST_NAMES := cmdlp_test

ifdef WITH_SRLMATE
   TEST_NAMES += srlmate_test
endif

# List of binaries that need to be built
BIN_NAMES := $(PROG_NAMES) $(TEST_NAMES)

# List of all possible binaries (programs), including those that won't be built.
ALL_BIN_NAMES := $(BIN_NAMES) srlmate_test

SRC_OBJS := $(patsubst %.cpp,../obj/%.o,$(wildcard *.cpp))

# We compile/link SRLMATE objects a bit differently.
SRLMATE_OBJS := $(addprefix ../obj/,srlmate.o)
SRLMATE_BIN_OBJS := $(addprefix ../obj/,srlmate_test.o)
SRLMATE_BINS := $(addprefix ../bin/,srlmate_test)


ifdef WITH_SRLMATE
   SRLMATE_OBJS += $(addprefix ../obj/,srl.o yisiscorer.o)
   SRLMATE_BINS += $(addprefix ../bin/,srl_test yisiscorer_test yisi)
endif

# Object files are c++ sources that do not result in stand alone binaries
ALL_OBJECTS := $(filter-out $(addprefix ../obj/,$(ALL_BIN_NAMES:%=%.o)),$(SRC_OBJS))

OBJECTS := $(filter-out $(SRLMATE_OBJS),$(ALL_OBJECTS))

#
# Targets
#

# Clear default suffix rules
.SUFFIXES:
# Keep dependencies between calls
.PRECIOUS: ../dep/%.d ../obj/%.o

.PHONY: all binaries scripts
all: binaries bash_scripts python_scripts
ifdef WITH_SRLMATE
all: ../obj/srlmate.jar
all: en.mplsconfig de.mplsconfig es.mplsconfig zh.mplsconfig
endif

binaries: $(BIN_NAMES:%=../bin/%)

YISIBIN_SUB := "s~^YISIBIN=/path/to/your/yisi/bin$$~YISIBIN=$(dir $(CURDIR))bin~"

.PHONY: bash_scripts
bash_scripts: | ../bin
	cp -p ../src/scripts/resolve_yisicmd.sh ../bin
	sed -e $(YISIBIN_SUB) scripts/run_yisi.sh > ../bin/run_yisi.sh
	chmod a+x ../bin/*.sh

.PHONY: python_scripts
python_scripts: | ../lib/python
	cp -p ../src/*.py ../lib/python
	chmod a+x ../lib/python/*.py

../obj/srlmate.jar: *.java
	mkdir -pv ../obj/java
	${JAVA_HOME}/bin/javac $(JFLAGS) -d ../obj/java $^
	cd ../obj/java && ${JAVA_HOME}/bin/jar -cvf ../srlmate.jar *

en.mplsconfig de.mplsconfig es.mplsconfig zh.mplsconfig: %: %.template
	sed -e "s#<YISI_HOME>#$(dir $(CURDIR))#g; s#//#/#g;" \
	    -e "s#<MATEPLUS_HOME>#$(MATEPLUS_HOME)#g;" \
	    -e "s#<MATETOOLS_HOME>#$(MATETOOLS_HOME)#g;" \
	    < $< > $@

$(SRLMATE_BINS): LDFLAGS += -L${JAVA_HOME}/jre/lib/amd64/server
$(SRLMATE_BINS): LIBRARIES += -ljvm
$(SRLMATE_OBJS) $(SRLMATE_BIN_OBJS): CXXFLAGS += -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -DWITH_SRLMATE

$(SRLMATE_BINS): $(SRLMATE_OBJS)
$(SRLMATE_BINS): OBJECTS += $(SRLMATE_OBJS)

../bin/%: ../obj/%.o $(OBJECTS) $(CMDLP_LIB) | ../bin
	$(CXX) $(LDFLAGS) $< $(OBJECTS) $(LIBRARIES) -o $@

$(SRC_OBJS): ../obj/%.o: %.cpp | ../obj ../dep

$(CMDLP_LIB):
	$(MAKE) -C cmdlp

ifdef WITH_SRLMATE
../obj/srl.o: ../obj/.STAMP.WITH_SRLMATE
else
../obj/srl.o: ../obj/.STAMP.WITHOUT_SRLMATE
endif

../obj/%.o:
	$(CXX) $(CXXFLAGS) -MM -MT '$@' $< > $(@:../obj/%.o=../dep/%.d)
	$(CXX) $(CXXFLAGS) -c $< -o $@

../obj/.STAMP.WITH_SRLMATE ../obj/.STAMP.WITHOUT_SRLMATE: | ../obj
	rm -rf ../obj/.STAMP.WITH*_SRLMATE
	touch $@

.PHONY: test
test:
	$(MAKE) -C ../test MATEPLUS_PATH=$(MATEPLUS_PATH)

../dep ../obj ../bin ../lib/python:
	mkdir -p $@

.PHONY: clean cleaner clean.mplsconfig clean.cmdlp cleaner.cmdlp clean.test
clean: clean.mplsconfig clean.cmdlp
	$(RM) -r ../bin ../lib
	$(RM) *~

clean.mplsconfig:
	$(RM) -f en.mplsconfig de.mplsconfig es.mplsconfig zh.mplsconfig

clean.cmdlp:
	$(MAKE) -C cmdlp clean

cleaner.cmdlp:
	$(MAKE) -C cmdlp cleaner

clean.test:
	$(MAKE) -C ../test clean

cleaner: clean cleaner.cmdlp clean.test
	$(RM) -r ../dep ../obj

-include $(wildcard ../dep/*.d)
