"""
bb84_simulation.py

A compact educational simulation of the BB84 quantum key distribution protocol.
This version is designed for a GitHub portfolio and emphasizes clarity.

Requirements:
    pip install qiskit qiskit-aer

What it does:
- Alice generates random bits and random bases
- Bob chooses random measurement bases
- Qubits are prepared and measured one by one
- Alice and Bob keep only positions where their bases match
- The script reports the sifted key and the quantum bit error rate (QBER)

Optional:
- You can enable a simple intercept-resend eavesdropper to see how errors rise
"""

import random
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator


def random_bits(n: int) -> list[int]:
    return [random.randint(0, 1) for _ in range(n)]


def random_bases(n: int) -> list[str]:
    return [random.choice(["Z", "X"]) for _ in range(n)]


def prepare_qubit(bit: int, basis: str) -> QuantumCircuit:
    """
    Prepare one BB84 state.

    Z basis:
        bit 0 -> |0>
        bit 1 -> |1>

    X basis:
        bit 0 -> |+> = (|0> + |1>) / sqrt(2)
        bit 1 -> |-> = (|0> - |1>) / sqrt(2)
    """
    qc = QuantumCircuit(1, 1)

    if basis == "Z":
        if bit == 1:
            qc.x(0)
    elif basis == "X":
        if bit == 1:
            qc.x(0)
        qc.h(0)
    else:
        raise ValueError(f"Unknown basis: {basis}")

    return qc


def measure_in_basis(qc: QuantumCircuit, basis: str) -> QuantumCircuit:
    """
    Measure in the chosen basis by rotating to the computational basis first.
    """
    test = qc.copy()

    if basis == "X":
        test.h(0)
    elif basis != "Z":
        raise ValueError(f"Unknown basis: {basis}")

    test.measure(0, 0)
    return test


def run_single_measurement(bit: int, alice_basis: str, bob_basis: str, shots: int = 1) -> int:
    sim = AerSimulator()
    qc = prepare_qubit(bit, alice_basis)
    qc = measure_in_basis(qc, bob_basis)
    result = sim.run(qc, shots=shots, memory=True).result()
    measured = result.get_memory()[0]
    return int(measured)


def intercept_resend(bit: int, alice_basis: str, eve_basis: str) -> tuple[int, str]:
    """
    Eve measures in a random basis and resends the post-measurement bit in that basis.
    """
    eve_bit = run_single_measurement(bit, alice_basis, eve_basis)
    return eve_bit, eve_basis


def simulate_bb84(n: int = 32, with_eve: bool = False) -> dict:
    alice_bits = random_bits(n)
    alice_bases = random_bases(n)
    bob_bases = random_bases(n)

    eve_bases = random_bases(n) if with_eve else [None] * n

    bob_results = []

    for i in range(n):
        sent_bit = alice_bits[i]
        sent_basis = alice_bases[i]

        if with_eve:
            resent_bit, resent_basis = intercept_resend(sent_bit, sent_basis, eve_bases[i])
            bob_bit = run_single_measurement(resent_bit, resent_basis, bob_bases[i])
        else:
            bob_bit = run_single_measurement(sent_bit, sent_basis, bob_bases[i])

        bob_results.append(bob_bit)

    matching_positions = [i for i in range(n) if alice_bases[i] == bob_bases[i]]
    alice_sifted = [alice_bits[i] for i in matching_positions]
    bob_sifted = [bob_results[i] for i in matching_positions]

    if len(alice_sifted) == 0:
        qber = 0.0
    else:
        errors = sum(1 for a, b in zip(alice_sifted, bob_sifted) if a != b)
        qber = errors / len(alice_sifted)

    return {
        "alice_bits": alice_bits,
        "alice_bases": alice_bases,
        "bob_bases": bob_bases,
        "bob_results": bob_results,
        "matching_positions": matching_positions,
        "alice_sifted": alice_sifted,
        "bob_sifted": bob_sifted,
        "qber": qber,
        "eve_bases": eve_bases if with_eve else None,
    }


def bits_to_string(bits: list[int]) -> str:
    return "".join(str(b) for b in bits)


def main() -> None:
    n = 40

    print("=== BB84 without eavesdropping ===")
    clean = simulate_bb84(n=n, with_eve=False)
    print("Alice bases: ", "".join(clean["alice_bases"]))
    print("Bob bases:   ", "".join(clean["bob_bases"]))
    print("Sifted key length:", len(clean["alice_sifted"]))
    print("Alice sifted key:", bits_to_string(clean["alice_sifted"]))
    print("Bob sifted key:  ", bits_to_string(clean["bob_sifted"]))
    print(f"QBER: {100 * clean['qber']:.2f}%")
    print()

    print("=== BB84 with intercept-resend eavesdropping ===")
    attacked = simulate_bb84(n=n, with_eve=True)
    print("Alice bases: ", "".join(attacked["alice_bases"]))
    print("Bob bases:   ", "".join(attacked["bob_bases"]))
    print("Eve bases:   ", "".join(attacked["eve_bases"]))
    print("Sifted key length:", len(attacked["alice_sifted"]))
    print("Alice sifted key:", bits_to_string(attacked["alice_sifted"]))
    print("Bob sifted key:  ", bits_to_string(attacked["bob_sifted"]))
    print(f"QBER: {100 * attacked['qber']:.2f}%")
    print()

    print("Interpretation:")
    print("- With no eavesdropper, the sifted keys should usually agree almost perfectly.")
    print("- With intercept-resend, Eve introduces detectable errors.")
    print("- In real BB84, Alice and Bob estimate the error rate and abort if it is too high.")


if __name__ == "__main__":
    main()
