// File: comm_complexity_equality.c
#include <stdio.h>
#include <string.h>

// Alice sends her full string to Bob (worst-case protocol)
int naive_communication_protocol(const char *alice, const char *bob, int n, int *bits_sent) {
    *bits_sent = n;
    return strcmp(alice, bob) == 0;
}

// Randomized protocol: Alice sends a simple checksum (e.g., XOR of bits)
int randomized_protocol(const char *alice, const char *bob, int n, int *bits_sent) {
    char alice_parity = 0, bob_parity = 0;
    for (int i = 0; i < n; i++) {
        alice_parity ^= alice[i];
        bob_parity ^= bob[i];
    }
    *bits_sent = 1;
    return alice_parity == bob_parity;
}

int main() {
    const char *alice_input = "11010101";
    const char *bob_input   = "11010101"; // Try changing to e.g. "11010100"

    int bits_sent;
    int n = strlen(alice_input);

    printf("=== Communication Complexity Demo ===\n");
    printf("Alice: %s\n", alice_input);
    printf("Bob:   %s\n\n", bob_input);

    printf("[Naive Protocol]\n");
    int equal_naive = naive_communication_protocol(alice_input, bob_input, n, &bits_sent);
    printf("Equality result: %s\n", equal_naive ? "Equal" : "Not equal");
    printf("Bits communicated: %d\n\n", bits_sent);

    printf("[Randomized Protocol (1-bit checksum)]\n");
    int equal_random = randomized_protocol(alice_input, bob_input, n, &bits_sent);
    printf("Equality approx. result: %s\n", equal_random ? "Probably equal" : "Not equal");
    printf("Bits communicated: %d\n", bits_sent);

    return 0;
}
