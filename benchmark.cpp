#include "Reader.h"
#include "SATree.h"
#include <iostream>

#include <benchmark/benchmark.h>

static void BM_SATreeBuild(benchmark::State &state)
{
    char filename[] = "dataset/siftsmall/siftsmall_base.fvecs";
    FVecsReader reader{filename};
    Points input;
    while(!reader.eof())
    {
        float *v = reader.readvec();
        std::vector<float> vf(v, v + reader.dimension());
        input.push_back(Point{vf});
    }

    for(auto _: state)
        SATree tree(input);
}

BENCHMARK(BM_SATreeBuild);
// Run the benchmark
BENCHMARK_MAIN();
