/* Given a directed weighted graph G=(V,E) with possibly negative edge weights,
 Find the shortest path from a source vertex s to every other vertex.
 Unlike the Dijkstra’s algorithm, Bellman–Ford correctly handles negative weights and can detect negative-weight cycles. */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct Edge {
    int src, dest, weight;
};

struct Graph {
    int V, E;
    struct Edge* edge;
};

struct Graph* createGraph(int V, int E) {
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->V = V;
    graph->E = E;
    graph->edge = (struct Edge*)malloc(E * sizeof(struct Edge));
    return graph;
}

void printArr(int dist[], int n) {
    printf("Vertex\tDistance from Source\n");
    for (int i = 0; i < n; i++)
        if (dist[i] == INT_MAX)
            printf("%d\tINF\n", i);
        else
            printf("%d\t%d\n", i, dist[i]);
}

void BellmanFord(struct Graph* graph, int src) {
    int V = graph->V;
    int E = graph->E;
    int dist[V];

    // Step 1: Initialize distances
    for (int i = 0; i < V; i++)
        dist[i] = INT_MAX;
    dist[src] = 0;

    // Step 2: Relax all edges |V| - 1 times
    for (int i = 1; i <= V - 1; i++) {
        for (int j = 0; j < E; j++) {
            int u = graph->edge[j].src;
            int v = graph->edge[j].dest;
            int w = graph->edge[j].weight;
            if (dist[u] != INT_MAX && dist[u] + w < dist[v])
                dist[v] = dist[u] + w;
        }
    }

    // Step 3: Detect negative weight cycles
    for (int j = 0; j < E; j++) {
        int u = graph->edge[j].src;
        int v = graph->edge[j].dest;
        int w = graph->edge[j].weight;
        if (dist[u] != INT_MAX && dist[u] + w < dist[v]) {
            printf("Graph contains a negative weight cycle!\n");
            return;
        }
    }

    printArr(dist, V);
}

int main() {
    int V = 5;  // vertices
    int E = 8;  // edges
    struct Graph* graph = createGraph(V, E);

    // Example from CLRS (Chapter 24.1)
    graph->edge[0] = (struct Edge){0, 1, -1};
    graph->edge[1] = (struct Edge){0, 2, 4};
    graph->edge[2] = (struct Edge){1, 2, 3};
    graph->edge[3] = (struct Edge){1, 3, 2};
    graph->edge[4] = (struct Edge){1, 4, 2};
    graph->edge[5] = (struct Edge){3, 2, 5};
    graph->edge[6] = (struct Edge){3, 1, 1};
    graph->edge[7] = (struct Edge){4, 3, -3};

    BellmanFord(graph, 0);

    free(graph->edge);
    free(graph);

    return 0;
}
