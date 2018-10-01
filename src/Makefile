# ========================================================================
# Makefile
#
# Authors: Jackie Lo
#
# Acknowledgement:
#
#   The skeleton of this makefile was cloned from
#
#   https://github.com/masaers/cmdlp
#
#   Thanks Markus!
#   Consider cloning the original repository if you like cmdlp.
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

CXXFLAGS+=-Wall -pedantic -std=c++11 -g -O3 -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux
LDFLAGS+=-L${JAVA_HOME}/jre/lib/amd64/server
LIBRARIES+=-ljvm
JFLAGS+=-cp ${MATEPLUS_HOME}/mateplus.jar

PROG_NAMES=
TEST_NAMES=cmdlp_test srlgraph_test maxmatching_test lexsim_test w2v_test biw2v_test lexweight_test lw_count phrasesim_test srlmate_test srl_test srlutil_test yisiscorer_test testbin yisi

#
# Derived settings
#

# List of binaries that needs to be built
BIN_NAMES=$(PROG_NAMES) $(TEST_NAMES)

SRC_OBJS=$(patsubst %.cpp,../obj/%.o,$(wildcard *.cpp))
CMDLP_OBJS=$(patsubst cmdlp/%.cpp,../obj/%.o,$(wildcard cmdlp/*.cpp))

# Object files are c++ sources that do not result in stand alone binaries
OBJECTS=$(filter-out $(addprefix ../obj/,$(BIN_NAMES:%=%.o)),$(SRC_OBJS) $(CMDLP_OBJS))

#
# Targets
#

# Where are we?
p=`pwd`

# Clear default suffix rules
.SUFFIXES :
# Keep STAMPs and dependencies between calls
.PRECIOUS : %/.STAMP ../dep/%.d ../obj/%.o

all : binaries scripts ../obj/srlmate.jar

binaries : $(BIN_NAMES:%=../bin/%)

scripts : ../bin/.STAMP
	@ln -sf ../src/scripts/pp* ../bin/.
	@ln -sf ../src/scripts/resolve_yisicmd.sh ../bin/.
	@echo YISIBIN=$(p)/../bin > ../bin/run_pp+yisi.sh && cat scripts/run_pp+yisi.sh >> ../bin/run_pp+yisi.sh
	@echo YISIBIN=$(p)/../bin > ../bin/run_yisi.sh && cat scripts/run_yisi.sh >> ../bin/run_yisi.sh
	@chmod a+x ../bin/*.sh

../obj/srlmate.jar : *.java
	@mkdir -pv ../obj/java
	/space/group/nrc_ict/pkgs/centos6/gcc-4.9.3/jdk1.8.0_131/bin/javac $(JFLAGS) -d ../obj/java $^
	cd ../obj/java && jar -cvf ../srlmate.jar *

test : $(TEST_NAMES:%=../test/%.out)
	@if [ -s ../test/.ERROR ]; then \
	     ( cat ../test/.ERROR; rm ../test/.ERROR ) \
	else echo "\n[ALL TESTS PASSED]\n"; \
	fi

../bin/% : ../obj/%.o $(OBJECTS) ../bin/.STAMP
	$(CXX) $(LDFLAGS) $< $(OBJECTS) $(LIBRARIES) -o $@ 

$(SRC_OBJS): ../obj/%.o : %.cpp ../obj/.STAMP ../dep/.STAMP

$(CMDLP_OBJS): ../obj/%.o : cmdlp/%.cpp ../obj/.STAMP ../dep/.STAMP

../obj/%.o :
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -MM -MT '$@' $< > $(@:../obj/%.o=../dep/%.d)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c $< -o $@ -ljvm

../test/%.out : ../bin/% ../test/.STAMP
	@if [ -e $@ ]; then \
	( cp $@ $@.old; \
          $< &> $@; \
	  diff $@ $@.old >> ../test/.ERROR \
	  || echo "REGRESSION TEST FAILED: $<" >> ../test/.ERROR \
	  ; \
	  rm $@.old ) \
	else \
	( $< &> $@; \
          echo "WARNING: No regression test: $<" >> ../test/.ERROR ) \
	fi

%/.STAMP :
	@mkdir -pv $(@D)
	@touch $@

clean :
	@rm -rf ../bin
	@rm -f *~

cleaner : clean
	@rm -rf ../dep ../obj

-include $(wildcard ../dep/*.d)
