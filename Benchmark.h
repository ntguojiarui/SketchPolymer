#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

#include <bits/stdc++.h>
#include <hash.h>
#include <Mmap.h>
#include "CorrectDetector.h"
#include "SketchPolymer.h"


struct CAIDA_Tuple {
    uint64_t timestamp;
    uint64_t id;
};

class CAIDABenchmark {
public:
	CAIDABenchmark(std::string PATH) {
        load_result = Load(PATH.c_str());
        dataset = (CAIDA_Tuple*)load_result.start;
        length = load_result.length / sizeof(CAIDA_Tuple);
    }
	~CAIDABenchmark() {}
    void Run(int memory) {
        uint32_t running_length = 10000000;
        std::map<uint64_t, uint32_t> id_map;
        for (int i = 0; i < running_length; ++i) {
            if (id_map.find(dataset[i].id) == id_map.end()) {
                id_map[dataset[i].id] = 0;
            }
            id_map[dataset[i].id]++;
        }
        SketchPolymer<uint64_t, uint64_t>* sketchpolymer = new SketchPolymer<uint64_t, uint64_t>(memory, 3, 5, 5, 3, 50, 1.5);
        CorrectDetector<uint64_t>* correct_detector = new CorrectDetector<uint64_t>();
        for (int i = 0; i < running_length; ++i) {
            sketchpolymer->insert(dataset[i].id, dataset[i + 1].timestamp - dataset[i].timestamp);
            correct_detector->insert(dataset[i].id, dataset[i + 1].timestamp - dataset[i].timestamp);
        }
        uint32_t num = 0;
        double are = 0.0, ale = 0.0, aqe = 0.0;
        double query_quantile = 0.95;
        for (auto i : id_map) {
            if (i.second < 1000)
                continue;
            num++;
            int64_t predict = sketchpolymer->query(i.first, query_quantile);
            int64_t truth = correct_detector->query_value(i.first, query_quantile);
            double truth_quantile = correct_detector->query_quantile(i.first, predict);
            are += 1.0 * abs(predict - truth) / truth;
            ale += 1.0 * abs(log2(predict) - log2(truth));
            aqe += 1.0 * fabs(truth_quantile - query_quantile);
        }
        std::cout << "Estimate: " << num << "\n";
        std::cout << "ARE: " << are / num << "\n";
        std::cout << "ALE: " << ale / num << "\n";
        std::cout << "AQE: " << aqe / num << "\n";
    }
private:
	std::string filename;
    LoadResult load_result;
    CAIDA_Tuple *dataset;
    uint64_t length;
};

#endif
