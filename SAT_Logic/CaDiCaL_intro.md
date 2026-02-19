# CaDiCaL: A Modern SAT Solver

## Abstract

CaDiCaL is a state-of-the-art **Conflict-Driven Clause Learning (CDCL) SAT solver** developed by Armin Biere. SAT solvers are tools for determining the satisfiability of Boolean formulas expressed in **Conjunctive Normal Form (CNF)**. CaDiCaL is widely used in research, industrial verification, cryptography, and combinatorial problem solving. This article provides an overview of CaDiCaL, including its background, installation, usage, modern features, and practical examples.

---

## 1. Introduction

The Boolean **Satisfiability Problem (SAT)** is a fundamental problem in theoretical computer science:

> Given a Boolean formula, is there an assignment of True/False values to the variables that makes the formula true?

SAT is **NP-complete** (Cook–Levin theorem, 1971), meaning it is computationally hard in the worst case. Despite this, modern SAT solvers such as CaDiCaL efficiently handle large, structured instances encountered in practice.

CaDiCaL builds upon **CDCL algorithms**, combining:

- **Decision heuristics** to choose branching variables
- **Conflict-driven clause learning** for pruning search space
- **Watched literals** for fast unit propagation
- **Preprocessing and simplification** to reduce problem size

Its performance and flexibility make it an attractive choice for researchers.

---

## 2. Installation

CaDiCaL can be installed on Linux, macOS, and Windows. For Linux systems:

### 2.1 Using Package Manager

```bash
sudo apt install cadical
```

This installs the command-line solver ready for immediate use.

### 2.2 Building from Source

1. Clone the repository:

```bash
git clone https://github.com/arminbiere/cadical.git
cd cadical
```

2. Compile:

```bash
./configure
make
```

3. Verify installation:

```bash
./cadical --version
```

### 2.3 Using with Python (PySAT)

For Python integration:

```bash
pip install python-sat[pblib,aiger]
```

Then in Python:

```python
from pysat.solvers import CaDiCaL

solver = CaDiCaL()
solver.add_clause([1, -2])
solver.add_clause([-1, 2])
solver.solve()
```

This allows rapid prototyping and integration with Sage or other Python workflows.

---

## 3. Using CaDiCaL with DIMACS CNF

CaDiCaL accepts **CNF files in DIMACS format**.  

### 3.1 CNF Format

- Each variable is an integer (positive = True, negative = False)
- Clauses are lists of literals ending with `0`
- The header line defines variables and clauses: `p cnf <num_vars> <num_clauses>`

#### Example CNF (`example.cnf`)

```
p cnf 3 3
1 -2 0
-1 2 0
-2 -3 0
```

- 3 variables, 3 clauses
- Satisfiable assignment: `x1 = True, x2 = True, x3 = False`

### 3.2 Running CaDiCaL

```bash
cadical example.cnf
```

Output:

```
s SATISFIABLE
v 1 2 -3 0
```

- `s SATISFIABLE` indicates satisfiable
- `v` line provides one satisfying assignment
- Trailing `0` is a **line terminator** (DIMACS convention)

### 3.3 Advanced CLI Options

- **Incremental solving**: `--incremental`  
- **Proof logging**: `--proof=proof.txt` (for UNSAT certificates)  
- **Timeout**: `--time=seconds`  

These options enable researchers to conduct experiments and formally verify results.

---

## 4. Modern Features of CaDiCaL

### 4.1 Conflict-Driven Clause Learning (CDCL)

CaDiCaL implements a modern CDCL algorithm with:

- **Unit propagation**
- **Clause learning**
- **Non-chronological backtracking (backjumping)**

This enables efficient pruning of large search spaces.

### 4.2 Incremental Solving

- Allows adding new clauses dynamically without restarting
- Useful for problems like bounded model checking or interactive SAT

### 4.3 Proof Logging

- Generates **UNSAT certificates** in DRAT format
- Supports formal verification and certifying algorithms

### 4.4 Heuristics and Restarts

- Variable selection heuristics (VSIDS-like)
- Adaptive restarts improve performance on hard instances
- Preprocessing simplifies formulas before solving

---

## 5. Example Research Application

### 5.1 Graph Coloring

Encode a 3-node, 2-color graph coloring problem:

```
p cnf 4 4
1 2 0      # Node 1: color 1 or 2
-1 -3 0    # Node 1 & 2 different colors
-2 -4 0    # Node 2 & 3 different colors
1 -4 0     # Node 1 & 3 different colors
```

Run CaDiCaL:

```bash
cadical graph.cnf
```

- Output gives a valid coloring assignment
- Can scale to hundreds of nodes efficiently

### 5.2 Integration with Python/Sage

```python
from pysat.solvers import CaDiCaL

solver = CaDiCaL()
solver.add_clause([1, 2])
solver.add_clause([-1, -3])
solver.add_clause([-2, -4])
solver.add_clause([1, -4])

if solver.solve():
    print("SAT assignment:", solver.get_model())
else:
    print("UNSAT")
```

- Useful for **combinatorial, cryptographic, and number-theoretic problems**

---

## 6. Why SAT is NP-Complete Despite Fast Solvers

- SAT is **NP-complete**, meaning **worst-case exponential complexity**
- Small or structured instances (like the examples above) can be solved in milliseconds
- Modern solvers exploit **heuristics, clause learning, and structure**
- Hard instances exist (e.g., random 3-SAT near phase transition) where solving takes significant time
- NP-completeness refers to **worst-case difficulty**, not practical runtime on typical formulas

---

## 7. Conclusion

CaDiCaL is a **modern, high-performance SAT solver**:

- Supports CDCL, incremental solving, and proof logging
- Efficient for small and large structured SAT instances
- Integrates with Python for prototyping and research
- Widely used in academia and industrial verification

Its flexibility and performance make it a **preferred choice for PhD research and experimental SAT projects**.

---

## References

1. Biere, Armin. *CaDiCaL SAT Solver*. GitHub repository: [https://github.com/arminbiere/cadical](https://github.com/arminbiere/cadical)  
2. Biere, Armin. *CaDiCaL 2.0: Architecture and Features*, CAV 2024.  
3. Biere, Armin, et al. *Handbook of Satisfiability*, 2009.  
4. Biere, Armin. *DRAT Proof Logging and Certificates*.

---

## 8. Example DIMACS CNF Files

### Simple 3-variable example

```
p cnf 3 3
1 -2 0
-1 2 0
-2 -3 0
```

### 4-variable graph coloring example

```
p cnf 4 4
1 2 0
-1 -3 0
-2 -4 0
1 -4 0
```

### Running in the command line

```bash
cadical example.cnf
cadical graph.cnf
```

### Using Python/PySAT

```python
from pysat.solvers import CaDiCaL
solver = CaDiCaL()
solver.add_clause([1, -2])
solver.add_clause([-1, 2])
solver.solve()
print(solver.get_model())
```

