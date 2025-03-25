#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// We'll read these from dataset.txt
static int INITIAL_KNOWN_SIZE;  // same as KNOWN_FLOWS_SIZE from dataset
static int NUM_PACKETS;
static int IP_RANGE;

// Simulated slow path
void deep_inspection(int ip) {
    int count = 0;
    for (int i = 1; i <= ip; i++) {
        if (ip % i == 0) count++;
    }
}

// Fast path
void fast_path_action(int ip) {
    volatile int result = ip * 2;
    (void)result;
}

// Linear search in known flows
int is_known_flow(int ip, int *known_flows, int known_count) {
    for (int i = 0; i < known_count; i++) {
        if (known_flows[i] == ip) {
            return 1;
        }
    }
    return 0;
}

// Add new flow (resizing array if needed)
void add_known_flow(int ip, int **known_flows_ptr, int *known_count, int *capacity) {
    // Already known? Skip
    if (is_known_flow(ip, *known_flows_ptr, *known_count)) return;

    // Resize if needed
    if (*known_count >= *capacity) {
        *capacity *= 2;
        *known_flows_ptr = (int *)realloc(*known_flows_ptr, (*capacity) * sizeof(int));
    }

    // Add new flow
    (*known_flows_ptr)[(*known_count)++] = ip;
}

int main() {
    // Read from dataset.txt
    FILE *fp = fopen("dataset.txt", "r");
    if (!fp) {
        perror("Error opening dataset.txt");
        return 1;
    }

    // Read basic info
    if (fscanf(fp, "%d %d %d", &INITIAL_KNOWN_SIZE, &NUM_PACKETS, &IP_RANGE) != 3) {
        fprintf(stderr, "Error reading dataset header\n");
        fclose(fp);
        return 1;
    }

    // Known flows capacity
    int capacity = INITIAL_KNOWN_SIZE * 2;
    int known_count = INITIAL_KNOWN_SIZE;
    int *known_flows = (int *)malloc(capacity * sizeof(int));

    // Read known flows
    for (int i = 0; i < known_count; i++) {
        if (fscanf(fp, "%d", &known_flows[i]) != 1) {
            fprintf(stderr, "Error reading known_flows\n");
            fclose(fp);
            return 1;
        }
    }

    // Read packets
    int *packets = (int *)malloc(NUM_PACKETS * sizeof(int));
    for (int i = 0; i < NUM_PACKETS; i++) {
        if (fscanf(fp, "%d", &packets[i]) != 1) {
            fprintf(stderr, "Error reading packets\n");
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);

    // Process packets with immediate learning
    long long slow_path_count = 0;
    clock_t start = clock();

    for (int i = 0; i < NUM_PACKETS; i++) {
        int ip = packets[i];
        if (is_known_flow(ip, known_flows, known_count)) {
            fast_path_action(ip);
        } else {
            deep_inspection(ip);
            slow_path_count++;
            // Immediately add to known flows so subsequent packets with same IP are fast
            add_known_flow(ip, &known_flows, &known_count, &capacity);
        }
    }

    clock_t end = clock();
    double total_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("=== Hybrid Immediate Learning ===\n");
    printf("Dataset: INITIAL_KNOWN_SIZE=%d, NUM_PACKETS=%d, IP_RANGE=%d\n",
           INITIAL_KNOWN_SIZE, NUM_PACKETS, IP_RANGE);
    printf("Final known flows: %d\n", known_count);
    printf("Slow path triggered: %lld times\n", slow_path_count);
    printf("Total time taken: %.3f seconds\n", total_time);

    free(known_flows);
    free(packets);
    return 0;
}

