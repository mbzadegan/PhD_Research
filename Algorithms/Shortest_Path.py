# Project: Shortest Path in a Weighted Graph (Dijkstra vs. Bellman-Ford)
# Given a weighted directed graph 
# G (V, E), implement two different algorithms to find the shortest path from a source node 
#   Dijkstra’s Algorithm (efficient for non-negative weights)
#   Bellman-Ford Algorithm (handles negative weights too)

# Then compare:
#    Their performance (runtime for large graphs)
#    Their correctness (especially when negative edges exist)

# Requirements
#   Input: number of vertices, edges, source node, and edge list (u, v, w).
#   Output: shortest distance from source to all nodes (or "INF" if unreachable).
#   Handle negative weights gracefully.


import heapq

def dijkstra(graph, src):
    n = len(graph)
    dist = [float('inf')] * n
    dist[src] = 0
    pq = [(0, src)]
    while pq:
        d, u = heapq.heappop(pq)
        if d > dist[u]:
            continue
        for v, w in graph[u]:
            if dist[u] + w < dist[v]:
                dist[v] = dist[u] + w
                heapq.heappush(pq, (dist[v], v))
    return dist

def bellman_ford(graph, n, src):
    dist = [float('inf')] * n
    dist[src] = 0
    for _ in range(n-1):
        for u in range(n):
            for v, w in graph[u]:
                if dist[u] != float('inf') and dist[u] + w < dist[v]:
                    dist[v] = dist[u] + w
    # Detect negative cycle
    for u in range(n):
        for v, w in graph[u]:
            if dist[u] != float('inf') and dist[u] + w < dist[v]:
                print("Graph contains negative weight cycle")
                return None
    return dist

# Example usage
if __name__ == "__main__":
    n = 5
    graph = [[] for _ in range(n)]
    edges = [
        (0, 1, 6), (0, 2, 7), (1, 2, 8),
        (1, 3, 5), (1, 4, -4), (2, 3, -3),
        (2, 4, 9), (4, 0, 2)
    ]
    for u, v, w in edges:
        graph[u].append((v, w))
    
    print("Dijkstra:", dijkstra(graph, 0))
    print("Bellman-Ford:", bellman_ford(graph, n, 0))
