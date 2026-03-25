# Bell State and CHSH Simulation

This folder contains a compact Qiskit example that:

- prepares the Bell state |Φ+⟩ = (|00⟩ + |11⟩)/√2
- estimates the correlation terms used in the CHSH inequality
- computes the CHSH value S
- demonstrates a Bell inequality violation numerically

## File

- `code/bell_state.py`

## How to run

```bash
pip install qiskit qiskit-aer
python code/bell_state.py
```

## Why this matters

Bell inequality violations show that quantum correlations cannot be explained by
local hidden-variable models. This is central to quantum foundations and is also
relevant to device-independent quantum cryptography.
