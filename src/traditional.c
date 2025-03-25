#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Configuration for certain parameters
static int NUM_PACKETS; // Total number of packets that's going to be simulated
static int KNOWN_FLOWS_SIZE; // Total known number of flows in table
static int IP_RANGE; // IP address range from (0 - IP_RANGE - 1 in this case 0 - 19999)


// Simulated "expensive" operation (i.e the slow path)
void deep_inspection(int ip) {
    int count = 0;
    for(int i = 1; i <= ip; i++) {
        if(ip % i == 0) {
            count++;
        }
    }
}


// Simulated "fast" operation 
void fast_path_action(int ip) {
    volatile int result = ip * 2;
    (void)result;
}

// Check if ip is in the known flows table (linear search)

int is_known_flow(int ip, int* known_flows) {
    for(int i = 0; i < KNOWN_FLOWS_SIZE; i++) {
        if(known_flows[i] == ip) {
            return 1;
        }
    }
    return 0;
}

int main() {
      // Read from dataset.txt
    FILE *fp = fopen("dataset.txt", "r");
    if (!fp) {
        perror("Error opening dataset.txt");
        return 1;
    }

    // 1) Read basic info
    if (fscanf(fp, "%d %d %d", &KNOWN_FLOWS_SIZE, &NUM_PACKETS, &IP_RANGE) != 3) {
        fprintf(stderr, "Error reading dataset header\n");
        fclose(fp);
        return 1;
    }

    // 2) Read known flows
    int *known_flows = (int *)malloc(KNOWN_FLOWS_SIZE * sizeof(int));
    for (int i = 0; i < KNOWN_FLOWS_SIZE; i++) {
        if (fscanf(fp, "%d", &known_flows[i]) != 1) {
            fprintf(stderr, "Error reading known_flows\n");
            fclose(fp);
            return 1;
        }
    }

    // 3) Read packets
    int *packets = (int *)malloc(NUM_PACKETS * sizeof(int));
    for (int i = 0; i < NUM_PACKETS; i++) {
        if (fscanf(fp, "%d", &packets[i]) != 1) {
            fprintf(stderr, "Error reading packets\n");
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);

    // Metrics
    long long slow_path_count = 0;
    clock_t start = clock();

    // Process packets
    for (int i = 0; i < NUM_PACKETS; i++) {
        int ip = packets[i];
        if (is_known_flow(ip, known_flows)) {
            fast_path_action(ip);
        } else {
            deep_inspection(ip);
            slow_path_count++;
        }
    }

    clock_t end = clock();
    double total_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("=== Traditional Approach ===\n");
    printf("Dataset: KNOWN_FLOWS_SIZE=%d, NUM_PACKETS=%d, IP_RANGE=%d\n",
           KNOWN_FLOWS_SIZE, NUM_PACKETS, IP_RANGE);
    printf("Slow path triggered: %lld times\n", slow_path_count);
    printf("Total time taken: %.3f seconds\n", total_time);

    free(known_flows);
    free(packets);
    return 0;  
}
