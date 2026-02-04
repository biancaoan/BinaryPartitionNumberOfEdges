#include <bits/stdc++.h>
using namespace std;

static constexpr int N = 8;
static constexpr int M = 28;
static constexpr int K = 5;

static int A[10], B[10];
static bool bA[11], bB[11], AB[11]; //vectori de frecventa dar bool


struct subgraphState{
    int edges = 0;
    int undecided = 0;
};


void createBoolPartitions(const vector<int>& pA, const vector<int>& pB){
    memset(bA, 0, sizeof(bA));
    memset(bB, 0, sizeof(bB));
    memset(AB,0, sizeof(AB));
    for (int x : pA) 
        bA[x] = true;
    for (int x : pB) 
        bB[x] = true;
    for (int i = 0; i <= 10; ++i) 
        AB[i] = bA[i] || bB[i];
}

u_int64_t edgeToIndex(pair<int,int> e){
    int u = e.first;
    int v = e.second;
    return (u*(2*N-u-1)/2)+(v-u-1);
}

pair<int,int> indexToEdge(uint64_t idx){
    for(int u = 0; u < N; u++){
        uint64_t l = N - u - 1;
        if(idx < l){
            int v = u + 1 + idx;
            return {u,v};
        }
        idx -= l;
    }
    return {-1,-1};
}

bool checkEdge(uint64_t &G, uint64_t idx){
    return ((G & (1ULL<<idx))!=0);
}

uint64_t addEdge(uint64_t &G, uint64_t idx){
    if(!checkEdge(G,idx)) 
        return (G + (1ULL<< idx));
    return G;
}

uint64_t removeEdge(uint64_t &G, uint64_t idx){
    if(checkEdge(G,idx)) 
        return (G - (1ULL<< idx));
    return G;
}

vector<pair<int,int>> computeEdges() {
    vector<pair<int,int>> edges;
    edges.reserve(N * (N - 1) / 2);   // fancy optimization, allocate space ahead of time
    for (int u = 0; u < N; ++u) {
        for (int v = u + 1; v < N; ++v) {
            edges.push_back(pair{u, v});
        }
    }
    return edges;
}

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

static vector<subgraphState> initialiseState(int S){
    return vector<subgraphState>(S, {0, K*(K-1)/2});
}

static vector<int> computeSubgraphsForEdge(const pair<int,int> &e, const vector<int> &s){
    vector<int> resultSubgraphs;
    int S = s.size(); 
    int uvMask = (1 << e.first) | (1 << e.second);
    for (int si = 0; si < S; ++si) {
        if ((s[si] & uvMask) == uvMask) {
            resultSubgraphs.push_back(si);
        }
    }
    return resultSubgraphs;
}

static vector<vector<int>> computeMatrixEdgeSubgraphs(const vector<pair<int,int>> &edges, const vector<int> &subgraphs){
    vector<vector<int>> res(M);

    for (int e = 0; e < M; ++e) {
        res[e] = computeSubgraphsForEdge(edges[e], subgraphs);
    }

    return res;
}

static bool checkPartition(const subgraphState& s, const bool partition[11]) {
    int l = s.edges;
    int h = s.edges + s.undecided;
    l = max(l, 0);
    h = min(h, 10);
    for (int i = l; i <= h; i++)
        if (partition[i]) 
            return true;
    return false;
}


bool initialiseGraph(const vector<pair<int,int>>& f, vector<subgraphState>& subgraphs, const vector<vector<int>>& matrixSubgraphs,  uint64_t& mask, vector<uint8_t>& fixedEdges){
    fixedEdges.assign(M,0);
    mask = 0;

    for(auto [u,v] : f){
        int idx = edgeToIndex({u,v});
        if(idx < 0)
            cerr << "Problem in initialiseGraph - wrong indexing";
        
        if(fixedEdges[idx]) continue;
        fixedEdges[idx] = 1;
        addEdge(mask,idx);

        for(int i : matrixSubgraphs[idx]){
            subgraphs[i].undecided -= 1;
            subgraphs[i].edges += 1;
            if (!checkPartition(subgraphs[i], AB)) 
                return false;
            if (subgraphs[i].undecided == 0 && !AB[subgraphs[i].edges]) 
                return false;
        }
    }
    return true;
}


static void findEquivalentGraphs(){

}

int main(){
    int a = edgeToIndex({1,2});
    cout<<a<<" ";
    pair<int,int> b = indexToEdge(a);
    cout<<b.first<<" "<<b.second;
    cout<<endl;

    createBoolPartitions({0,2,3,5,6,7,8,9}, {1,4,10});

    auto subgraphMasks = computeSubgraphs();
    int S = subgraphMasks.size();
    auto states = initialiseState(S);

    cout << "Number of 5-vertex induced subgraphs: " << S << " \n";

    auto edges = computeEdges();
    auto matrixSubgraphs = computeMatrixEdgeSubgraphs(edges, subgraphMasks);

    vector<pair<int,int>> forced = {
        {0,1},{0,2},{0,3},{0,4},
        {1,2},{1,3},{1,4},
        {2,3},{2,4},
        {3,4}
    };

    uint64_t Gmask = 0;
    vector<uint8_t> fixedEdges;

    bool ok = initialiseGraph(forced, states, matrixSubgraphs, Gmask, fixedEdges);
    if (!ok) {
        cout << "initialiseGraph failed.\n";
        return 0;
    }

    cout << "Forced edges present check:\n";
    int presentCount = 0;
    for (auto [u,v] : forced) {
        int idx = (int)edgeToIndex({u,v});
        bool present = (Gmask & (1ULL << idx)) != 0;
        cout << "(" << u << "," << v << ") idx=" << idx
             << " present=" << present << "\n";
        if (present) presentCount++;
    }
    cout << "Confirmed present: " << presentCount << " / " << forced.size() << "\n\n";

    cout << "All edges in Gmask:\n";
    for (int i = 0; i < M; ++i) {
        if (Gmask & (1ULL << i)) {
            auto e = indexToEdge((uint64_t)i);
            cout << "(" << e.first << "," << e.second << ") ";
        }
    }
    cout << "\n\n";

    int targetMask = (1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4);
    int targetIndex = -1;
    for (int si = 0; si < S; ++si) {
        if (subgraphMasks[si] == targetMask) { targetIndex = si; break; }
    }
    if (targetIndex != -1) {
        cout << "State for induced subgraph on {0,1,2,3,4}:\n";
        cout << "edges=" << states[targetIndex].edges
             << " undecided=" << states[targetIndex].undecided << "\n";
        cout << "(Expected edges=10, undecided=0 for K5 forced)\n";
    } else {
        cout << "Did not find target 5-set mask for {0,1,2,3,4} (unexpected).\n";
    }

    return 0;
}
