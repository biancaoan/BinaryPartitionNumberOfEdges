#include <bits/stdc++.h>
using namespace std;

static constexpr int N = 10;
static constexpr int K = 5;

struct Edge{
    int u,v;
    int8_t state; // undecided: -1, edge: 1, non-edge: 0
};

struct subgraphState{
    int edges = 0;
    int undecided = 0;
};

// logic: based on the decided edges, check if the subgraph can still end up with x or y edges
// parameters: the subgraph, x and y
// return: true if it's doable, flase otherwise

static bool checkBreak(const subgraphState &sG, int x, int y){
    int minim = sG.edges;
    int maxim = sG.undecided + sG.edges;
    return ( (minim <= x && x <= maxim) || (minim <= y && y <= maxim) );
}

// logic: based on the edges, check if the subgraph is completed, if yes check if it has x or y edges
// parameters: the subgraph, x and y
// return: true if it's completed and the number of edges is x or y, flase otherwise

static bool checkSubgraph(const subgraphState& sG, int x, int y) {
    if (sG.undecided != 0) return true;          // not complete yet
    return (sG.edges == x) || (sG.edges == y);   // complete so must match
}

// compute edges
vector<Edge> computeEdges() {
    vector<Edge> edges;
    edges.reserve(N * (N - 1) / 2);   // fancy optimization, allocate space ahead of time
    for (int u = 0; u < N; ++u) {
        for (int v = u + 1; v < N; ++v) {
            edges.push_back(Edge{u, v, int8_t(-1)}); // initialize as undecided
        }
    }
    return edges;
}

// logic: computes all K-vertex subgraphs (as bitmasks)
// returns: a vector of masks where bit i = 1 iff vertex i is included
// exemple: for the subgrapf {1,3,5,7,8} -> bits[] = 0110101010, submask[]={...{426}}
static vector<int> computeSubgraphs() {
    vector<int> subgraphMasks;

    vector<int> bits(N, 0);
    for (int i = 0; i < K; ++i) 
        bits[i] = 1;
    
    sort(bits.begin(), bits.end());

    do {
        int mask = 0;
        for (int i = 0; i < N; ++i) {
            if (bits[i]) 
                mask |= (1 << i);
        }
        subgraphMasks.push_back(mask);
    } while (next_permutation(bits.begin(), bits.end()));

    return subgraphMasks;
}

// function that returns which subsets are "affected" by a certain edge 
// exemple: for N = 10 and K = 5 it returns a vector with 8 choose 3 elememts - the indexes of all subgraphs that contain the edge
// param: the edge, the vector with all subgraphs
static vector<int> computeSubgraphsForEdge(const Edge &e, const vector<int> &s){
    vector<int> resultSubgraphs;
    int S = s.size(); 
    int u = e.u;
    int v = e.v;
    int uvMask = (1 << u) | (1 << v);
    for (int si = 0; si < S; ++si) {
        if ((s[si] & uvMask) == uvMask) {
            resultSubgraphs.push_back(si);
        }
    }
    return resultSubgraphs;
}

//map the result from computeSubgraphsForEdge for every edge 
static vector<vector<int>> computeMatrixEdgeSubgraphs(const vector<Edge> &edges, const vector<int> &subgraphs){
    int M = edges.size();
    vector<vector<int>> res(M);

    for (int e = 0; e < M; ++e) {
        res[e] = computeSubgraphsForEdge(edges[e], subgraphs);
    }

    return res;
}


// return a vector that knows the state of all subgraphs
static vector<subgraphState> initialiseState(int S){
    return vector<subgraphState>(S, {0, 10});
}


static int dfs(int edge, vector<Edge> E, vector<int> S, vector<subgraphState> States, vector<vector<int>> matrixSubgraphs, int M, int x, int y)
{
    if (edge == M) {
        for (auto &sg : States) {
            if (!checkSubgraph(sg, x, y)) return 0;
        }
        
        cout << "Graph for (x,y)=(" << x << "," << y << "): ";

        for (int e = 0; e < M; ++e) {
            if (E[e].state == 1) {
                cout << "(" << E[e].u << "," << E[e].v << ") ";
            }
        }
        cout << "\n";
        cout.flush();   

        return 1; // found a solution
    }
    
    struct backUp { 
        int modifiedStates; 
        int edges_delta; 
        int undecided_delta; 
    };

    int solutions = 0;

    // branch: 0 and 1
    for (int val = 0; val <= 1; ++val) {
        E[edge].state = int8_t(val);

        vector<backUp> undos;
        undos.reserve(matrixSubgraphs[edge].size()); // one backup per subgrapg affected by the edge

        bool ok = true;

        // update every k-subgraph (5-set) that contains this edge
        for (int state : matrixSubgraphs[edge]) {
            // for every subgraph one of its 10 edges got decided
            States[state].undecided -= 1;

            int edges_delta = 0;
            if (val == 1) { 
                States[state].edges += 1;
                edges_delta = 1;
            }

            undos.push_back({state, edges_delta, -1});

            // break if impossible now, or if completed and wrong
            if (!checkBreak(States[state], x, y) || !checkSubgraph(States[state], x, y)) {
                ok = false;
                break;
            }
        }

        if (ok) {
            solutions += dfs(edge + 1, E, S, States, matrixSubgraphs, M, x, y);
        }

        // undo changes so the next val (0/1) starts from the same parent state
        for (auto &u : undos) {
            States[u.modifiedStates].undecided -= u.undecided_delta; 
            States[u.modifiedStates].edges     -= u.edges_delta;
        }
        E[edge].state = int8_t(-1);
    }

    return solutions;
}



static int computeSolutions(int x, int y, int &solutions){
    vector<Edge> edges = computeEdges(); // the graph by indexes
    vector<int> subgraphs = computeSubgraphs(); // all subgraphs written in decimal, for N = 10 and K = 5: size = 252 
    int M = edges.size();      // number of edges in total
    int S = subgraphs.size();  // number of subgraphs in total

    vector<subgraphState> States = initialiseState(S);

    vector<vector<int>> matrixSubgraphs = computeMatrixEdgeSubgraphs(edges, subgraphs);

    solutions = dfs(0, edges, subgraphs, States, matrixSubgraphs, M, x, y);
    return solutions;
}
