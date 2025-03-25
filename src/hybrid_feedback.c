#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Configuration
#define NUM_PACKETS 1000000
#define INITIAL_KNOWN_SIZE 1000 
#define IP_RANGE 20000
#define FEEDBACK_INTERVAL 50000 // Packets processed per feedback window 
#define SLOW_PATH_THRESHOLD 0.05 // Slow path trigger threshold in a window 

// Simulated "expensive" operation (slowpath)

void deep_inspection(int ip) {
    int count = 0;
    for (int i = 1; i <= ip; i++) {
        if (ip % i == 0)
            count++;
    }
}

// Simulated "fast" operation
void fast_path_action(int ip) {
    volatile int result = ip * 2;
    (void)result;
}

// Linear search to check if IP is in known flows
int is_known_flow(int ip, int *known_flows, int known_count) {
    for (int i = 0; i < known_count; i++) {
        if (known_flows[i] == ip) {
            return 1;
        }
    }
    return 0;
}

// Simple (can be modified) Add a new IP to the known flows table (if not already present)

void add_known_flow(int ip, int** known_flows_ptr, int *known_count, int *capacity) {
    if(is_known_flow(ip, *known_flows_ptr, *known_count)) {
        return;
    }


    // Resize the array if needed 
    if(*known_count >= *capacity) {
        *capacity *= 2;
        *known_flows_ptr = (int*)realloc(*known_flows_ptr, (*capacity)*sizeof(int));
    }
    (*known_flows_ptr)[(*known_count)++] = ip; 
}

int main() {
    srand((unsigned) time(NULL));

    // Initialize known flows with extra capacity for feedback
    int capacity = INITIAL_KNOWN_SIZE * 2;
    int known_count = INITIAL_KNOWN_SIZE;
    int *known_flows = (int *)malloc(capacity * sizeof(int));

    for (int i = 0; i < known_count; i++) {
        known_flows[i] = rand() % IP_RANGE;
    }

    // Generate random packets
    int *packets = (int *)malloc(NUM_PACKETS * sizeof(int));
    for (int i = 0; i < NUM_PACKETS; i++) {
        packets[i] = rand() % IP_RANGE;
    }

    long long slow_path_count = 0;
    clock_t start = clock();
    long long window_slow_count = 0;  // Slow path count for current window

    // Process packets with feedback loop
    for (int i = 0; i < NUM_PACKETS; i++) {
        int ip = packets[i];
        if (is_known_flow(ip, known_flows, known_count)) {
            fast_path_action(ip);
        } else {
            deep_inspection(ip);
            slow_path_count++;
            window_slow_count++;
        }

        // Feedback check every FEEDBACK_INTERVAL packets
        if ((i + 1) % FEEDBACK_INTERVAL == 0) {
            double slow_ratio = (double)window_slow_count / FEEDBACK_INTERVAL;
            if (slow_ratio > SLOW_PATH_THRESHOLD) {
                int window_start = i + 1 - FEEDBACK_INTERVAL;
                for (int j = window_start; j <= i; j++) {
                    int wip = packets[j];
                    if (!is_known_flow(wip, known_flows, known_count)) {
                        add_known_flow(wip, &known_flows, &known_count, &capacity);
                    }
                }
            }
            window_slow_count = 0; // Reset for next interval
        }
    }

    clock_t end = clock();
    double total_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("=== Proposed Hybrid with Feedback ===\n");
    printf("Total packets processed: %d\n", NUM_PACKETS);
    printf("Initial known flows: %d\n", INITIAL_KNOWN_SIZE);
    printf("Final known flows: %d\n", known_count);
    printf("Slow path triggered: %lld times\n", slow_path_count);
    printf("Total time taken: %.3f seconds\n", total_time);

    free(known_flows);
    free(packets);
    return 0;
}
