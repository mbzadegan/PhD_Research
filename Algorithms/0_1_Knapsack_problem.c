// 0/1 Knapsack Problem (Dynamic Programming)
// From CLRS, Chapter 15.3
// Author: Mohammad

#include <stdio.h>

// Function to get the maximum of two integers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Solves the 0/1 Knapsack problem using bottom-up DP
int knapsack(int W, int wt[], int val[], int n) {
    int K[n + 1][W + 1];

    // Build table K[][] in bottom-up manner
    for (int i = 0; i <= n; i++) {
        for (int w = 0; w <= W; w++) {
            if (i == 0 || w == 0)
                K[i][w] = 0;
            else if (wt[i - 1] <= w)
                K[i][w] = max(val[i - 1] + K[i - 1][w - wt[i - 1]], K[i - 1][w]);
            else
                K[i][w] = K[i - 1][w];
        }
    }

    return K[n][W];
}

int main() {
    // Example input from CLRS
    int val[] = {60, 100, 120};
    int wt[] = {10, 20, 30};
    int W = 50; // knapsack capacity
    int n = sizeof(val) / sizeof(val[0]);

    int maxValue = knapsack(W, wt, val, n);
    printf("Maximum value in Knapsack = %d\n", maxValue);
    return 0;
}
