#include "code.hpp"
#include <iostream>

// iterate through all (x,y) for x != y, ignore the complement pairs 
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    for (int i = 0; i <= 10; i++) {
        for (int j = i + 1; j <= 10; j++) {

            // skip complements: complement of (i,j) is (10-j, 10-i)
            int ci = 10 - j;
            int cj = 10 - i;

            if (ci < i || (ci == i && cj < j))
                continue;

            int solutions = 0;
            computeSolutions(i, j, solutions);

            if (solutions > 0) {
                cout << "(x,y)=(" << i << "," << j << ") -> solutions: " << solutions << "\n";
            }
        }
    }

    return 0;
}
