// A pure C implementation of a Branch and Bound solver for 0-1 Integer Programming (where variables are binary). 
// This captures the core logic of ILP—branching on variables and pruning the search tree—without requiring thousands of lines of code.
// The Algorithm: Branch and Bound
//
// We will solve a maximization problem. The algorithm explores a tree of possible variable assignments (0 or 1).
//     Branch: We define the value of one variable at a time (recurse).
//     Bound (Pruning): At each step, we calculate the "best possible" outcome for the remaining variables. If that potential outcome is worse than a solution we've already found, we stop (prune) that branch.
//     Feasibility: We check if the current constraints are violated.

// C Code: 0-1 ILP Solver, This code solves a problem with:
// $N$ Variables (binary).
// $M$ Constraints ($Ax \le b$).
// An objective function to maximize ($c^T x$).


#include <stdio.h>
#include <stdbool.h>

// --- Problem Configuration ---
#define N_VARS 4        // Number of variables
#define N_CONSTRAINTS 2 // Number of constraints

// Problem Data (Global for simplicity in this example)
// Objective function coefficients (Maximize)
int c[N_VARS] = {8, 11, 6, 4};

// Constraint Matrix A (LHS)
int A[N_CONSTRAINTS][N_VARS] = {
    {5, 7, 4, 3},  // 5x0 + 7x1 + 4x2 + 3x3 <= 14
    {3, 5, 2, 2}   // Constraint 2
};

// Constraint Vector b (RHS)
int b[N_CONSTRAINTS] = {14, 10};

// --- Solver State ---
int best_objective = -1;       // Best value found so far
int best_solution[N_VARS];     // Best variable assignment found
int current_solution[N_VARS];  // Current working assignment

// --- Helper Functions ---

// Calculate the dot product of a specific row in A with the variables
// considering only variables up to 'level' (fixed variables)
int calculate_lhs(int constraint_idx, int level) {
    int sum = 0;
    for (int i = 0; i <= level; i++) {
        sum += A[constraint_idx][i] * current_solution[i];
    }
    return sum;
}

// Check if the current partial assignment is valid regarding constraints
// Returns true if potentially valid, false if already violated
bool is_feasible(int level) {
    for (int i = 0; i < N_CONSTRAINTS; i++) {
        int current_lhs = calculate_lhs(i, level);
        
        // Optimistic check: Assume remaining variables are 0 (since strictly non-negative coeffs in this example).
        // For general ILP with negative coeffs, you'd need a "min potential" check here.
        if (current_lhs > b[i]) {
            return false; 
        }
    }
    return true;
}

// Calculate the upper bound (potential) for the current branch
// Sum of current value + sum of all positive future coefficients
int bound(int current_value, int level) {
    int potential = current_value;
    for (int i = level + 1; i < N_VARS; i++) {
        if (c[i] > 0) {
            potential += c[i];
        }
    }
    return potential;
}

// --- Recursive Solver ---
void branch_and_bound(int level, int current_obj_value) {
    // 1. Base Case: All variables assigned
    if (level == N_VARS - 1) {
        if (current_obj_value > best_objective) {
            // Found a better valid solution
            best_objective = current_obj_value;
            for (int i = 0; i < N_VARS; i++) {
                best_solution[i] = current_solution[i];
            }
        }
        return;
    }

    // Move to next variable
    int next_level = level + 1;

    // --- Branch 1: Try setting x[next_level] = 1 ---
    current_solution[next_level] = 1;
    int val_if_1 = current_obj_value + c[next_level];
    
    // Check feasibility and bounding
    if (is_feasible(next_level)) {
        if (bound(val_if_1, next_level) > best_objective) {
            branch_and_bound(next_level, val_if_1);
        }
    }

    // --- Branch 2: Try setting x[next_level] = 0 ---
    current_solution[next_level] = 0;
    int val_if_0 = current_obj_value; // No value added
    
    if (is_feasible(next_level)) {
        if (bound(val_if_0, next_level) > best_objective) {
            branch_and_bound(next_level, val_if_0);
        }
    }
}

int main() {
    printf("--- Integer Linear Programming Solver (Branch & Bound) ---\n");
    printf("Problem: Maximize Objective with %d vars and %d constraints.\n\n", N_VARS, N_CONSTRAINTS);

    // Start the recursion with level -1 (no variables set yet)
    branch_and_bound(-1, 0);

    // Output Results
    printf("Optimization Complete.\n");
    if (best_objective != -1) {
        printf("Max Objective Value: %d\n", best_objective);
        printf("Variable Assignment:\n");
        printf("[ ");
        for (int i = 0; i < N_VARS; i++) {
            printf("%d ", best_solution[i]);
        }
        printf("]\n");
    } else {
        printf("No feasible solution found.\n");
    }

    return 0;
}
