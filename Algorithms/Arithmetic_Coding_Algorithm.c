// Arithmetic Coding Algorithm
// gcc Arithmetic_Coding_Algorithm.c -o arith



int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("\n=====================================\n");
        printf(" Arithmetic Coding Tool (C Version)\n");
        printf("=====================================\n");
        printf("No arguments detected.\n");
        printf("\nChoose an option:\n");
        printf("  1) Encode a file\n");
        printf("  2) Decode a file\n");
        printf("  0) Exit\n");
        printf("-------------------------------------\n");
        printf("Enter choice: ");

        int choice;
        scanf("%d", &choice);

        if (choice == 0) {
            printf("Exiting.\n");
            return 0;
        }

        char in[256], out[256];

        if (choice == 1) {
            printf("Enter input file to ENCODE: ");
            scanf("%s", in);

            printf("Enter output file name: ");
            scanf("%s", out);

            arithmetic_encode(in, out);
            printf("Encoding completed.\n");
        }
        else if (choice == 2) {
            printf("Enter input file to DECODE: ");
            scanf("%s", in);

            printf("Enter output file name: ");
            scanf("%s", out);

            arithmetic_decode(in, out);
            printf("Decoding completed.\n");
        }
        else {
            printf("Invalid option.\n");
        }

        return 0;
    }

    // ------------------------------
    // Command-line mode (old behavior)
    // ------------------------------

    if (argc != 4) {
        printf("Usage:\n");
        printf("  %s encode input.txt output.bin\n", argv[0]);
        printf("  %s decode input.bin output.txt\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "encode") == 0) {
        arithmetic_encode(argv[2], argv[3]);
        printf("Encoded.\n");
    }
    else if (strcmp(argv[1], "decode") == 0) {
        arithmetic_decode(argv[2], argv[3]);
        printf("Decoded.\n");
    }
    else {
        printf("Unknown command.\n");
    }

    return 0;
}
