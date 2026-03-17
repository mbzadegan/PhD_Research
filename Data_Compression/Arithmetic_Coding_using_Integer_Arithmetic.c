/*
 * Arithmetic Coding Implementation (Integer Arithmetic with Scaling)
 * Based on "Introduction to Data Compression" book by Khalid Sayood
 * Chapter 4, Section 4.4.3 "Integer Implementation"
 *
 * This program implements:
 * 1. Adaptive Arithmetic Encoding/Decoding.
 * 2. Renormalization (scaling) to handle finite precision.
 * 3. The "E3 Mapping" (Underflow handling) using a follow-on counter.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Constants for 16-bit Precision ---
// As described in Section 4.4.3, we map [0,1) to integer ranges.
#define CODE_VALUE_BITS 16
#define MAX_VALUE       ((1 << CODE_VALUE_BITS) - 1) // 0xFFFF
#define MAX_FREQ        16383 // 2^14 - 1 to prevent overflow during range calc

// Quarter points for rescaling (Section 4.4.2)
#define ONE_QUARTER     (1 << (CODE_VALUE_BITS - 2)) // 0x4000
#define HALF            (2 * ONE_QUARTER)            // 0x8000
#define THREE_QUARTERS  (3 * ONE_QUARTER)            // 0xC000

#define NO_OF_CHARS     256
#define EOF_SYMBOL      (NO_OF_CHARS) // Special symbol to mark end of stream
#define NO_OF_SYMBOLS   (NO_OF_CHARS + 1)

// --- Global Variables for Bit I/O ---
int buffer;      // Buffer for bit I/O
int bits_to_go;  // Number of bits remaining in buffer
int e3_counter;  // "Scale3" variable from Section 4.4.3 for E3 mapping

// --- Adaptive Model Variables ---
// cum_freq[i] is the cumulative frequency of symbol i
int cum_freq[NO_OF_SYMBOLS + 1]; 
int freq[NO_OF_SYMBOLS + 1];

// --- Bit I/O Helper Functions ---

void output_bit(int bit, FILE *f) {
    buffer >>= 1;
    if (bit) buffer |= 0x80; // Put bit in MSB
    bits_to_go--;
    if (bits_to_go == 0) {
        putc(buffer, f);
        bits_to_go = 8;
    }
}

int input_bit(FILE *f) {
    int t;
    if (bits_to_go == 0) {
        buffer = getc(f);
        if (buffer == EOF) {
            // Handle end of stream gracefully (padding 0s)
            buffer = 0; 
        }
        bits_to_go = 8;
    }
    t = buffer & 1;
    buffer >>= 1;
    return t;
}

// Write pending bits stored during E3 mapping (Section 4.4.3)
void bits_plus_follow(int bit, FILE *f) {
    output_bit(bit, f);
    while (e3_counter > 0) {
        output_bit(!bit, f); // Output complement
        e3_counter--;
    }
}

// --- Adaptive Model Functions (Section 4.5) ---

void initialize_model() {
    int i;
    // Initialize frequencies to 1 (Laplace smoothing)
    for (i = 0; i <= NO_OF_SYMBOLS; i++) {
        freq[i] = 1;
        cum_freq[i] = NO_OF_SYMBOLS - i;
    }
    freq[0] = 0; // cum_freq[NO_OF_SYMBOLS] is 0
}

void update_model(int symbol) {
    int i;
    // Increment frequency for the seen symbol
    // In a real implementation, we would rescale if sum exceeds MAX_FREQ
    if (cum_freq[0] >= MAX_FREQ) {
        int cum = 0;
        for (i = NO_OF_SYMBOLS; i >= 0; i--) {
            freq[i] = (freq[i] + 1) / 2;
            cum_freq[i] = cum;
            cum += freq[i];
        }
    }
    
    // Increment frequency and update cumulative counts
    for (i = symbol; i >= 0; i--) {
        cum_freq[i]++;
    }
}

// --- Encoder (Based on Pseudocode on Page 110) ---

void encode(FILE *in, FILE *out) {
    int symbol;
    unsigned int low = 0;
    unsigned int high = MAX_VALUE;
    long range; // Must be larger than int to hold intermediate calculation

    initialize_model();
    bits_to_go = 8;
    e3_counter = 0;

    printf("Encoding...\n");

    for (;;) {
        symbol = getc(in);
        if (symbol == EOF) symbol = EOF_SYMBOL;

        range = (long)(high - low) + 1;
        
        // Update High/Low based on Cumulative Counts (Eq 4.28, 4.29)
        // Note: cum_freq is stored in reverse order in some implementations, 
        // here we assume standard mapping:
        // range for symbol i is [cum_freq[i+1], cum_freq[i])
        high = low + (range * cum_freq[symbol] / cum_freq[0]) - 1;
        low  = low + (range * cum_freq[symbol + 1] / cum_freq[0]);

        // Renormalization Loop (Section 4.4.3)
        for (;;) {
            if (high < HALF) {
                // E1 Condition: Upper half empty -> Output 0
                bits_plus_follow(0, out);
            } 
            else if (low >= HALF) {
                // E2 Condition: Lower half empty -> Output 1
                bits_plus_follow(1, out);
                low -= HALF;
                high -= HALF;
            } 
            else if (low >= ONE_QUARTER && high < THREE_QUARTERS) {
                // E3 Condition: Range straddles midpoint -> Underflow risk
                // Increment counter, discard middle bits (Section 4.4.3)
                e3_counter++;
                low -= ONE_QUARTER;
                high -= ONE_QUARTER;
            } 
            else {
                break; // No scaling needed
            }
            // Scale up (shift left)
            low = 2 * low;
            high = 2 * high + 1;
        }

        update_model(symbol);
        if (symbol == EOF_SYMBOL) break;
    }
    
    // Termination: Output enough bits to disambiguate final range
    e3_counter++;
    if (low < ONE_QUARTER) bits_plus_follow(0, out);
    else bits_plus_follow(1, out);
    
    // Flush buffer
    putc(buffer >> bits_to_go, out);
    printf("Encoding Complete.\n");
}

// --- Decoder (Based on Pseudocode on Page 114) ---

void decode(FILE *in, FILE *out) {
    int i, symbol;
    unsigned int low = 0;
    unsigned int high = MAX_VALUE;
    unsigned int value = 0;
    long range;
    unsigned long count;

    initialize_model();
    bits_to_go = 0; // Force read from file immediately

    printf("Decoding...\n");

    // Initialize the "value" register with the first 16 bits from stream
    for (i = 0; i < CODE_VALUE_BITS; i++) {
        value = 2 * value + input_bit(in);
    }

    for (;;) {
        range = (long)(high - low) + 1;
        
        // Calculate the target count (reverse of encoding)
        // count = ((value - low + 1) * total_freq - 1) / range
        count = ((((long)(value - low) + 1) * cum_freq[0]) - 1) / range;

        // Find symbol corresponding to this count
        for (symbol = 0; symbol < NO_OF_SYMBOLS; symbol++) {
            if (cum_freq[symbol + 1] <= count) break;
        }

        // Write output
        if (symbol == EOF_SYMBOL) break;
        putc(symbol, out);

        // Narrow the range (same logic as encoder)
        high = low + (range * cum_freq[symbol] / cum_freq[0]) - 1;
        low  = low + (range * cum_freq[symbol + 1] / cum_freq[0]);

        // Renormalization Loop (Identical to Encoder!)
        for (;;) {
            if (high < HALF) {
                // E1: Nothing to do for low/high adjustment beyond shifting
            } 
            else if (low >= HALF) {
                // E2: Subtract offset
                value -= HALF;
                low -= HALF;
                high -= HALF;
            } 
            else if (low >= ONE_QUARTER && high < THREE_QUARTERS) {
                // E3: Subtract offset
                value -= ONE_QUARTER;
                low -= ONE_QUARTER;
                high -= ONE_QUARTER;
            } 
            else {
                break;
            }
            
            // Shift out old bits, shift in new bit from stream
            low = 2 * low;
            high = 2 * high + 1;
            value = 2 * value + input_bit(in);
        }
        
        update_model(symbol);
    }
    printf("Decoding Complete.\n");
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <e/d> <input file> <output file>\n", argv[0]);
        return 1;
    }

    FILE *fin = fopen(argv[2], "rb");
    FILE *fout = fopen(argv[3], "wb");

    if (!fin || !fout) {
        printf("Error opening files.\n");
        return 1;
    }

    if (argv[1][0] == 'e') {
        encode(fin, fout);
    } else if (argv[1][0] == 'd') {
        decode(fin, fout);
    } else {
        printf("Invalid mode. Use 'e' for encode or 'd' for decode.\n");
    }

    fclose(fin);
    fclose(fout);
    return 0;
}
