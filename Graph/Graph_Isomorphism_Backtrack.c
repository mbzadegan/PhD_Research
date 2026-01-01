#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAXN 20   // increase if needed (exponential!)

int n;
int G1[MAXN][MAXN], G2[MAXN][MAXN];
int perm[MAXN];
bool used[MAXN];

/* Read graph from file */
void read_graph(const char *filename, int G[MAXN][MAXN]) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("File error");
        exit(1);
    }

    fscanf(f, "%d", &n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            fscanf(f, "%d", &G[i][j]);

    fclose(f);
}

/* Check if current permutation preserves adjacency */
bool check_isomorphism() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (G1[i][j] != G2[perm[i]][perm[j]])
                return false;
        }
    }
    return true;
}

/* Backtracking over all vertex permutations */
bool backtrack(int depth) {
    if (depth == n)
        return check_isomorphism();

    for (int v = 0; v < n; v++) {
        if (!used[v]) {
            perm[depth] = v;
            used[v] = true;

            if (backtrack(depth + 1))
                return true;

            used[v] = false;
        }
    }
    return false;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s graph1.txt graph2.txt\n", argv[0]);
        return 1;
    }

    read_graph(argv[1], G1);
    read_graph(argv[2], G2);

    for (int i = 0; i < n; i++)
        used[i] = false;

    if (backtrack(0))
        printf("Graphs are ISOMORPHIC\n");
    else
        printf("Graphs are NOT isomorphic\n");

    return 0;
}
