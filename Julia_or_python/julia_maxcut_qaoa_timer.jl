using Random
using Graphs
using Optim
using LinearAlgebra
using Printf

# -----------------------------
# MaxCut graph
# -----------------------------
const N_QUBITS = 14
const EDGE_PROBABILITY = 0.35
const QAOA_DEPTH = 3
const MAX_ITERATIONS = 80
const SEED = 1234

Random.seed!(SEED)

graph = erdos_renyi(N_QUBITS, EDGE_PROBABILITY, seed=SEED)
edges_list = [(src(e), dst(e)) for e in edges(graph)]

println("Number of qubits: ", N_QUBITS)
println("Number of edges: ", length(edges_list))
println("QAOA depth p: ", QAOA_DEPTH)

# -----------------------------
# MaxCut objective
# -----------------------------
function maxcut_value(index::Int, edges_list)
    value = 0

    for (i, j) in edges_list
        bit_i = (index >> (i - 1)) & 1
        bit_j = (index >> (j - 1)) & 1

        if bit_i != bit_j
            value += 1
        end
    end

    return value
end

function apply_rx!(state, qubit, beta, n)
    c = cos(beta)
    s = -im * sin(beta)

    step = 1 << (qubit - 1)
    block = step << 1
    size = length(state)

    for base in 0:block:(size - 1)
        for offset in 0:(step - 1)
            i0 = base + offset + 1
            i1 = i0 + step

            a = state[i0]
            b = state[i1]

            state[i0] = c * a + s * b
            state[i1] = s * a + c * b
        end
    end
end

function expected_cut_value(params)
    gammas = params[1:QAOA_DEPTH]
    betas = params[QAOA_DEPTH+1:end]

    dim = 1 << N_QUBITS
    state = fill(complex(1 / sqrt(dim), 0.0), dim)

    for layer in 1:QAOA_DEPTH
        gamma = gammas[layer]
        beta = betas[layer]

        # Cost Hamiltonian phase separator
        for index in 0:(dim - 1)
            cut = maxcut_value(index, edges_list)
            state[index + 1] *= exp(-im * gamma * cut)
        end

        # Mixer Hamiltonian
        for q in 1:N_QUBITS
            apply_rx!(state, q, beta, N_QUBITS)
        end
    end

    expectation = 0.0

    for index in 0:(dim - 1)
        probability = abs2(state[index + 1])
        expectation += probability * maxcut_value(index, edges_list)
    end

    # Optim minimizes, so return negative value
    return -expectation
end

# -----------------------------
# Run benchmark
# -----------------------------
initial_params = rand(2 * QAOA_DEPTH) .* π

start_time = time()

result = optimize(
    expected_cut_value,
    initial_params,
    NelderMead(),
    Optim.Options(iterations = MAX_ITERATIONS)
)

end_time = time()

best_expected_value = -Optim.minimum(result)
best_parameters = Optim.minimizer(result)
total_time = end_time - start_time

println()
println("========== RESULTS ==========")
@printf("Best expected MaxCut value: %.6f\n", best_expected_value)
println("Best parameters: ", best_parameters)
println("Optimizer converged: ", Optim.converged(result))
@printf("Total processing time: %.6f seconds\n", total_time)
println("=============================")