#include "code.hpp"
#include <cassert>
#include <iostream>
using namespace std;

// test computeEdges()
static void test_computeEdges() {
    auto edges = computeEdges();
    
    // manually verify all 45 edges
    vector<pair<int,int>> expected = {
        {0,1},{0,2},{0,3},{0,4},{0,5},{0,6},{0,7},{0,8},{0,9},
        {1,2},{1,3},{1,4},{1,5},{1,6},{1,7},{1,8},{1,9},
        {2,3},{2,4},{2,5},{2,6},{2,7},{2,8},{2,9},
        {3,4},{3,5},{3,6},{3,7},{3,8},{3,9},
        {4,5},{4,6},{4,7},{4,8},{4,9},
        {5,6},{5,7},{5,8},{5,9},
        {6,7},{6,8},{6,9},
        {7,8},{7,9},
        {8,9}
    };
    
    assert((int)expected.size() == 45);
    
    // check edges
    for (int i = 0; i < 45; ++i) {
        assert(edges[i].u == expected[i].first);
        assert(edges[i].v == expected[i].second);
        assert(edges[i].state == -1);  // all should be undecided initially
    }
    
    cout << "computeEdges passed all tests: " << edges.size() << " edges\n";
}


// test computeSubgraphs()
static void test_computeSubgraphs() {
    auto subgraphs = computeSubgraphs();
    
    // for N=10, K=5, total 5-vertex subsets should be 252
    assert(subgraphs.size() == 252);
    
    // check first 10 masks
    vector<int> first_10 = {992, 976, 944, 880, 752, 496, 968, 936, 872, 744};
    for (int i = 0; i < 10; ++i) {
        assert(subgraphs[i] == first_10[i]);
    }

    // each mask should have exactly 5 1s
    for (int mask : subgraphs) {
        int count = 0;
        for (int i = 0; i < N; ++i) {
            if (mask & (1 << i)) count++;
        }
        assert(count == K);
    }
    
    // check that all masks are unique
    set<int> unique_masks(subgraphs.begin(), subgraphs.end());
    assert((int)unique_masks.size() == 252);
    
    cout << "computeSubgraphs passed all tests: " << subgraphs.size() << " subgraphs\n";
}

// test computeSubgraphsForEdge()
static void test_computeSubgraphsForEdge() {
    auto edges = computeEdges();
    auto subgraphs = computeSubgraphs();
    
    // test with edge (0, 1)
    Edge e = {0, 1, int8_t(-1)};
    auto affected = computeSubgraphsForEdge(e, subgraphs);
    
    // edge (0,1) should be contained in 8 choose 3 = 56 subgraphs
    assert(affected.size() == 56);
    
    // verify each returned index is valid and contains the edge
    int uvMask = (1 << e.u) | (1 << e.v);
    for (int si : affected) {
        assert(0 <= si && si < (int)subgraphs.size());
        assert((subgraphs[si] & uvMask) == uvMask);
    }
    
    // test with another edge (3, 7)
    Edge e2 = {3, 7, int8_t(-1)};
    auto affected2 = computeSubgraphsForEdge(e2, subgraphs);
    assert((int)affected2.size() == 56);
    
    int uv2Mask = (1 << e2.u) | (1 << e2.v);
    for (int si : affected2) {
        assert((subgraphs[si] & uv2Mask) == uv2Mask);
    }
    
    cout << "computeSubgraphsForEdge passed all tests: edge contains " << affected.size() << " subgraphs\n";
}

int main(){

    test_computeEdges();
    test_computeSubgraphs();
    test_computeSubgraphsForEdge();
    
    cout << "\n All tests passed!\n";
    return 0;
}