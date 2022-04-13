# Required packages:
# Ubuntu 20.04: clang-12
CC=clang
CXX=clang++
OPT=opt
LLVM_CONFIG=llvm-config
LLC=llc

CFLAGS=-O2 -Werror -Wall -pedantic -fno-inline-functions -fPIC
LDLIBS=-lm

BENCH=binary_trees
PASS_NAME=statepoint_checker

$(PASS_NAME).so: $(PASS_NAME).cpp
	$(CXX) $(CFLAGS) `$(LLVM_CONFIG) --cxxflags` -shared -fPIC -o $@ $<

$(BENCH): $(BENCH).c $(PASS_NAME).so
	$(CC) $(CFLAGS) -S -emit-llvm -o $(BENCH).orig.ll $<
	$(OPT) -enable-new-pm=0 -load ./$(PASS_NAME).so -S -$(PASS_NAME) < $(BENCH).orig.ll > $(BENCH).ll
	$(LLC) -O2 --relocation-model=pic -o $(BENCH).s $(BENCH).ll
	$(CC) $(LDLIBS) $(BENCH).s -o $@

test: tests/test1.ll tests/test2.ll $(PASS_NAME).so
	echo "TEST 1"
	$(OPT) -enable-new-pm=0 -load ./$(PASS_NAME).so -m 3 -S -$(PASS_NAME) < tests/test1.ll > /dev/null
	echo "TEST 2"
	$(OPT) -enable-new-pm=0 -load ./$(PASS_NAME).so -m 3 -S -$(PASS_NAME) < tests/test2.ll > /dev/null


.PHONY: clean
clean:
	rm -f $(BENCH) \
	      $(PASS_NAME).so \
	      $(BENCH).orig.ll $(BENCH).ll $(BENCH).s
