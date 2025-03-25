#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Configuration
#define KNOWN_FLOWS_SIZE 1000
#define NUM_PACKETS      1000000
#define IP_RANGE         20000

int main() {
    // Use a fixed seed for reproducibility
    srand(12345);

    // Open the output file
    FILE *fp = fopen("dataset.txt", "w");
    if (!fp) {
        perror("Failed to open dataset.txt for writing");
        return 1;
    }

    // Write basic info
    fprintf(fp, "%d %d %d\n", KNOWN_FLOWS_SIZE, NUM_PACKETS, IP_RANGE);

    // 1) Generate known flows
    int *known_flows = (int *)malloc(KNOWN_FLOWS_SIZE * sizeof(int));
    for (int i = 0; i < KNOWN_FLOWS_SIZE; i++) {
        known_flows[i] = rand() % IP_RANGE;
    }

    // Write known flows to file
    for (int i = 0; i < KNOWN_FLOWS_SIZE; i++) {
        fprintf(fp, "%d\n", known_flows[i]);
    }

    // 2) Generate packets
    int *packets = (int *)malloc(NUM_PACKETS * sizeof(int));
    for (int i = 0; i < NUM_PACKETS; i++) {
        packets[i] = rand() % IP_RANGE;
    }

    // Write packets to file
    for (int i = 0; i < NUM_PACKETS; i++) {
        fprintf(fp, "%d\n", packets[i]);
    }

    fclose(fp);
    free(known_flows);
    free(packets);

    printf("Dataset generated successfully in dataset.txt\n");
    return 0;
}

