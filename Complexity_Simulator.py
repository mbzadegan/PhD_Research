import itertools
import math

def proof_complexity_simulation(formula, proof_steps):
    """
    Simulate a naive propositional proof verifier.
    """
    print("\n=== Proof Complexity ===")
    print(f"Formula: {formula}")
    print("Proof steps:")
    for i, step in enumerate(proof_steps):
        print(f"  Step {i+1}: {step}")
    print("✓ Proof verified (toy model)")

def circuit_complexity_simulation(truth_table):
    """
    Estimate circuit complexity via formula size (very rough).
    """
    print("\n=== Circuit Complexity ===")
    n = len(truth_table[0][0])  # input size
    size_estimate = 2 ** n  # Upper bound for formula size
    print(f"Input size: {n} bits")
    print(f"Estimated circuit size: O({size_estimate})")


def communication_complexity_equality(x, y):
    """
    Simulate communication complexity of equality function (naive model).
    """
    print("\n=== Communication Complexity ===")
    if x == y:
        result = "equal"
    else:
        result = "not equal"
    bits_needed = len(x)  # naive case: send full input
    print(f"Alice sends {bits_needed} bits to Bob")
    print(f"Result: x and y are {result}")


def kolmogorov_complexity(string):
    """
    Meta-complexity: estimate string complexity via compression. (very rough)
    """
    print("\n=== Meta-Complexity (Kolmogorov Complexity) ===")
    unique_chars = set(string)
    compressed_length = len(unique_chars) + math.ceil(math.log2(len(string)+1))
    print(f"String: {string}")
    print(f"Estimated Kolmogorov complexity: ~{compressed_length} bits")


def main():
    # 1. Proof Complexity: toy proof of a tautology
    formula = "(A ∨ ¬A)"  # tautology
    proof_steps = [
        "Assume A is true → A ∨ ¬A",
        "Assume A is false → ¬A → A ∨ ¬A",
        "Therefore, A ∨ ¬A is always true"
    ]
    proof_complexity_simulation(formula, proof_steps)

    # 2. Circuit Complexity: toy truth table for XOR
    truth_table = [
        ((0,0), 0),
        ((0,1), 1),
        ((1,0), 1),
        ((1,1), 0),
    ]
    circuit_complexity_simulation(truth_table)

    # 3. Communication Complexity: equality check
    x = "1011"
    y = "1011"
    communication_complexity_equality(x, y)

    # 4. Meta-Complexity: complexity of a string
    string = "abababababab"
    kolmogorov_complexity(string)

    # 5. Connection to algorithms
    print("\n=== Algorithmic Connections ===")
    print("Each complexity notion was explored using simple algorithms.")
    print("Real-world algorithms aim to optimize size, time, or information.")

if __name__ == "__main__":
    main()
