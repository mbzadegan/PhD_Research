import time
import numpy as np
import networkx as nx
from scipy.optimize import minimize

from qiskit import QuantumCircuit, transpile
from qiskit_aer import AerSimulator


# -----------------------------
# MaxCut graph
# -----------------------------
N_QUBITS = 14
EDGE_PROBABILITY = 0.35
QAOA_DEPTH = 3
MAX_ITERATIONS = 80
SEED = 1234

rng = np.random.default_rng(SEED)
graph = nx.erdos_renyi_graph(N_QUBITS, EDGE_PROBABILITY, seed=SEED)
edges = list(graph.edges())

print(f"Number of qubits: {N_QUBITS}")
print(f"Number of edges: {len(edges)}")
print(f"QAOA depth p: {QAOA_DEPTH}")


# -----------------------------
# MaxCut objective
# -----------------------------
def maxcut_value(bitstring, edges):
    value = 0
    for i, j in edges:
        if bitstring[i] != bitstring[j]:
            value += 1
    return value


def build_qaoa_circuit(params):
    gammas = params[:QAOA_DEPTH]
    betas = params[QAOA_DEPTH:]

    qc = QuantumCircuit(N_QUBITS)

    # Initial |+> state
    qc.h(range(N_QUBITS))

    for layer in range(QAOA_DEPTH):
        gamma = gammas[layer]
        beta = betas[layer]

        # Cost Hamiltonian for MaxCut
        for i, j in edges:
            qc.rzz(2.0 * gamma, i, j)

        # Mixer Hamiltonian
        for q in range(N_QUBITS):
            qc.rx(2.0 * beta, q)

    qc.save_statevector()
    return qc


def expected_cut_value(params, simulator):
    qc = build_qaoa_circuit(params)
    tqc = transpile(qc, simulator, optimization_level=0)

    result = simulator.run(tqc).result()
    state = np.asarray(result.get_statevector(tqc))

    expectation = 0.0

    for index, amplitude in enumerate(state):
        probability = abs(amplitude) ** 2

        if probability > 1e-12:
            bitstring = format(index, f"0{N_QUBITS}b")
            expectation += probability * maxcut_value(bitstring, edges)

    # scipy minimizes, so return negative value
    return -expectation


# -----------------------------
# Run benchmark
# -----------------------------
simulator = AerSimulator(method="statevector")

initial_params = rng.uniform(
    low=0.0,
    high=np.pi,
    size=2 * QAOA_DEPTH
)

start_time = time.perf_counter()

result = minimize(
    expected_cut_value,
    initial_params,
    args=(simulator,),
    method="COBYLA",
    options={"maxiter": MAX_ITERATIONS}
)

end_time = time.perf_counter()

best_expected_value = -result.fun
total_time = end_time - start_time

print("\n========== RESULTS ==========")
print(f"Best expected MaxCut value: {best_expected_value:.6f}")
print(f"Best parameters: {result.x}")
print(f"Optimizer success: {result.success}")
print(f"Optimizer message: {result.message}")
print(f"Total processing time: {total_time:.6f} seconds")
print("=============================")
