// Johnson's Algorithm for All-Pairs Shortest Paths
// Based on CLRS Book - Chapter 25.2

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INF INT_MAX

struct Edge {
    int src, dest, weight;
};

struct Graph {
    int V, E;
    struct Edge* edges;
};

// ---------- Utility ----------
struct Graph* createGraph(int V, int E) {
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->V = V;
    graph->E = E;
    graph->edges = (struct Edge*)malloc(E * sizeof(struct Edge));
    return graph;
}

// ---------- Bellman–Ford ----------
int bellmanFord(struct Graph* graph, int src, int* dist) {
    int V = graph->V;
    int E = graph->E;

    for (int i = 0; i < V; i++)
        dist[i] = INF;
    dist[src] = 0;

    for (int i = 1; i <= V - 1; i++) {
        for (int j = 0; j < E; j++) {
            int u = graph->edges[j].src;
            int v = graph->edges[j].dest;
            int w = graph->edges[j].weight;
            if (dist[u] != INF && dist[u] + w < dist[v])
                dist[v] = dist[u] + w;
        }
    }

    // Check for negative cycles
    for (int j = 0; j < E; j++) {
        int u = graph->edges[j].src;
        int v = graph->edges[j].dest;
        int w = graph->edges[j].weight;
        if (dist[u] != INF && dist[u] + w < dist[v]) {
            printf("Graph contains negative weight cycle!\n");
            return 0;
        }
    }
    return 1;
}

// ---------- Dijkstra (Min-Heap Implementation) ----------
int minDistance(int dist[], int visited[], int V) {
    int min = INF, min_index = -1;
    for (int v = 0; v < V; v++)
        if (!visited[v] && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    return min_index;
}

void dijkstra(int** adj, int V, int src, int* result) {
    int dist[V];
    int visited[V];
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        visited[i] = 0;
    }
    dist[src] = 0;

    for (int count = 0; count < V - 1; count++) {
        int u = minDistance(dist, visited, V);
        if (u == -1) break;
        visited[u] = 1;
        for (int v = 0; v < V; v++)
            if (!visited[v] && adj[u][v] != INF && dist[u] != INF
                && dist[u] + adj[u][v] < dist[v])
                dist[v] = dist[u] + adj[u][v];
    }

    for (int i = 0; i < V; i++)
        result[i] = dist[i];
}

// ---------- Johnson’s Algorithm ----------
void johnson(struct Graph* graph) {
    int V = graph->V;

    // Step 1: Add new vertex q
    struct Graph* g2 = createGraph(V + 1, graph->E + V);
    for (int i = 0; i < graph->E; i++)
        g2->edges[i] = graph->edges[i];
    for (int i = 0; i < V; i++) {
        g2->edges[graph->E + i].src = V;
        g2->edges[graph->E + i].dest = i;
        g2->edges[graph->E + i].weight = 0;
    }

    // Step 2: Run Bellman–Ford from q
    int* h = (int*)malloc((V + 1) * sizeof(int));
    if (!bellmanFord(g2, V, h)) {
        free(h);
        return;
    }

    // Step 3: Reweight edges
    int** adj = (int**)malloc(V * sizeof(int*));
    for (int i = 0; i < V; i++) {
        adj[i] = (int*)malloc(V * sizeof(int));
        for (int j = 0; j < V; j++)
            adj[i][j] = INF;
    }

    for (int i = 0; i < graph->E; i++) {
        int u = graph->edges[i].src;
        int v = graph->edges[i].dest;
        int w = graph->edges[i].weight;
        adj[u][v] = w + h[u] - h[v];
    }

    // Step 4: Run Dijkstra from each vertex
    printf("All-Pairs Shortest Paths (Johnson's Algorithm):\n");
    for (int u = 0; u < V; u++) {
        int* dist = (int*)malloc(V * sizeof(int));
        dijkstra(adj, V, u, dist);

        printf("From vertex %d:\n", u);
        for (int v = 0; v < V; v++) {
            if (dist[v] == INF)
                printf("  to %d: INF\n", v);
            else
                printf("  to %d: %d\n", v, dist[v] - h[u] + h[v]);
        }
        printf("\n");
        free(dist);
    }

    free(h);
    for (int i = 0; i < V; i++)
        free(adj[i]);
    free(adj);
}

// ---------- Example ----------
int main() {
    int V = 5;
    int E = 8;
    struct Graph* graph = createGraph(V, E);

    graph->edges[0] = (struct Edge){0, 1, -1};
    graph->edges[1] = (struct Edge){0, 2, 4};
    graph->edges[2] = (struct Edge){1, 2, 3};
    graph->edges[3] = (struct Edge){1, 3, 2};
    graph->edges[4] = (struct Edge){1, 4, 2};
    graph->edges[5] = (struct Edge){3, 2, 5};
    graph->edges[6] = (struct Edge){3, 1, 1};
    graph->edges[7] = (struct Edge){4, 3, -3};

    johnson(graph);
    return 0;
}
