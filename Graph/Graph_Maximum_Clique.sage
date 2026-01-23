import time
from sage.graphs.graph_generators import graphs

# Generate a hard random graph
n = 600            # adjust to increase hardness
p = 0.5           # edge probability
G = graphs.RandomGNP(n, p)

start = time.time()

# Solve maximum clique by SageMath
clique = G.clique_maximum()

end = time.time()

print("Maximum clique size:", len(clique))
print("SageMath time (seconds):", end - start)
