/* Longest Path in a DAG (using Topological Sort)
Given a Directed Acyclic Graph (DAG) with weighted edges, find the longest path from a given source vertex.
Unlike general graphs, the longest paths problem is NP-hard; however, in a DAG, it can be solved in O(V + E) using topological sorting.
From CLRS Chapter 24.2 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INF INT_MIN  // For longest path (negative infinity)

struct Edge {
    int dest;
    int weight;
    struct Edge* next;
};

struct Graph {
    int V;
    struct Edge** adj;
};

// Create a new edge:
struct Edge* newEdge(int dest, int weight) {
    struct Edge* edge = (struct Edge*)malloc(sizeof(struct Edge));
    edge->dest = dest;
    edge->weight = weight;
    edge->next = NULL;
    return edge;
}

// Create a new graph
struct Graph* createGraph(int V) {
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->V = V;
    graph->adj = (struct Edge**)malloc(V * sizeof(struct Edge*));
    for (int i = 0; i < V; i++)
        graph->adj[i] = NULL;
    return graph;
}

// Add a directed edge u -> v with weight w
void addEdge(struct Graph* graph, int u, int v, int w) {
    struct Edge* edge = newEdge(v, w);
    edge->next = graph->adj[u];
    graph->adj[u] = edge;
}

// Topological Sort (DFS-based)
void topologicalSortUtil(struct Graph* graph, int v, int visited[], int stack[], int* top) {
    visited[v] = 1;

    struct Edge* temp = graph->adj[v];
    while (temp) {
        if (!visited[temp->dest])
            topologicalSortUtil(graph, temp->dest, visited, stack, top);
        temp = temp->next;
    }

    stack[(*top)++] = v;  // push to stack
}

// Function to find the longest path from source
void longestPath(struct Graph* graph, int src) {
    int V = graph->V;
    int visited[V];
    int stack[V];
    int top = 0;

    for (int i = 0; i < V; i++)
        visited[i] = 0;

    // 1. Perform topological sort
    for (int i = 0; i < V; i++)
        if (!visited[i])
            topologicalSortUtil(graph, i, visited, stack, &top);

    // 2. Initialize distances to -INF
    int dist[V];
    for (int i = 0; i < V; i++)
        dist[i] = INF;
    dist[src] = 0;

    // 3. Process
