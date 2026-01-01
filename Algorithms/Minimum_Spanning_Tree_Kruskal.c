/* Minimum Spanning Tree using Kruskal’s Algorithm

Given a connected, undirected, weighted graph G=(V,E),
find a subset of edges that forms a tree including all vertices and has the minimum total weight.
From CLRS Chapter 23.2 */

#include <stdio.h>
#include <stdlib.h>

// Structure to represent an edge
struct Edge {
    int src, dest, weight;
};

// Structure to represent a subset for Union-Find
struct Subset {
    int parent;
    int rank;
};

// Find set of an element (uses path compression)
int find(struct Subset subsets[], int i) {
    if (subsets[i].parent != i)
        subsets[i].parent = find(subsets, subsets[i].parent);
    return subsets[i].parent;
}

// Union of two sets (by rank)
void Union(struct Subset subsets[], int x, int y) {
    int xroot = find(subsets, x);
    int yroot = find(subsets, y);

    if (subsets[xroot].rank < subsets[yroot].rank)
        subsets[xroot].parent = yroot;
    else if (subsets[xroot].rank > subsets[yroot].rank)
        subsets[yroot].parent = xroot;
    else {
        subsets[yroot].parent = xroot;
        subsets[xroot].rank++;
    }
}

// Compare function for qsort
int compareEdges(const void* a, const void* b) {
    struct Edge* a1 = (struct Edge*)a;
    struct Edge* b1 = (struct Edge*)b;
    return a1->weight - b1->weight;
}

// Kruskal's MST Algorithm
void KruskalMST(struct Edge edges[], int V, int E) {
    struct Edge result[V];  // Store MST edges
    int e = 0;              // Counter for result[]
    int i = 0;              // Counter for sorted edges

    // Step 1: Sort edges by weight
    qsort(edges, E, sizeof(edges[0]), compareEdges);

    // Allocate memory for Union-Find subsets
    struct Subset* subsets = (struct Subset*)malloc(V * sizeof(struct Subset));
    for (int v = 0; v < V; v++) {
        subsets[v].parent = v;
        subsets[v].rank = 0;
    }

    // Step 2: Process edges one by one
    while (e < V - 1 && i < E) {
        struct Edge next = edges[i++];

        int x = find(subsets, next.src);
        int y = find(subsets, next.dest);

        // If including this edge doesn’t form a cycle
        if (x != y) {
            result[e++] = next;
            Union(subsets, x, y);
        }
    }

    // Print the MST
    printf("Edges in the constructed MST:\n");
    int totalWeight = 0;
    for (i = 0; i < e; i++) {
        printf("%d -- %d == %d\n", result[i].src, result[i].dest, result[i].weight);
        totalWeight += result[i].weight;
    }
    printf("Total weight of MST = %d\n", totalWeight);

    free(subsets);
}

// Example graph from CLRS
int main() {
    int V = 4;  // Number of vertices
    int E = 5;  // Number of edges
    struct Edge edges[] = {
        {0, 1, 10},
        {0, 2, 6},
        {0, 3, 5},
        {1, 3, 15},
        {2, 3, 4}
    };

    KruskalMST(edges, V, E);
    return 0;
}
