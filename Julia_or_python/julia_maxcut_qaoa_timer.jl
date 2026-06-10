using Random
using Optim
using Printf

const N_QUBITS = 14
const QAOA_DEPTH = 3
const MAX_ITERATIONS = 80
const SEED = 1234

const edges_list = [
    (1, 2), (1, 5), (2, 3), (2, 7), (3, 4),
    (4, 8), (5, 6), (6, 7), (7, 8), (8, 9),
    (9, 10), (10, 11), (11, 12), (12, 13), (13, 14),
    (3, 10), (4, 12), (6, 14), (2, 11), (5, 13),
    (1, 9), (7, 12), (8, 14), (4, 6), (10, 14)
]

Random.seed!(SEED)

println("Number of qubits: ", N_QUBITS)
println("Number of edges: ", length(edges_list))
println("QAOA depth p: ", QAOA_DEPTH)


function maxcut_value(index::Int)
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


function apply_rx!(state, qubit, beta)
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

        for index in 0:(dim - 1)
            cut = maxcut_value(index)
            state[index + 1] *= exp(-im * gamma * cut)
        end

        for q in 1:N_QUBITS
            apply_rx!(state, q, beta)
        end
    end

    expectation = 0.0

    for index in 0:(dim - 1)
        probability = abs2(state[index + 1])
        expectation += probability * maxcut_value(index)
    end

    return -expectation
end


initial_params = rand(2 * QAOA_DEPTH) .* π

start_time = time()

result = optimize(
    expected_cut_value,
    initial_params,
    NelderMead(),
    Optim.Options(iterations = MAX_ITERATIONS)
)

end_time = time()

println()
println("========== RESULTS ==========")
@printf("Best expected MaxCut value: %.6f\n", -Optim.minimum(result))
println("Best parameters: ", Optim.minimizer(result))
println("Optimizer converged: ", Optim.converged(result))
@printf("Total processing time: %.6f seconds\n", end_time - start_time)
println("=============================")
