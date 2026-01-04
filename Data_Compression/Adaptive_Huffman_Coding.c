/*
 * Adaptive Huffman Coding (FGK Algorithm)
 * Based on "Introduction to Data Compression" by Khalid Sayood
 * Chapter 3, Section 3.4
 *
 * This program implements:
 * 1. Dynamic Huffman Tree construction.
 * 2. The Sibling Property maintenance via node swapping.
 * 3. NYT (Not Yet Transmitted) handling for new symbols.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 256
#define MAX_NODES 512  // 2 * ALPHABET_SIZE - 1 + NYT

// Tree Node Structure
typedef struct Node {
    int weight;
    int symbol;       // -1 for internal nodes, -2 for NYT
    int id;           // Node number (for sibling property ordering)
    struct Node *parent;
    struct Node *left;
    struct Node *right;
} Node;

Node *nodes[MAX_NODES]; // Pointers to all nodes indexed by ID
Node *leaf_table[ALPHABET_SIZE]; // Quick access to leaves by symbol
Node *nyt_node;       // Pointer to current NYT node
Node *root;           // Pointer to root
int next_id;          // Next available ID (decrements from MAX_NODES)

// --- Helper Functions ---

// Initialize the tree with just the NYT node
void init_tree() {
    next_id = MAX_NODES - 1;
    
    nyt_node = (Node *)malloc(sizeof(Node));
    nyt_node->weight = 0;
    nyt_node->symbol = -2; // NYT
    nyt_node->id = next_id;
    nyt_node->parent = NULL;
    nyt_node->left = NULL;
    nyt_node->right = NULL;
    
    root = nyt_node;
    nodes[next_id] = nyt_node;
    
    for(int i=0; i<ALPHABET_SIZE; i++) leaf_table[i] = NULL;
}

// Find the highest numbered node in the block (same weight)
// This is critical for the "Update Procedure" in Section 3.4.1
Node* find_highest_in_block(int weight, Node* exclude) {
    // Search backwards from max ID because higher IDs have higher/equal weights
    // in a valid sibling-property tree.
    for (int i = MAX_NODES - 1; i >= 0; i--) {
        if (nodes[i] && nodes[i]->weight == weight && nodes[i] != exclude && nodes[i] != exclude->parent) {
            return nodes[i];
        }
    }
    return exclude; // No swap needed
}

void swap_nodes(Node *a, Node *b) {
    if (a == b) return;

    // Swap positions in the tree structure
    Node *parent_a = a->parent;
    Node *parent_b = b->parent;

    // Determine if a and b are left or right children
    int a_is_left = (parent_a->left == a);
    int b_is_left = (parent_b->left == b);

    if (a_is_left) parent_a->left = b; else parent_a->right = b;
    if (b_is_left) parent_b->left = a; else parent_b->right = a;

    b->parent = parent_a;
    a->parent = parent_b;

    // Swap IDs to maintain array indexing implies order
    // But in C implementation, we just swap the pointers in the 'nodes' array
    // and the 'id' fields to reflect their new logical position.
    int temp_id = a->id;
    a->id = b->id;
    b->id = temp_id;

    nodes[a->id] = a;
    nodes[b->id] = b;
}

// The core "Update Procedure" (Section 3.4.1)
void update_tree(Node *q) {
    while (q != NULL) {
        // Find node with highest ID in the same weight block
        Node *leader = find_highest_in_block(q->weight, q);
        
        if (leader != q && leader->parent != q) {
            swap_nodes(q, leader);
        }
        
        q->weight++;
        q = q->parent;
    }
}

// Add a new symbol to the tree (spawning from NYT)
void spawn_nyt(int symbol) {
    Node *old_nyt = nyt_node;
    
    // Create new internal node
    Node *new_internal = (Node *)malloc(sizeof(Node));
    new_internal->weight = 1; // Will be 0 initially, incremented by update
    new_internal->symbol = -1;
    new_internal->id = old_nyt->id; // Takes old NYT's ID
    new_internal->parent = old_nyt->parent;
    new_internal->left = old_nyt; // Old NYT becomes left child
    
    // Create new leaf node for symbol
    Node *new_leaf = (Node *)malloc(sizeof(Node));
    new_leaf->weight = 1; // Will be incremented
    new_leaf->symbol = symbol;
    new_leaf->id = old_nyt->id - 1;
    new_leaf->parent = new_internal;
    new_leaf->left = NULL;
    new_leaf->right = NULL;
    
    // Reset Old NYT
    old_nyt->weight = 0;
    old_nyt->id = old_nyt->id - 2;
    old_nyt->parent = new_internal;
    
    // Link new internal node to parent
    if (new_internal->parent) {
        if (new_internal->parent->left == old_nyt) 
            new_internal->parent->left = new_internal;
        else 
            new_internal->parent->right = new_internal;
    } else {
        root = new_internal;
    }
    
    new_internal->right = new_leaf; // Right child is the new symbol
    
    // Update Tables
    nodes[new_internal->id] = new_internal;
    nodes[new_leaf->id] = new_leaf;
    nodes[old_nyt->id] = old_nyt;
    leaf_table[symbol] = new_leaf;
    
    next_id -= 2;
    
    // Increment weights up to root (Start from parent of new leaf)
    update_tree(new_internal->parent);
}

// --- Output Functions ---

void print_code(Node *n) {
    if (n == NULL || n->parent == NULL) return;
    print_code(n->parent);
    if (n->parent->left == n) printf("0");
    else printf("1");
}

void adaptive_encode(char *input) {
    init_tree();
    
    printf("Input: %s\n", input);
    printf("Encoding Stream:\n");
    
    for (int i = 0; input[i] != '\0'; i++) {
        int symbol = (unsigned char)input[i];
        
        if (leaf_table[symbol] == NULL) {
            // 1. Transmit NYT Code
            print_code(nyt_node);
            
            // 2. Transmit fixed code for symbol (8-bit ASCII for simplicity)
            // In Sayood's book, this is e+1 bits, but we use standard char here.
            printf(" [NYT '%c'] ", symbol);
            
            // 3. Spawn new node and update
            spawn_nyt(symbol);
        } else {
            // 1. Transmit Symbol Code
            print_code(leaf_table[symbol]);
            printf(" ");
            
            // 2. Update Tree (Section 3.4.1)
            update_tree(leaf_table[symbol]);
        }
    }
    printf("\n");
}

int main() {
    // Example from the book usually involves "aardvark" to show repetition effects
    char *data = "aardvark";
    adaptive_encode(data);
    return 0;
}
