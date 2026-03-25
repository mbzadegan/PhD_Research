from qiskit import QuantumCircuit
from qiskit.visualization import plot_bloch_multivector
from qiskit.quantum_info import Statevector

# Create a single qubit circuit
qc = QuantumCircuit(1)

# Apply Hadamard gate (creates superposition)
qc.h(0)

# Get statevector
state = Statevector.from_instruction(qc)

print("Statevector:", state)

# Visualize on Bloch sphere
plot_bloch_multivector(state).show()
