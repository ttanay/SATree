.PHONY: all benchmark test clean

CXX=g++

all: benchmark test

benchmark:
	$(CXX) benchmark.cpp SATree.cpp -lbenchmark -o benchmark

test:
	$(CXX) SATree_test.cpp SATree.cpp -o test

clean:
	rm -f benchmark test