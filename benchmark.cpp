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

static void BM_SATreeRangeSearch(benchmark::State &state)
{
    char bvecs_filename[] = "dataset/siftsmall/siftsmall_base.fvecs";
    FVecsReader reader{bvecs_filename};
    Points input;
    while(!reader.eof())
    {
        float *v = reader.readvec();
        std::vector<float> vf(v, v + reader.dimension());
        input.push_back(Point{vf});
    }

    char query_filename[] = "dataset/siftsmall/siftsmall_query.fvecs";
    FVecsReader qreader{query_filename};
    float *qvec = qreader.readvec();
    std::vector<float> qv(qvec, qvec + qreader.dimension());
    Point query{qv};
    SATree tree(input);

    for(auto _: state)
        tree.range_search(query, 5.0);
}

static void BM_SATreekNNSearch(benchmark::State &state)
{
    char bvecs_filename[] = "dataset/siftsmall/siftsmall_base.fvecs";
    FVecsReader reader{bvecs_filename};
    Points input;
    while(!reader.eof())
    {
        float *v = reader.readvec();
        std::vector<float> vf(v, v + reader.dimension());
        input.push_back(Point{vf});
    }

    char query_filename[] = "dataset/siftsmall/siftsmall_query.fvecs";
    FVecsReader qreader{query_filename};
    float *qvec = qreader.readvec();
    std::vector<float> qv(qvec, qvec + qreader.dimension());
    Point query{qv};
    SATree tree(input);

    for(auto _: state)
        tree.nearest_neighbour_search(query, 100);
}

BENCHMARK(BM_SATreeBuild);
BENCHMARK(BM_SATreeRangeSearch);
BENCHMARK(BM_SATreekNNSearch);
// Run the benchmark
BENCHMARK_MAIN();
