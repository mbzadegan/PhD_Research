// K Shortest Paths using Dijkstra + Yen's Algorithm
// Advanced CLRS book style implementation

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define INF INT_MAX
#define MAXV 100

struct Edge {
    int dest;
    int weight;
    struct Edge* next;
};

struct Graph {
    int V;
    struct Edge** adj;
};

struct Path {
    int cost;
    int vertices[MAXV];
    int len;
};

struct Graph* createGraph(int V) {
    struct Graph* graph = malloc(sizeof(struct Graph));
    graph->V = V;
    graph->adj = malloc(V * sizeof(struct Edge*));
    for (int i = 0; i < V; i++)
        graph->adj[i] = NULL;
    return graph;
}

void addEdge(struct Graph* graph, int src, int dest, int weight) {
    struct Edge* e = malloc(sizeof(struct Edge));
    e->dest = dest;
    e->weight = weight;
    e->next = graph->adj[src];
    graph->adj[src] = e;
}

// ---------- Simple Dijkstra to compute one shortest path ----------
void dijkstra(struct Graph* g, int src, int dest, int* dist, int* parent) {
    int V = g->V;
    int visited[V];
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        parent[i] = -1;
        visited[i] = 0;
    }
    dist[src] = 0;

    for (int count = 0; count < V - 1; count++) {
        int u = -1;
        int min = INF;
        for (int v = 0; v < V; v++)
            if (!visited[v] && dist[v] <= min) {
                min = dist[v];
                u = v;
            }

        if (u == -1) break;
        visited[u] = 1;

        struct Edge* e = g->adj[u];
        while (e) {
            int v = e->dest;
            if (!visited[v] && dist[u] != INF && dist[u] + e->weight < dist[v]) {
                dist[v] = dist[u] + e->weight;
                parent[v] = u;
            }
            e = e->next;
        }
    }
}

// ---------- Reconstruct path from parent array ----------
struct Path buildPath(int* parent, int src, int dest, int dist) {
    struct Path p;
    p.len = 0;
    p.cost = dist;
    int v = dest;
    while (v != -1) {
        p.vertices[p.len++] = v;
        if (v == src) break;
        v = parent[v];
    }
    // reverse
    for (int i = 0; i < p.len / 2; i++) {
        int tmp = p.vertices[i];
        p.vertices[i] = p.vertices[p.len - 1 - i];
        p.vertices[p.len - 1 - i] = tmp;
    }
    return p;
}

// ---------- Print a path ----------
void printPath(struct Path p) {
    printf("Cost = %d, Path: ", p.cost);
    for (int i = 0; i < p.len; i++)
        printf("%d%s", p.vertices[i], (i == p.len - 1 ? "\n" : " -> "));
}

// ---------- Yen's K Shortest Paths ----------
void yenKShortest(struct Graph* g, int src, int dest, int K) {
    int dist[MAXV], parent[MAXV];
    struct Path A[K];   // shortest paths
    struct Path B[K*MAXV]; // potential next paths
    int Bcount = 0;

    // First shortest path
    dijkstra(g, src, dest, dist, parent);
    if (dist[dest] == INF) {
        printf("No path found.\n");
        return;
    }
    A[0] = buildPath(parent, src, dest, dist[dest]);
    printf("1st shortest path:\n");
    printPath(A[0]);

    int Acnt = 1;
    // Generate up to K paths
    for (int k = 1; k < K; k++) {
        Bcount = 0;
        for (int i = 0; i < A[k - 1].len - 1; i++) {
            int spurNode = A[k - 1].vertices[i];
            struct Graph* gCopy = createGraph(g->V);

            // Copy graph
            for (int u = 0; u < g->V; u++) {
                struct Edge* e = g->adj[u];
                while (e) {
                    struct Edge* ne = malloc(sizeof(struct Edge));
                    *ne = *e;
                    ne->next = gCopy->adj[u];
                    gCopy->adj[u] = ne;
                    e = e->next;
                }
            }

            // Remove edges in previous paths that share the same root
            for (int j = 0; j < Acnt; j++) {
                int match = 1;
                for (int m = 0; m <= i; m++) {
                    if (A[j].vertices[m] != A[k - 1].vertices[m]) {
                        match = 0;
                        break;
                    }
                }
                if (match && i + 1 < A[j].len) {
                    int u = A[j].vertices[i];
                    int v = A[j].vertices[i + 1];
                    // remove edge u->v
                    struct Edge** ptr = &gCopy->adj[u];
                    while (*ptr) {
                        if ((*ptr)->dest == v) {
                            *ptr = (*ptr)->next;
                            break;
                        }
                        ptr = &((*ptr)->next);
                    }
                }
            }

            // Run Dijkstra from spurNode
            dijkstra(gCopy, spurNode, dest, dist, parent);
            if (dist[dest] != INF) {
                struct Path spur = buildPath(parent, spurNode, dest, dist[dest]);

                struct Path root = A[k - 1];
                struct Path total;
                total.len = 0;
                total.cost = 0;

                // Combine root + spur
                for (int t = 0; t <= i; t++)
                    total.vertices[total.len++] = root.vertices[t];
                for (int t = 1; t < spur.len; t++)
                    total.vertices[total.len++] = spur.vertices[t];

                total.cost = 0;
                for (int t = 0; t < total.len - 1; t++) {
                    struct Edge* e = g->adj[total.vertices[t]];
                    while (e && e->dest != total.vertices[t + 1])
                        e = e->next;
                    if (e)
                        total.cost += e->weight;
                }

                B[Bcount++] = total;
            }

            free(gCopy->adj);
            free(gCopy);
        }

        if (Bcount == 0) break;

        // Find the minimal cost in B[]
        int minIdx = 0;
        for (int i = 1; i < Bcount; i++)
            if (B[i].cost < B[minIdx].cost)
                minIdx = i;

        A[Acnt++] = B[minIdx];
        printf("%dth shortest path:\n", k + 1);
        printPath(A[Acnt - 1]);
    }
}

// ---------- Example ----------
int main() {
    struct Graph* g = createGraph(6);
    addEdge(g, 0, 1, 7);
    addEdge(g, 0, 2, 9);
    addEdge(g, 0, 5, 14);
    addEdge(g, 1, 2, 10);
    addEdge(g, 1, 3, 15);
    addEdge(g, 2, 3, 11);
    addEdge(g, 2, 5, 2);
    addEdge(g, 3, 4, 6);
    addEdge(g, 4, 5, 9);

    int src = 0, dest = 4, K = 3;
    yenKShortest(g, src, dest, K);
    return 0;
}
