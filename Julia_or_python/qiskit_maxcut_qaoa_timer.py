import time
import numpy as np
from scipy.optimize import minimize
from qiskit import QuantumCircuit, transpile
from qiskit_aer import AerSimulator

N_QUBITS = 14
QAOA_DEPTH = 3
MAX_ITERATIONS = 80
SEED = 1234

edges = [
    (0, 1), (0, 4), (1, 2), (1, 6), (2, 3),
    (3, 7), (4, 5), (5, 6), (6, 7), (7, 8),
    (8, 9), (9, 10), (10, 11), (11, 12), (12, 13),
    (2, 9), (3, 11), (5, 13), (1, 10), (4, 12),
    (0, 8), (6, 11), (7, 13), (3, 5), (9, 13)
]

rng = np.random.default_rng(SEED)

print(f"Number of qubits: {N_QUBITS}")
print(f"Number of edges: {len(edges)}")
print(f"QAOA depth p: {QAOA_DEPTH}")


def maxcut_value(bitstring):
    value = 0
    for i, j in edges:
        if bitstring[i] != bitstring[j]:
            value += 1
    return value


def build_qaoa_circuit(params):
    gammas = params[:QAOA_DEPTH]
    betas = params[QAOA_DEPTH:]

    qc = QuantumCircuit(N_QUBITS)
    qc.h(range(N_QUBITS))

    for layer in range(QAOA_DEPTH):
        gamma = gammas[layer]
        beta = betas[layer]

        for i, j in edges:
            qc.rzz(2.0 * gamma, i, j)

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
            expectation += probability * maxcut_value(bitstring)

    return -expectation


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

print("\n========== RESULTS ==========")
print(f"Best expected MaxCut value: {-result.fun:.6f}")
print(f"Best parameters: {result.x}")
print(f"Optimizer success: {result.success}")
print(f"Optimizer message: {result.message}")
print(f"Total processing time: {end_time - start_time:.6f} seconds")
print("=============================")
