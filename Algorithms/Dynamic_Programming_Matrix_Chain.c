// Matrix Chain Multiplication using Dynamic Programming
// Based on CLRS Chapter 15.2

#include <stdio.h>
#include <limits.h>

// Function to print the optimal parenthesization
void printParenthesis(int i, int j, int n, int s[n][n], char name) {
    if (i == j) {
        printf("%c", name + i - 1);
        return;
    }
    printf("(");
    printParenthesis(i, s[i][j], n, s, name);
    printParenthesis(s[i][j] + 1, j, n, s, name);
    printf(")");
}

// Matrix Chain Order algorithm (CLRS)
void matrixChainOrder(int p[], int n) {
    int m[n][n]; // minimum cost table
    int s[n][n]; // table for k index to reconstruct solution

    // cost of multiplying one matrix = 0
    for (int i = 1; i < n; i++)
        m[i][i] = 0;

    // l = chain length
    for (int l = 2; l < n; l++) {
        for (int i = 1; i < n - l + 1; i++) {
            int j = i + l - 1;
            m[i][j] = INT_MAX;
            for (int k = i; k <= j - 1; k++) {
                int q = m[i][k] + m[k + 1][j] + p[i - 1] * p[k] * p[j];
                if (q < m[i][j]) {
                    m[i][j] = q;
                    s[i][j] = k;
                }
            }
        }
    }

    printf("Minimum number of multiplications: %d\n", m[1][n - 1]);
    printf("Optimal Parenthesization: ");
    printParenthesis(1, n - 1, n, s, 'A');
    printf("\n");
}

int main() {
    // Example from CLRS Figure 15.4
    int p[] = {30, 35, 15, 5, 10, 20, 25};
    int n = sizeof(p) / sizeof(p[0]);

    matrixChainOrder(p, n);
    return 0;
}
