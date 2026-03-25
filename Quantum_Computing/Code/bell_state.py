"""
bell_state.py

A small Qiskit-based demonstration of Bell-state preparation and CHSH-style
correlation estimation. This is intended as a compact educational example for a
GitHub portfolio related to quantum information foundations.

Requirements:
    pip install qiskit qiskit-aer

Notes:
- The script prepares the Bell state |Φ+⟩ = (|00⟩ + |11⟩)/√2
- It estimates correlations E(a,b) for four angle pairs
- It computes the CHSH quantity:
      S = E(a0,b0) + E(a0,b1) + E(a1,b0) - E(a1,b1)
- For suitable angle choices, quantum mechanics predicts |S| up to 2√2
"""

from math import pi
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator


def make_bell_circuit() -> QuantumCircuit:
    """Prepare the Bell state |Φ+⟩ = (|00⟩ + |11⟩)/√2."""
    qc = QuantumCircuit(2, 2)
    qc.h(0)
    qc.cx(0, 1)
    return qc


def add_measurement_for_angle(qc: QuantumCircuit, theta_a: float, theta_b: float) -> QuantumCircuit:
    """
    Measure each qubit in a basis parameterized by an angle.
    A simple way to emulate measurements along different axes is to rotate
    before measuring in the computational basis.

    Here we use Ry(-theta) before the Z-basis measurement.
    """
    test = qc.copy()
    test.ry(-theta_a, 0)
    test.ry(-theta_b, 1)
    test.measure(0, 0)
    test.measure(1, 1)
    return test


def correlation_from_counts(counts: dict, shots: int) -> float:
    """
    Convert bitstring counts into a correlation value E in [-1, 1].

    Convention:
      same outcomes   -> +1   (00, 11)
      different       -> -1   (01, 10)
    """
    same = counts.get("00", 0) + counts.get("11", 0)
    different = counts.get("01", 0) + counts.get("10", 0)
    return (same - different) / shots


def estimate_correlation(theta_a: float, theta_b: float, shots: int = 4096) -> float:
    """Run one correlation experiment for the given measurement angles."""
    sim = AerSimulator()
    qc = make_bell_circuit()
    qc = add_measurement_for_angle(qc, theta_a, theta_b)
    result = sim.run(qc, shots=shots).result()
    counts = result.get_counts()
    return correlation_from_counts(counts, shots)


def main() -> None:
    # Standard CHSH angle choice for the Bell state |Φ+⟩
    a0 = 0
    a1 = pi / 2
    b0 = pi / 4
    b1 = -pi / 4

    shots = 4096

    e_a0b0 = estimate_correlation(a0, b0, shots)
    e_a0b1 = estimate_correlation(a0, b1, shots)
    e_a1b0 = estimate_correlation(a1, b0, shots)
    e_a1b1 = estimate_correlation(a1, b1, shots)

    s = e_a0b0 + e_a0b1 + e_a1b0 - e_a1b1

    print("Estimated CHSH correlations")
    print(f"E(a0, b0) = {e_a0b0:.4f}")
    print(f"E(a0, b1) = {e_a0b1:.4f}")
    print(f"E(a1, b0) = {e_a1b0:.4f}")
    print(f"E(a1, b1) = {e_a1b1:.4f}")
    print()
    print(f"CHSH S = {s:.4f}")
    print("Classical bound: |S| <= 2")
    print(f"Quantum maximum: 2*sqrt(2) ≈ {2 * (2 ** 0.5):.4f}")

    if abs(s) > 2:
        print("Result: CHSH violation observed (within sampling noise).")
    else:
        print("Result: No violation observed in this run. Increase shots or check angles.")


if __name__ == "__main__":
    main()
