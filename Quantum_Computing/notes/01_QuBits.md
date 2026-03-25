# Qubits: Basic Representation and Intuition

A qubit is the fundamental unit of quantum information. Unlike a classical bit, which can be either 0 or 1, a qubit can exist in a superposition of both states.

Mathematically, a qubit is represented as a vector in a two-dimensional complex Hilbert space:

\[
|\psi\rangle = \alpha |0\rangle + \beta |1\rangle
\]

where:

- \(\alpha, \beta \in \mathbb{C}\)
- \(|\alpha|^2 + |\beta|^2 = 1\)

This normalization condition ensures that the probabilities sum to one.

## Measurement

When a qubit is measured in the computational basis:

- Outcome \(0\) occurs with probability \(|\alpha|^2\)
- Outcome \(1\) occurs with probability \(|\beta|^2\)

After measurement, the state collapses to the observed basis state.

## Bloch Sphere Representation

A pure qubit state can be visualized on the Bloch sphere:

\[
|\psi\rangle = \cos(\theta/2)|0\rangle + e^{i\phi}\sin(\theta/2)|1\rangle
\]

This provides a geometric interpretation of quantum states.

## Connection to Cryptography

From a cryptographic perspective, the key difference between classical and quantum information is:

- Classical bits can be copied freely
- Quantum states cannot be cloned (no-cloning theorem)

This property is fundamental to quantum cryptography, enabling protocols such as BB84.
