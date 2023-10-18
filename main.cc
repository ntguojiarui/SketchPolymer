#include <bits/stdc++.h>
#include "Benchmark.h"
using namespace std;

int main(int argc, char** argv) {
	string path = "/share/datasets/CAIDA2016/CAIDA.dat";
	CAIDABenchmark benchmark(path);
	benchmark.Run(atoi(argv[1]));
	return 0;
}