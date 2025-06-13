#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Dataset types for testing
typedef enum {
  UNIFORM_RANDOM = 0,       // Your current dataset
  ZIPF_DISTRIBUTION = 1,    // 80/20 rule - realistic web traffic
  PARETO_DISTRIBUTION = 2,  // Heavy-tail distribution
  NORMAL_DISTRIBUTION = 3,  // Bell curve traffic
  BIMODAL_TRAFFIC = 4,      // Peak hours + off-peak
  DDOS_SIMULATION = 5,      // Attack simulation
  IOT_SENSOR_DATA = 6,      // IoT device patterns
  VIDEO_STREAMING = 7,      // Netflix/YouTube-like
  DATACENTER_EAST_WEST = 8, // Internal datacenter traffic
  CDN_EDGE_TRAFFIC = 9,     // Content delivery patterns
  ENTERPRISE_MIXED = 10,    // Mixed enterprise workload
  GAMING_TRAFFIC = 11       // Online gaming patterns
} DatasetType;

// Traffic pattern characteristics
typedef struct {
  double elephant_ratio;    // % of large flows
  double mice_ratio;        // % of small flows
  double burst_intensity;   // How bursty the traffic is
  double temporal_locality; // How clustered flows are in time
  double spatial_locality;  // How clustered flows are in IP space
  int avg_flow_size;        // Average packets per flow
  double seasonality;       // Daily/weekly patterns
} TrafficProfile;

// Dataset generation parameters
typedef struct {
  int num_packets;
  int ip_range;
  int initial_known_size;
  DatasetType dataset_type;
  TrafficProfile profile;
  char description[256];
  char filename[64];
} DatasetConfig;

// Pre-defined realistic dataset configurations
DatasetConfig datasets[] = {
    // Dataset 0: Your current uniform random
    {1000000,
     20000,
     1000,
     UNIFORM_RANDOM,
     {0.1, 0.6, 0.2, 0.3, 0.4, 50, 0.1},
     "Uniform random distribution - baseline test",
     "tests/dataset_uniform.txt"},

    // Dataset 1: Realistic web traffic (Zipf distribution)
    {1000000,
     50000,
     2000,
     ZIPF_DISTRIBUTION,
     {0.05, 0.8, 0.4, 0.7, 0.6, 25, 0.3},
     "Web traffic - 80/20 rule, few large flows dominate",
     "tests/dataset_web.txt"},

    // Dataset 2: Enterprise datacenter traffic
    {1500000,
     30000,
     5000,
     DATACENTER_EAST_WEST,
     {0.15, 0.4, 0.6, 0.8, 0.9, 150, 0.4},
     "Datacenter east-west - high locality, large flows",
     "tests/dataset_datacenter.txt"},

    // Dataset 3: DDoS attack simulation
    {800000,
     100000,
     500,
     DDOS_SIMULATION,
     {0.02, 0.95, 0.9, 0.3, 0.1, 5, 0.1},
     "DDoS simulation - many small flows from diverse sources",
     "tests/dataset_ddos.txt"},

    // Dataset 4: Video streaming traffic (Netflix-like)
    {2000000,
     25000,
     3000,
     VIDEO_STREAMING,
     {0.3, 0.2, 0.3, 0.6, 0.5, 300, 0.7},
     "Video streaming - large sustained flows with seasonality",
     "tests/dataset_streaming.txt"},

    // Dataset 5: IoT sensor network
    {500000,
     80000,
     8000,
     IOT_SENSOR_DATA,
     {0.01, 0.9, 0.2, 0.9, 0.4, 3, 0.5},
     "IoT sensors - many tiny flows, periodic patterns",
     "tests/dataset_iot.txt"},

    // Dataset 6: Gaming traffic (eSports/MMO)
    {750000,
     15000,
     1500,
     GAMING_TRAFFIC,
     {0.08, 0.7, 0.8, 0.5, 0.7, 20, 0.6},
     "Gaming traffic - low latency, bursty, synchronized events",
     "tests/dataset_gaming.txt"},

    // Dataset 7: CDN edge traffic
    {1200000,
     40000,
     4000,
     CDN_EDGE_TRAFFIC,
     {0.2, 0.5, 0.5, 0.8, 0.6, 80, 0.8},
     "CDN edge - cached content, high temporal locality",
     "tests/dataset_cdn.txt"},

    // Dataset 8: Mixed enterprise workload
    {1100000,
     35000,
     3500,
     ENTERPRISE_MIXED,
     {0.12, 0.6, 0.4, 0.6, 0.5, 60, 0.9},
     "Enterprise mixed - business hours pattern, diverse apps",
     "tests/dataset_enterprise.txt"},

    // Dataset 9: Heavy-tail Pareto distribution
    {900000,
     45000,
     1800,
     PARETO_DISTRIBUTION,
     {0.25, 0.3, 0.7, 0.4, 0.3, 200, 0.2},
     "Pareto distribution - extreme heavy-tail, few giant flows",
     "tests/dataset_pareto.txt"}};

#define NUM_DATASETS (sizeof(datasets) / sizeof(datasets[0]))

// Random number generators for different distributions
double uniform_random() { return (double)rand() / RAND_MAX; }

double zipf_random(double alpha, int n) {
  static double *zipf_table = NULL;
  static int table_size = 0;

  if (zipf_table == NULL || table_size != n) {
    if (zipf_table)
      free(zipf_table);
    zipf_table = (double *)malloc(n * sizeof(double));
    table_size = n;

    double sum = 0.0;
    for (int i = 1; i <= n; i++) {
      sum += 1.0 / pow(i, alpha);
    }

    double cumulative = 0.0;
    for (int i = 0; i < n; i++) {
      cumulative += (1.0 / pow(i + 1, alpha)) / sum;
      zipf_table[i] = cumulative;
    }
  }

  double r = uniform_random();
  for (int i = 0; i < n; i++) {
    if (r <= zipf_table[i]) {
      return i + 1;
    }
  }
  return n;
}

double pareto_random(double alpha, double xm) {
  double u = uniform_random();
  return xm / pow(u, 1.0 / alpha);
}

double normal_random(double mu, double sigma) {
  static int has_spare = 0;
  static double spare;

  if (has_spare) {
    has_spare = 0;
    return spare * sigma + mu;
  }

  has_spare = 1;
  double u = uniform_random();
  double v = uniform_random();
  double mag = sigma * sqrt(-2.0 * log(u));
  spare = mag * cos(2.0 * M_PI * v);
  return mag * sin(2.0 * M_PI * v) + mu;
}

// Generate IP based on distribution type
int generate_ip(DatasetType type, TrafficProfile *profile, int ip_range,
                int packet_index, int total_packets) {
  int ip;

  switch (type) {
  case UNIFORM_RANDOM:
    ip = rand() % ip_range;
    break;

  case ZIPF_DISTRIBUTION:
    // 80/20 rule - few IPs get most traffic
    ip = (int)zipf_random(1.2, ip_range) - 1;
    break;

  case PARETO_DISTRIBUTION:
    // Heavy-tail distribution
    ip = (int)pareto_random(1.5, 1.0) % ip_range;
    break;

  case NORMAL_DISTRIBUTION:
    // Bell curve around middle of IP range
    ip = (int)normal_random(ip_range / 2.0, ip_range / 6.0);
    ip = (ip < 0) ? 0 : (ip >= ip_range ? ip_range - 1 : ip);
    break;

  case BIMODAL_TRAFFIC:
    // Two peaks - business hours simulation
    if (uniform_random() < 0.6) {
      ip = (int)normal_random(ip_range * 0.3, ip_range * 0.1);
    } else {
      ip = (int)normal_random(ip_range * 0.7, ip_range * 0.1);
    }
    ip = (ip < 0) ? 0 : (ip >= ip_range ? ip_range - 1 : ip);
    break;

  case DDOS_SIMULATION:
    // Many random sources attacking few targets
    if (uniform_random() < 0.05) {
      // Target IPs (being attacked)
      ip = rand() % 10;
    } else {
      // Attack sources (distributed)
      ip = rand() % ip_range;
    }
    break;

  case IOT_SENSOR_DATA:
    // Many sensors reporting to few collectors
    if (uniform_random() < 0.8) {
      // Sensor IPs (many)
      ip = 1000 + (rand() % (ip_range - 1000));
    } else {
      // Collector IPs (few)
      ip = rand() % 1000;
    }
    break;

  case VIDEO_STREAMING:
    // Few content servers serving many clients
    if (uniform_random() < profile->elephant_ratio) {
      // Content servers (few, large flows)
      ip = rand() % 100;
    } else {
      // Clients (many, receiving flows)
      ip = 100 + (rand() % (ip_range - 100));
    }
    break;

  case DATACENTER_EAST_WEST:
    // High spatial locality - nearby servers communicate
    {
      static int last_ip = -1;
      if (last_ip == -1 || uniform_random() > profile->spatial_locality) {
        last_ip = rand() % ip_range;
      }
      // Generate nearby IP with some probability
      int offset = (int)normal_random(0, ip_range * 0.02);
      ip = (last_ip + offset) % ip_range;
      ip = (ip < 0) ? ip + ip_range : ip;
      last_ip = ip;
    }
    break;

  case CDN_EDGE_TRAFFIC:
    // Temporal locality - popular content accessed repeatedly
    {
      static int popular_ips[100];
      static int popular_count = 0;
      static int last_update = 0;

      // Update popular IPs periodically
      if (packet_index - last_update > 10000) {
        for (int i = 0; i < 100; i++) {
          popular_ips[i] = rand() % ip_range;
        }
        popular_count = 100;
        last_update = packet_index;
      }

      if (uniform_random() < profile->temporal_locality) {
        // Access popular content
        ip = popular_ips[rand() % popular_count];
      } else {
        // Access random content
        ip = rand() % ip_range;
      }
    }
    break;

  case GAMING_TRAFFIC:
    // Synchronized events - players in same game session
    {
      double time_progress = (double)packet_index / total_packets;
      int session_id = (int)(time_progress * 10) % 5; // 5 game sessions

      if (uniform_random() < 0.8) {
        // Players in current active session
        ip = session_id * 1000 + (rand() % 1000);
      } else {
        // Random players
        ip = rand() % ip_range;
      }
    }
    break;

  case ENTERPRISE_MIXED:
    // Business hours pattern with seasonality
    {
      double time_progress = (double)packet_index / total_packets;
      double business_hour_factor =
          0.5 + 0.5 * sin(time_progress * 2 * M_PI * profile->seasonality);

      if (uniform_random() < business_hour_factor) {
        // Business applications (clustered IPs)
        ip = (int)normal_random(ip_range * 0.3, ip_range * 0.1);
      } else {
        // Background traffic
        ip = rand() % ip_range;
      }
      ip = (ip < 0) ? 0 : (ip >= ip_range ? ip_range - 1 : ip);
    }
    break;

  default:
    ip = rand() % ip_range;
    break;
  }

  return ip;
}

// Generate flow sizes based on traffic profile
int generate_flow_size(TrafficProfile *profile, int ip) {
  double r = uniform_random();

  if (r < profile->elephant_ratio) {
    // Elephant flow - large size
    return (int)pareto_random(1.2, profile->avg_flow_size * 10);
  } else if (r < profile->elephant_ratio + profile->mice_ratio) {
    // Mice flow - small size
    return 1 + (rand() % 5);
  } else {
    // Normal flow
    return (int)normal_random(profile->avg_flow_size,
                              profile->avg_flow_size * 0.3);
  }
}

// Generate dataset file
void generate_dataset(DatasetConfig *config) {
  printf("Generating %s...\n", config->description);

  FILE *fp = fopen(config->filename, "w");
  if (!fp) {
    perror("Error creating dataset file");
    return;
  }

  // Write header
  fprintf(fp, "%d %d %d\n", config->initial_known_size, config->num_packets,
          config->ip_range);

  // Generate and write known flows
  int *known_flows = (int *)malloc(config->initial_known_size * sizeof(int));

  for (int i = 0; i < config->initial_known_size; i++) {
    known_flows[i] =
        generate_ip(config->dataset_type, &config->profile, config->ip_range, i,
                    config->initial_known_size);
    fprintf(fp, "%d\n", known_flows[i]);
  }

  // Track flow states for realistic flow generation
  typedef struct {
    int ip;
    int remaining_packets;
    int last_seen;
  } ActiveFlow;

  ActiveFlow *active_flows = (ActiveFlow *)calloc(10000, sizeof(ActiveFlow));
  int active_flow_count = 0;

  // Generate packets
  for (int i = 0; i < config->num_packets; i++) {
    int ip;

    // Check if we should continue an existing flow or start new one
    if (active_flow_count > 0 &&
        uniform_random() < config->profile.temporal_locality) {
      // Continue existing flow
      int flow_idx = rand() % active_flow_count;
      ip = active_flows[flow_idx].ip;
      active_flows[flow_idx].remaining_packets--;
      active_flows[flow_idx].last_seen = i;

      // Remove finished flow
      if (active_flows[flow_idx].remaining_packets <= 0) {
        active_flows[flow_idx] = active_flows[active_flow_count - 1];
        active_flow_count--;
      }
    } else {
      // Start new flow
      ip = generate_ip(config->dataset_type, &config->profile, config->ip_range,
                       i, config->num_packets);

      // Add to active flows if space available
      if (active_flow_count < 10000) {
        active_flows[active_flow_count].ip = ip;
        active_flows[active_flow_count].remaining_packets =
            generate_flow_size(&config->profile, ip);
        active_flows[active_flow_count].last_seen = i;
        active_flow_count++;
      }
    }

    // Add burst behavior
    if (uniform_random() < config->profile.burst_intensity * 0.001) {
      // Generate burst of same IP
      int burst_size = 5 + (rand() % 20);
      for (int b = 0; b < burst_size && i + b < config->num_packets; b++) {
        fprintf(fp, "%d\n", ip);
      }
      i += burst_size - 1; // Skip ahead
    } else {
      fprintf(fp, "%d\n", ip);
    }

    // Age out old flows
    if (i % 1000 == 0) {
      for (int j = active_flow_count - 1; j >= 0; j--) {
        if (i - active_flows[j].last_seen > 1000) {
          active_flows[j] = active_flows[active_flow_count - 1];
          active_flow_count--;
        }
      }
    }
  }

  free(known_flows);
  free(active_flows);
  fclose(fp);

  printf("Generated %s successfully!\n", config->filename);
}

// Calculate traffic concentration (what % of traffic is from top 10% of IPs)
double calculate_concentration(int *ip_counts, int ip_range,
                               int total_packets) {
  // Sort IP counts
  int *sorted_counts = (int *)malloc(ip_range * sizeof(int));
  memcpy(sorted_counts, ip_counts, ip_range * sizeof(int));

  // Simple bubble sort (good enough for analysis)
  for (int i = 0; i < ip_range - 1; i++) {
    for (int j = 0; j < ip_range - i - 1; j++) {
      if (sorted_counts[j] < sorted_counts[j + 1]) {
        int temp = sorted_counts[j];
        sorted_counts[j] = sorted_counts[j + 1];
        sorted_counts[j + 1] = temp;
      }
    }
  }

  // Calculate top 10% traffic
  int top_10_percent = ip_range / 10;
  int top_traffic = 0;
  for (int i = 0; i < top_10_percent; i++) {
    top_traffic += sorted_counts[i];
  }

  free(sorted_counts);
  return 100.0 * top_traffic / total_packets;
}

// Dataset analysis function
void analyze_dataset(const char *filename) {
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    printf("Cannot analyze %s - file not found\n", filename);
    return;
  }

  int initial_known, num_packets, ip_range;
  fscanf(fp, "%d %d %d", &initial_known, &num_packets, &ip_range);

  // Skip known flows
  for (int i = 0; i < initial_known; i++) {
    int tmp;
    fscanf(fp, "%d", &tmp);
  }

  // Analyze packet distribution
  int *ip_counts = (int *)calloc(ip_range, sizeof(int));
  int total_flows = 0;

  int prev_ip = -1;
  int current_flow_size = 0;

  for (int i = 0; i < num_packets; i++) {
    int ip;
    fscanf(fp, "%d", &ip);
    ip_counts[ip]++;

    if (ip == prev_ip) {
      current_flow_size++;
    } else {
      if (prev_ip != -1) {
        total_flows++;
      }
      current_flow_size = 1;
      prev_ip = ip;
    }
  }

  // Calculate statistics
  int unique_ips = 0;
  int max_count = 0;
  double mean_count = 0;

  for (int i = 0; i < ip_range; i++) {
    if (ip_counts[i] > 0) {
      unique_ips++;
      mean_count += ip_counts[i];
      if (ip_counts[i] > max_count) {
        max_count = ip_counts[i];
      }
    }
  }
  mean_count /= unique_ips;

  // Calculate entropy
  double entropy = 0.0;
  for (int i = 0; i < ip_range; i++) {
    if (ip_counts[i] > 0) {
      double p = (double)ip_counts[i] / num_packets;
      entropy -= p * log2(p);
    }
  }

  printf("\n📊 Dataset Analysis for %s:\n", filename);
  printf("  Total packets: %d\n", num_packets);
  printf("  Unique IPs: %d / %d (%.1f%%)\n", unique_ips, ip_range,
         100.0 * unique_ips / ip_range);
  printf("  Max packets per IP: %d\n", max_count);
  printf("  Mean packets per IP: %.1f\n", mean_count);
  printf("  Shannon entropy: %.3f bits\n", entropy);
  printf("  Traffic concentration: %.3f%% (top 10%% IPs)\n",
         calculate_concentration(ip_counts, ip_range, num_packets));

  free(ip_counts);
  fclose(fp);
}

// Test runner for multiple datasets
void run_dataset_tests() {
  printf("🧪 === MULTI-DATASET TESTING FRAMEWORK === 🧪\n\n");

  // Generate all datasets
  printf("📁 Generating realistic network traffic datasets...\n\n");
  for (int i = 0; i < NUM_DATASETS; i++) {
    generate_dataset(&datasets[i]);
  }

  printf("\n📊 Analyzing generated datasets...\n");
  for (int i = 0; i < NUM_DATASETS; i++) {
    analyze_dataset(datasets[i].filename);
  }

  printf(
      "\n🚀 Ready to test your flow processor on diverse traffic patterns!\n");
  printf("\nTo test each dataset, run:\n");
  for (int i = 0; i < NUM_DATASETS; i++) {
    printf("  cp %s dataset.txt && ./hybrid_accelerated\n",
           datasets[i].filename);
  }

  printf("\nDataset Characteristics Summary:\n");
  printf(
      "┌─────────────────────────────────────────────────────────────────┐\n");
  printf(
      "│ Dataset Type        │ Elephant │ Mice   │ Burst │ Locality     │\n");
  printf(
      "├─────────────────────────────────────────────────────────────────┤\n");
  for (int i = 0; i < NUM_DATASETS; i++) {
    char type_name[20];
    snprintf(type_name, sizeof(type_name), "Dataset %d", i);
    printf("│ %-18s │ %6.1f%% │ %5.1f%% │ %5.1f │ %5.1f/%5.1f │\n", type_name,
           datasets[i].profile.elephant_ratio * 100,
           datasets[i].profile.mice_ratio * 100,
           datasets[i].profile.burst_intensity,
           datasets[i].profile.temporal_locality,
           datasets[i].profile.spatial_locality);
  }
  printf(
      "└─────────────────────────────────────────────────────────────────┘\n");
}

int main() {
  srand(time(NULL));
  run_dataset_tests();
  return 0;
}
