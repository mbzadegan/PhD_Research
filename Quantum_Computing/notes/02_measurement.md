# Quantum Measurement: Basic Intuition

Quantum measurement is the process by which a quantum state produces a classical outcome. In contrast to classical systems, measurement in quantum mechanics generally changes the state being measured.

## Computational Basis Measurement

Suppose a qubit is in the state

\[
|\psi\rangle = \alpha |0\rangle + \beta |1\rangle
\]

with \(|\alpha|^2 + |\beta|^2 = 1\). Measuring in the computational basis gives:

- \(0\) with probability \(|\alpha|^2\)
- \(1\) with probability \(|\beta|^2\)

After measurement, the state collapses to the basis state corresponding to the observed outcome.

## Projective Measurement

A projective measurement is described by orthogonal projection operators. For the computational basis, these are:

\[
P_0 = |0\rangle \langle 0|,\qquad P_1 = |1\rangle \langle 1|
\]

The probability of outcome \(i\) is:

\[
p(i) = \langle \psi | P_i | \psi \rangle
\]

and the post-measurement state is the normalized projection onto the measured subspace.

## Why Measurement Matters in Cryptography

Measurement disturbance is a central reason why quantum cryptography is possible. If an eavesdropper measures a quantum state without knowing the correct basis, the act of measurement can disturb the system and reveal the intrusion.
