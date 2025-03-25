#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Configuration for certain parameters
#define NUM_PACKETS 1000000 // Total number of packets that's going to be simulated
#define KNOWN_FLOW_SIZE 1000 // Total known number of flows in table
#define IP_RANGE 20000 // IP address range from (0 - IP_RANGE - 1 in this case 0 - 19999)


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
    for(int i = 0; i < KNOWN_FLOW_SIZE; i++) {
        if(known_flows[i] == ip) {
            return 1;
        }
    }
    return 0;
}

int main() {
    srand((unsigned) time(NULL));

    // For test generate random table of known flows
    int *known_flows = (int *)malloc(KNOWN_FLOW_SIZE * sizeof(int));
    for(int i = 0; i < KNOWN_FLOW_SIZE; i++) {
        known_flows[i] = rand() % IP_RANGE;
    }

    // Similary we will generate random packets 
    int *packets = (int *)malloc(NUM_PACKETS*sizeof(int));
    for(int i = 0; i < NUM_PACKETS; i++) {
        packets[i] = rand() % IP_RANGE;
    }


    long long slow_path_count = 0;
    clock_t start = clock();


    // Process Packets 
    for(int i = 0; i < NUM_PACKETS; i++) {
        int ip = packets[i];
        if(is_known_flow(ip, known_flows)) {
            fast_path_action(ip);
        } else {
            deep_inspection(ip);
            slow_path_count++;
        }
    }

    clock_t end = clock();
    double total_time = (double)(end - start) / CLOCLS_PER_SEC;

    printf("=== Traditional Approach ===\n");
    printf("Total packets processed: %d\n", NUM_PACKETS);
    printf("Known flows size: %d\n", KNOWN_FLOWS_SIZE);
    printf("Slow path triggered: %lld times\n", slow_path_count);
    printf("Total time taken: %.3f seconds\n", total_time);

    free(known_flows);
    free(packets);

    return 0;
}
