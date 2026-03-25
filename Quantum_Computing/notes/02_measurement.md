# Measurement

Quantum measurement is described by operators acting on a Hilbert space.

## Projective Measurement
For the computational basis:
P₀ = |0⟩⟨0|,  P₁ = |1⟩⟨1|.

Given |ψ⟩ = α|0⟩ + β|1⟩:
- Pr(0) = ⟨ψ|P₀|ψ⟩ = |α|²
- Pr(1) = ⟨ψ|P₁|ψ⟩ = |β|²

Post-measurement states:
- 0 → |0⟩
- 1 → |1⟩

## General Measurements (POVMs)
More generally, a measurement is given by { M_k } with ∑ₖ M_k† M_k = I.
Probability: Pr(k) = ⟨ψ| M_k† M_k |ψ⟩.

## Irreversibility
Measurement is generally non-unitary and irreversible, in contrast to unitary evolution.

## Cryptographic Note
Measurement disturbance enables eavesdropping detection in QKD.
