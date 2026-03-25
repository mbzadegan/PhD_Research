### Qubits: Basic Representation and Intuition

A qubit is the fundamental unit of quantum information. Unlike a classical bit, which can be either 0 or 1, a qubit can exist in a superposition of both states.

Mathematically, a qubit is represented as:

|ψ⟩ = α|0⟩ + β|1⟩

where:
- α, β ∈ ℂ  
- |α|² + |β|² = 1  

---

### Measurement

When a qubit is measured in the computational basis:

- Outcome 0 occurs with probability |α|²  
- Outcome 1 occurs with probability |β|²  

After measurement, the state collapses to the observed basis state.

---

### Bloch Sphere Representation

A general qubit can also be written as:

|ψ⟩ = cos(θ/2)|0⟩ + e^{iφ} sin(θ/2)|1⟩

---

### Connection to Cryptography

From a cryptographic perspective:

- Classical bits can be copied freely  
- Quantum states **cannot be cloned** (no-cloning theorem)

This is the foundation of quantum cryptography (e.g., BB84).
