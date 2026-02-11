#include <bits/stdc++.h>
using namespace std;

static constexpr int N = 8;
static constexpr int M = 28;
static constexpr int K = 5;
static constexpr int S = 56;
static constexpr int E5 = 10;

static bool A[11]; //vectori de frecventa dar bool


struct subgraphState{
    int edges = 0;
    int undecided = 0;
};


void createBoolPartition(vector<int> a){
    memset(A, 0, sizeof(A));
    for (int i = 0; i < a.size(); i++) 
        A[a[i]] = true;
}

uint64_t edgeToIndex(pair<int,int> e){
    int u = e.first;
    int v = e.second;
    if(u > v) std::swap(u,v);
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
    return (G >> idx) & 1ULL;
}

void addEdge(uint64_t &G, uint64_t idx){
    G |= (1ULL << idx);
}

void removeEdge(uint64_t &G, uint64_t idx){
    G &= ~(1ULL << idx);
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

bool checkPossibleA(const subgraphState& s){
    int l = max(0, s.edges);
    int h = min(10, s.edges + s.undecided);
    for (int i = l; i <= h; i++) 
        if (A[i]) 
            return true;
    return false;
}

bool checkPossibleB(const subgraphState& s){
    int l = max(0, s.edges);
    int h = min(10, s.edges + s.undecided);
    for (int i = l; i <= h; i++) 
        if (!A[i]) 
            return true;
    return false;
}

bool checkA(const subgraphState& s){
    return (s.undecided == 0) && A[s.edges];
}

bool checkB(const subgraphState& s){
    return (s.undecided == 0) && !A[s.edges];
}

vector<int8_t> buildFixedEdges(const vector<pair<int,int>>& forcedEdge, const vector<pair<int,int>>& forcedNonEdge){
    vector<int8_t> res(M, -1);
    for (auto [u,v] : forcedEdge) {
        int idx = edgeToIndex({u,v});
        res[idx] = 1;
    }
    for (auto [u,v] : forcedNonEdge) {
        int idx = edgeToIndex({u,v});
        res[idx] = 0;
    }
    return res;
}

bool partitionsStillCompatible(const subgraphState& g, const subgraphState& h) {
    bool a = checkPossibleA(g) && checkPossibleA(h);
    bool b = checkPossibleB(g) && checkPossibleB(h);
    return a || b;
}

struct Result {
    bool found = false;
    uint64_t Gmask = 0;
    uint64_t Hmask = 0;
};

Result dfs(int pos, const vector<vector<int>>& matrixSubgraphs, const vector<int8_t>& constraintsG, uint64_t& Gmask, 
           uint64_t& Hmask, bool different, vector<subgraphState>& Gstates, vector<subgraphState>& Hstates){
    
    if (pos == M) {
        if (!different)
            return {};
        bool someGA = false;
        bool someGB =false;
        for (int si = 0; si < Gstates.size(); ++si) {
            bool gA = checkA(Gstates[si]);
            bool gB = checkB(Gstates[si]);
            bool hA = checkA(Hstates[si]);
            bool hB = checkB(Hstates[si]);
                if (!((gA && hA) || (gB && hB))) 
                    return {};
                if (gA) 
                    someGA = true;
                if (gB) 
                    someGB = true;
            }
            if (!someGA || !someGB) 
                return {};

            return {true, Gmask, Hmask};
        }

    int constraint = constraintsG[pos];
    int pos1G = 0;
    int pos2G = 1;

    if (constraint == 0){ 
        pos1G = 0;
        pos2G = 0; 
    }
    else if (constraint == 1){ 
        pos1G = 1;
        pos2G = 1; 
    }

    const vector<int>& affected = matrixSubgraphs[pos];

    for (int i = pos1G; i <= pos2G; i++) {
        if (i) 
            addEdge(Gmask, pos); 
        else 
            removeEdge(Gmask, pos);
        for (int si : affected) {
            Gstates[si].undecided -= 1;
            if (i)
                Gstates[si].edges += 1;
        }

        for (int j = 0; j <= 1; j++) {
            bool newDiff = different || (i != j);
            if (j) 
                addEdge(Hmask, pos); 
            else 
                removeEdge(Hmask, pos);
            for (int si : affected) {
                Hstates[si].undecided -= 1;
                if (j)
                    Hstates[si].edges += 1;
            }

            bool ok = true;
            for (int si : affected) {
                if (!partitionsStillCompatible(Gstates[si], Hstates[si])){
                    ok = false; 
                    break; }
            }
            if (ok) {
                auto res = dfs(pos + 1, matrixSubgraphs, constraintsG, Gmask, Hmask, newDiff, Gstates, Hstates);
                if (res.found) 
                    return res;
            }

            for (int si : affected) {
                if (j)
                    Hstates[si].edges -= 1;
                Hstates[si].undecided += 1;
            }
        }

        for (int si : affected) {
            if (i) Gstates[si].edges -= 1;
            Gstates[si].undecided += 1;
        }
    }
    return {};
}

void printMaskEdges(const char* name, uint64_t mask) {
    cout << name << " edges:\n";
    int cnt = 0;
    for (int i = 0; i < M; i++)
        if ((mask >> i) & 1ULL) 
            cnt++;
    cout << "# of edge = " << cnt << "\n";
    for (int i = 0; i < M; ++i) {
        if ((mask >> i) & 1ULL) {
            auto e = indexToEdge(i);
            cout << "(" << e.first << "," << e.second << ") ";
        }
    }
    cout << "\n\n";
}

void findEquivalentGraphs(const vector<vector<int>>& matrixSubgraphs, int S, const vector<int8_t>& constraintsG){
    auto Gstates = initialiseState(S);
    auto Hstates = initialiseState(S);

    uint64_t Gmask = 0, Hmask = 0;
    Result res = dfs(0, matrixSubgraphs, constraintsG, Gmask, Hmask, false, Gstates, Hstates);

    if (!res.found) {
        cout << "No pair (G,H) found.\n";
        return;
    }

    cout << "Found G and H equivalent but not equal!\n";

    printMaskEdges("G", res.Gmask);
    printMaskEdges("H", res.Hmask);
}

void solveForEachPartition(vector<int> a){
    createBoolPartition(a);
    vector<int> subgraphMasks = computeSubgraphs();
    vector<pair<int,int>> edges = computeEdges();
    vector<vector<int>> matrixSubgraphs = computeMatrixEdgeSubgraphs(edges, subgraphMasks);

    vector<pair<int,int>> forcedPresentG = {
        {0,1}
    };

    vector<pair<int,int>> forcedAbsentG = {
        {0,2},{0,3},{0,4},{1,2},{1,3},{1,4},{2,3},{2,4},{3,4}
    };

    auto constrainsG = buildFixedEdges(forcedPresentG, forcedAbsentG);

    findEquivalentGraphs(matrixSubgraphs, S, constrainsG);
    
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<int> a = {0, 6, 9};
    vector<int> c = {2, 3, 5, 7, 8};
    int n = c.size();

    for (int i = 0; i < (1 << n); i++){
        vector<int> ca = a;
        for (int j = 0; j < n; j++){
            if ((i >> j) & 1){
                ca.push_back(c[j]);
            }
        }
        for(auto x : ca)
            cout<<x<<" ";
        cout<<endl;
        solveForEachPartition(ca);
    }
    return 0;
}
