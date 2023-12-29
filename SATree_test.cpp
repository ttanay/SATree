#include <cmath>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "Reader.h"
#include "SATree.h"

enum TestCaseStatus
{
    PASS,
    FAIL
};
struct TestCaseResult
{
    std::string name;
    TestCaseStatus status;
    std::string err;

    void display()
    {
        if (status == TestCaseStatus::PASS)
            std::cout << "TestCase \"" << name << "\": PASSED" << std::endl;
        else
            std::cout << "TestCase \"" << name << "\": FAILED with " << err << std::endl;
    }
};

TestCaseResult test_case3(const std::string & name)
{
    Points input{{0, {-3, -3}}, {1, {-3, -4}}, {2, {-5, -3}}, {3, {-4, -4}}, {4, {-1, -1}}};
    SATree tree(input);
    auto res = tree.range_search({-1, {-6, -2}}, 2.0);
    Point expected{2, {-5, -3}};
    if (res && res.value() == expected)
    {
        return {name, TestCaseStatus::PASS, ""};
    }
    else if (!res)
        return {name, TestCaseStatus::FAIL, "returned null"};
    else
    {
        std::string err = "Expected: " + expected.to_string() + "; Got: " + res->to_string();
        return {name, TestCaseStatus::FAIL, err};
    }
}

TestCaseResult test_case4(const std::string & name)
{
    Points input{{0, {-3, -3}}, {1, {-3, -4}}, {2, {-5, -3}}, {3, {-4, -4}}, {4, {-1, -1}}};
    SATree tree(input);
    auto res = tree.range_search({-1, {-6, -2}}, 1.0);
    Point expected{2, {-5, -3}};
    if (!res)
    {
        return {name, TestCaseStatus::PASS, ""};
    }
    else
    {
        std::string err = "Expected: " + expected.to_string() + "; Got: " + res->to_string();
        return {name, TestCaseStatus::FAIL, err};
    }
}

TestCaseResult test_case5(const std::string & name)
{
    Points input{{0, {1, 1}},   {1, {3, 3}},   {2, {5, 3}},   {3, {3, 4}},   {4, {6, 4}},
                 {5, {-3, -3}}, {6, {-3, -4}}, {7, {-5, -3}}, {8, {-4, -4}}, {9, {-1, -1}}};
    SATree tree(input);
    auto res = tree.nearest_neighbour_search({-1, {-4, -5}}, 2);

    kNNResult expected{{{6, {-3, -4}}, (float)std::sqrt(2.0)}, {{8, {-4, -4}}, 1.0}};

    if (res.size() != 2)
    {
        std::string err = "Expected size: 2; Got: " + std::to_string(res.size());
        return {name, TestCaseStatus::FAIL, err};
    }

    if (res != expected)
    {
        std::string err = "Expected: [";
        for (auto i : expected)
            err += i.to_string();
        err += "]; Got: [";
        for (auto i : res)
            err += i.to_string();
        err += "]";
        return {name, TestCaseStatus::FAIL, err};
    }

    return {name, TestCaseStatus::PASS, ""};
}

TestCaseResult test_case6(const std::string & name)
{
    Points input{{0, {1, 1}},   {1, {3, 3}},   {2, {5, 3}},   {3, {3, 4}},   {4, {6, 4}},
                 {5, {-3, -3}}, {6, {-3, -4}}, {7, {-5, -3}}, {8, {-4, -4}}, {9, {-1, -1}}};
    SATree tree(input);
    if (!tree.nearest_neighbour_search({-1, {-4, -5}}, 0).empty())
    {
        std::string err = "Expected empty result";
        return {name, TestCaseStatus::FAIL, err};
    }
    return {name, TestCaseStatus::PASS, ""};
}

TestCaseResult test_case7(const std::string & name)
{
    Points input{{0, {1, 1, 1}}, {1, {3, 3, 3}}, {2, {5, 3, 2}}, {3, {3, 4, 1}}, {4, {6, 4, 2}}};
    SATree tree(input);
    auto res = tree.nearest_neighbour_search({-1, {1, 1, 0}}, 1);

    kNNResult expected{{{-1, {1, 1, 1}}, 1.0}};

    if (res.size() != 1)
        return {name, TestCaseStatus::FAIL, "Result does not have 1 element"};

    if (res != expected)
    {
        std::string err = "Expected: [";
        for (auto i : expected)
            err += i.to_string();
        err += "]; Got: [";
        for (auto i : res)
            err += i.to_string();
        err += "]";
        return {name, TestCaseStatus::FAIL, err};
    }

    return {name, TestCaseStatus::PASS, ""};
}


TestCaseResult e2e_test_case(const std::string & name, SATree & tree, Point query, std::set<int> gt)
{
    auto res = tree.nearest_neighbour_search(query, 100);
    if (res.empty())
    {
        std::string err = "Query result empty";
        return {name, TestCaseStatus::FAIL, err};
    }
    else if (res.size() != 100)
    {
        std::string err = "Query result size expected 100 got" + std::to_string(res.size());
        return {name, TestCaseStatus::FAIL, err};
    }
    for (auto r : res)
    {
        if (gt.find(r.p.node_id) == gt.end())
        {
            std::string err = "Point " + r.p.to_string() + " is not a ground truth";
            return {name, TestCaseStatus::FAIL, err};
        }
    }
    return {name, TestCaseStatus::PASS, ""};
}

void e2e_driver(const std::string & name)
{
    // Read base vectors
    char bvecs_filename[] = "dataset/siftsmall/siftsmall_base.fvecs";
    FVecsReader reader{bvecs_filename};
    Points base_vecs;
    int counter{0};
    while (!reader.eof())
    {
        float * v = reader.readvec();
        std::vector<float> vf(v, v + reader.dimension());
        base_vecs.push_back(Point{counter, vf});
        counter++;
    }
    reader.close();

    // Build SATree
    SATree tree(base_vecs);

    char qvecs_filename[] = "dataset/siftsmall/siftsmall_query.fvecs";
    reader.open(qvecs_filename);
    Points qvecs;
    while (!reader.eof())
    {
        float * v = reader.readvec();
        std::vector<float> vf(v, v + reader.dimension());
        qvecs.push_back(Point{-1, vf});
    }

    char ivecs_filename[] = "dataset/siftsmall/siftsmall_groundtruth.ivecs";
    IVecsReader ivreader{ivecs_filename};
    std::vector<std::set<int>> gts;
    while (!ivreader.eof())
    {
        int * v = ivreader.readvec();
        std::set<int> vf(v, v + ivreader.dimension());
        gts.push_back(vf);
    }

    // For each query vector,
    for (int i = 0; i < qvecs.size(); i++)
    {
        auto t_name = name + " {" + std::to_string(i) + "}";
        e2e_test_case(t_name, tree, qvecs[i], gts[i]).display();
    }
}

int main(int argc, char ** argv)
{
    test_case3("Range search").display();
    test_case4("Range search null").display();
    test_case5("kNN Search").display();
    test_case6("kNN Search empty").display();
    test_case7("kNN 3-dimensions").display();
    e2e_driver("TexMex E2E");
    return 0;
}