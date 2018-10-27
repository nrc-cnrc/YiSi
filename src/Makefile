# ========================================================================
# Makefile
#
# Authors: Jackie Lo and Darlene Stewart
#
# Acknowledgement:
#
#   The skeleton of this makefile was cloned from
#
#   https://github.com/masaers/cmdlp
#
#   Thanks Markus!
#
# ========================================================================
#
# Settings
#

# Override the value of MATEPLUS_HOME with a command line definition, or
# consider defining MATEPLUS_HOME in your .profile, for example:
#   export MATEPLUS_HOME=~/u/sandboxes/mateplus
MATEPLUS_HOME ?= ~/u/tools/MATE/mateplus-master/src

JAVA_HOME ?= /space/group/nrc_ict/pkgs/centos6/gcc-4.9.3/jdk1.8.0_131

MATEPLUS_PATH ?= $(MATEPLUS_HOME)/mateplus.jar

ifneq ("$(wildcard $(MATEPLUS_PATH))","")
   WITH_SRLMATE ?= True
else
   $(info *** mateplus.jar not found)
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


CXXFLAGS += -Wall -pedantic -std=c++11 -g -O3
JFLAGS += -cp ${MATEPLUS_PATH}

PROG_NAMES :=
TEST_NAMES := cmdlp_test srlgraph_test maxmatching_test lexsim_test w2v_test biw2v_test \
	      lexweight_test lw_count phrasesim_test srl_test srlutil_test util_test \
	      yisiscorer_test yisi testbin

ifdef WITH_SRLMATE
   TEST_NAMES += srlmate_test
endif

# List of binaries that need to be built
BIN_NAMES := $(PROG_NAMES) $(TEST_NAMES)

# List of all possible binaries (programs), including those that won't be built.
ALL_BIN_NAMES := $(BIN_NAMES) srlmate_test

SRC_OBJS := $(patsubst %.cpp,../obj/%.o,$(wildcard *.cpp))
CMDLP_OBJS := $(patsubst cmdlp/%.cpp,../obj/%.o,$(wildcard cmdlp/*.cpp))

# We compile/link SRLMATE objects a bit differently.
SRLMATE_OBJS := $(addprefix ../obj/,srlmate.o)
SRLMATE_BIN_OBJS := $(addprefix ../obj/,srlmate_test.o)
SRLMATE_BINS := $(addprefix ../bin/,srlmate_test)


ifdef WITH_SRLMATE
   SRLMATE_OBJS += $(addprefix ../obj/,srl.o)
   SRLMATE_BINS += $(addprefix ../bin/,srl_test yisiscorer_test yisi)
endif

# Object files are c++ sources that do not result in stand alone binaries
ALL_OBJECTS := $(filter-out $(addprefix ../obj/,$(ALL_BIN_NAMES:%=%.o)),$(SRC_OBJS) $(CMDLP_OBJS))

OBJECTS := $(filter-out $(SRLMATE_OBJS),$(ALL_OBJECTS))

#
# Targets
#

# Clear default suffix rules
.SUFFIXES:
# Keep STAMPs and dependencies between calls
.PRECIOUS: %/.STAMP ../dep/%.d ../obj/%.o

.PHONY: all binaries scripts
all: binaries scripts
ifdef WITH_SRLMATE
all: ../obj/srlmate.jar
all: en.mplsconfig de.mplsconfig
endif

binaries: $(BIN_NAMES:%=../bin/%)

scripts: ../bin/.STAMP
	@ln -sf ../src/scripts/pp* ../bin
	@ln -sf ../src/scripts/resolve_yisicmd.sh ../bin
	@echo YISIBIN=$(dir $(CURDIR))bin > ../bin/run_pp+yisi.sh && cat scripts/run_pp+yisi.sh >> ../bin/run_pp+yisi.sh
	@echo YISIBIN=$(dir $(CURDIR))bin > ../bin/run_yisi.sh && cat scripts/run_yisi.sh >> ../bin/run_yisi.sh
	@chmod a+x ../bin/*.sh

../obj/srlmate.jar: *.java
	@mkdir -pv ../obj/java
	${JAVA_HOME}/bin/javac $(JFLAGS) -d ../obj/java $^
	cd ../obj/java && jar -cvf ../srlmate.jar *

en.mplsconfig de.mplsconfig: %: %.template
	sed -e "s#<YISI_HOME>/#$(dir $(CURDIR))#g; s#<MATEPLUS_HOME>#$(MATEPLUS_HOME)#g;" < $< > $@

$(SRLMATE_BINS): LDFLAGS += -L${JAVA_HOME}/jre/lib/amd64/server
$(SRLMATE_BINS): LIBRARIES += -ljvm
$(SRLMATE_OBJS) $(SRLMATE_BIN_OBJS): CXXFLAGS += -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -DWITH_SRLMATE

$(SRLMATE_BINS): $(SRLMATE_OBJS)
$(SRLMATE_BINS): OBJECTS += $(SRLMATE_OBJS)

../bin/%: ../obj/%.o $(OBJECTS) ../bin/.STAMP
	$(CXX) $(LDFLAGS) $< $(OBJECTS) $(LIBRARIES) -o $@

$(SRC_OBJS): ../obj/%.o: %.cpp ../obj/.STAMP ../dep/.STAMP

$(CMDLP_OBJS): ../obj/%.o: cmdlp/%.cpp ../obj/.STAMP ../dep/.STAMP

ifdef WITH_SRLMATE
../obj/srl.o: ../obj/.STAMP.WITH_SRLMATE
else
../obj/srl.o: ../obj/.STAMP.WITHOUT_SRLMATE
endif

../obj/%.o:
	$(CXX) $(CXXFLAGS) -MM -MT '$@' $< > $(@:../obj/%.o=../dep/%.d)
	$(CXX) $(CXXFLAGS) -c $< -o $@

../obj/.STAMP.WITH_SRLMATE ../obj/.STAMP.WITHOUT_SRLMATE: ../obj/.STAMP
	rm -rf ../obj/.STAMP.WITH*_SRLMATE
	touch $@

.PHONY: test
test: ../test/.STAMP $(TEST_NAMES:%=../test/%.out)
	@if [ -s ../test/.ERROR ]; then \
	     ( cat ../test/.ERROR; rm ../test/.ERROR ) \
	else echo "\n[ALL TESTS PASSED]\n"; \
	fi

../test/%.out: ../bin/% ../test/.STAMP
	@if [ -e $@ ]; then \
	( cp $@ $@.old; \
          echo "$< &> $@"; $< &> $@; \
	  diff $@ $@.old >> ../test/.ERROR \
	  || echo "REGRESSION TEST FAILED: $<" >> ../test/.ERROR \
	  ; \
	  rm $@.old ) \
	else \
	( $< &> $@; \
          echo "WARNING: No regression test: $<" >> ../test/.ERROR ) \
	fi

%/.STAMP:
	@mkdir -pv $(@D)
	@touch $@

.PHONY: clean cleaner clean.mplsconfig
clean: clean.mplsconfig
	@rm -rf ../bin
	@rm -f *~

clean.mplsconfig:
	@rm -f en.mplsconfig de.mplsconfig

cleaner: clean
	@rm -rf ../dep ../obj

-include $(wildcard ../dep/*.d)
