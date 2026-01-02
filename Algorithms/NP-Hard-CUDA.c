/* Below is an example of solving the Traveling Salesman Problem (TSP) exactly using a brute-force approach. 
This is an ideal candidate for CUDA because checking millions of permutations is highly parallelizable.
The Problem: Traveling Salesman (TSP)Given $N$ cities and the distances between them, what is the shortest possible route that visits each city exactly once and returns to the origin?
The Strategy: We will generate permutations of the path. 

  Since there are (N-1)! Unique paths (fixing the start city), the number of paths explodes quickly.
  CPU: calculating 12 cities (11! \approx 40 million paths) takes seconds, and 15 cities (14! \approx 87$ billion) take hours or days.
  
  GPU (CUDA): We can check millions of paths simultaneously.
  */



// Compile this code after installing the CUDA Toolkit.
// nvcc NP-Hard-CUDA.c -o NP-Hard-CUDA -O3



#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cuda_runtime.h>

// --- CONFIGURATION ---
// Set this to the number of cities you have data for
#define N 5   
#define BLOCK_SIZE 256

// Helper: Calculate factorial
__device__ __host__ long long factorial(int n) {
    long long val = 1;
    for (int i = 2; i <= n; i++) val *= i;
    return val;
}

// Device function: Get path from permutation index
__device__ void get_permutation(long long index, int* path, int n) {
    int available[N];
    for (int i = 0; i < n; i++) available[i] = i;

    path[0] = 0; // Fix start city
    for(int i=0; i<n-1; i++) available[i] = available[i+1];

    for (int i = 1; i < n; i++) {
        long long fact = 1;
        for (int j = 1; j < n - i; j++) fact *= j; 

        int selection = index / fact;
        index %= fact;

        path[i] = available[selection];

        for (int k = selection; k < n - 1 - i; k++) {
            available[k] = available[k + 1];
        }
    }
}

// Kernel: Checks path cost using the lookup matrix
__global__ void tsp_kernel(float* dist_matrix, float* min_dist, long long total_permutations) {
    long long idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= total_permutations) return;

    int path[N];
    get_permutation(idx, path, N);

    float current_dist = 0.0f;
    for (int i = 0; i < N - 1; i++) {
        int from = path[i];
        int to = path[i+1];
        
        // LOOKUP DISTANCE FROM MATRIX
        // Matrix is flattened: row * N + col
        current_dist += dist_matrix[from * N + to];
    }
    
    // Add return to start
    int last = path[N-1];
    int first = path[0];
    current_dist += dist_matrix[last * N + first];

    // Atomic Min Update
    int* addr_as_int = (int*)min_dist;
    int old = *addr_as_int, assumed;
    if (current_dist < *min_dist) {
        do {
            assumed = old;
            if (current_dist >= __int_as_float(assumed)) break;
            old = atomicCAS(addr_as_int, assumed, __float_as_int(current_dist));
        } while (assumed != old);
    }
}

int main() {
    // 1. Allocate host memory for the matrix
    // We use a 1D array to represent a 2D matrix (size N*N)
    float h_matrix[N * N];

    // ==========================================
    // STEP 1: ENTER YOUR DISTANCES HERE
    // ==========================================
    // Example for 5 cities (0 to 4).
    // Row 0 is distances from City 0 to others.
    // Row 1 is distances from City 1 to others, etc.
    
    // City:      0    1    2    3    4
    float data[] = {
        0,   10,  15,  20,  25,  // From City 0
        10,  0,   35,  25,  15,  // From City 1
        15,  35,  0,   30,  10,  // From City 2
        20,  25,  30,  0,   20,  // From City 3
        25,  15,  10,  20,  0    // From City 4
    };

    // Copy your data into the variable the code uses
    for(int i=0; i<N*N; i++) h_matrix[i] = data[i];

    // ==========================================

    long long total_permutations = factorial(N - 1);
    printf("Solving TSP for %d cities. Total paths: %lld\n", N, total_permutations);

    // 2. GPU Allocation
    float *d_matrix, *d_min_dist;
    cudaMalloc(&d_matrix, N * N * sizeof(float));
    cudaMalloc(&d_min_dist, sizeof(float));

    cudaMemcpy(d_matrix, h_matrix, N * N * sizeof(float), cudaMemcpyHostToDevice);

    float initial_min = FLT_MAX;
    cudaMemcpy(d_min_dist, &initial_min, sizeof(float), cudaMemcpyHostToDevice);

    // 3. Launch
    long long num_blocks = (total_permutations + BLOCK_SIZE - 1) / BLOCK_SIZE;
    tsp_kernel<<<num_blocks, BLOCK_SIZE>>>(d_matrix, d_min_dist, total_permutations);
    
    cudaDeviceSynchronize();

    // 4. Result
    float h_min_dist;
    cudaMemcpy(&h_min_dist, d_min_dist, sizeof(float), cudaMemcpyDeviceToHost);

    printf("Minimum Cost found: %.2f\n", h_min_dist);

    cudaFree(d_matrix); cudaFree(d_min_dist);
    return 0;
}
