# Required packages:
# Ubuntu 20.04: clang-12
CC=clang
CXX=clang++
OPT=opt
LLVM_CONFIG=llvm-config
LLC=llc

FILE=tests/test1.ll


CFLAGS=-O2 -Werror -Wall -pedantic -fno-inline-functions -fPIC
LDLIBS=-lm

PASS_NAME=statepoint_counter

$(PASS_NAME).so: $(PASS_NAME).cpp
	$(CXX) $(CFLAGS) `$(LLVM_CONFIG) --cxxflags` -shared -fPIC -o $@ $<

run: $(PASS_NAME).so $(FILE)
	$(OPT) -enable-new-pm=0 -load ./$(PASS_NAME).so -S -$(PASS_NAME) < $(FILE) > /dev/null

test: tests/test1.ll tests/test2.ll $(PASS_NAME).so
	echo "TEST 1"
	$(OPT) -enable-new-pm=0 -load ./$(PASS_NAME).so -S -$(PASS_NAME) < tests/test1.ll > /dev/null
	echo "TEST 2"
	$(OPT) -enable-new-pm=0 -load ./$(PASS_NAME).so -S -$(PASS_NAME) < tests/test2.ll > /dev/null
	echo "TEST 3"
	$(OPT) -enable-new-pm=0 -load ./$(PASS_NAME).so -S -$(PASS_NAME) < tests/test3.ll > /dev/null


.PHONY: clean
clean:
	rm -f $(BENCH) \
	      $(PASS_NAME).so \
	      $(BENCH).orig.ll $(BENCH).ll $(BENCH).s
