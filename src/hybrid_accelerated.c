#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Optimized Configuration
static int INITIAL_KNOWN_SIZE;
static int NUM_PACKETS;
static int IP_RANGE;

#define LARGE_FLOW_AREA_SIZE 50000
#define BURSTY_FLOW_AREA_SIZE 500
#define MICRO_FLOW_AREA_SIZE 1000
#define HASH_TABLE_SIZE 65536 // Power of 2 for fast modulo
#define CACHE_SIZE 8192       // Larger, power of 2 cache
#define BURST_THRESHOLD 100   // More reasonable burst threshold
#define CONFIDENCE_FAST_TRACK 60
#define CONFIDENCE_ULTRA_FAST 85
#define AGING_INTERVAL 25000 // More frequent aging for ML
#define SKETCH_WIDTH 4096    // Optimized sketch size
#define SKETCH_DEPTH 3       // Reduced depth for speed

// Enhanced ML Configuration
#define ML_FEATURE_COUNT 8
#define ML_HISTORY_SIZE 8            // Reduced for better performance
#define ML_ADAPTATION_INTERVAL 50000 // Less frequent adaptation
#define AGING_BUCKETS 4
#define PREDICTION_CACHE_SIZE 1024 // Larger prediction cache
#define BURST_WINDOW_SIZE 100      // More reasonable window

// Processing paths
typedef enum {
  FAST_PATH = 0,
  ACCELERATED_PATH = 1,
  ULTRA_FAST_PATH = 2,
  SLOW_PATH = 3,
  ADAPTIVE_PATH = 4,
  DEEP_ANALYSIS_PATH = 5
} ProcessingPath;

// Enhanced flow types
typedef enum {
  NORMAL_FLOW = 0,
  LARGE_FLOW = 1,
  BURSTY_FLOW = 2,
  MICRO_FLOW = 3,
  DYING_FLOW = 4,
  PROMOTED_FLOW = 5,
  SUSPECTED_FLOW = 6
} FlowType;

// Aging strategies
typedef enum {
  AGING_LINEAR = 0,
  AGING_EXPONENTIAL = 1,
  AGING_ADAPTIVE = 2,
  AGING_AGGRESSIVE = 3
} AgingStrategy;

// Enhanced ML model with better accuracy tracking
typedef struct {
  double weights[ML_FEATURE_COUNT];
  double bias;
  double learning_rate;
  uint64_t predictions_made;
  uint64_t correct_predictions;
  double accuracy;

  // Feature normalization - improved
  double feature_mins[ML_FEATURE_COUNT];
  double feature_maxs[ML_FEATURE_COUNT];

  // Adaptation tracking
  uint64_t last_adaptation;
  double previous_accuracy;

  // Performance validation
  uint32_t validation_samples;
  uint32_t validation_correct;
} MLModel;

// Simplified flow pattern tracking
typedef struct {
  uint8_t path_history[ML_HISTORY_SIZE];
  uint8_t history_index;
  uint8_t history_filled;

  // Derived patterns
  double path_consistency;
  double burst_score;
  uint32_t consecutive_fast_paths;
  uint32_t recent_promotions;
} FlowPattern;

// Enhanced aging metadata
typedef struct {
  time_t creation_time;
  time_t last_access_time;
  uint32_t idle_periods;
  uint32_t total_accesses;
  AgingStrategy aging_strategy;
  double aging_multiplier;
} AgingInfo;

// Optimized flow entry
typedef struct FlowEntry {
  uint32_t ip;
  uint16_t confidence;
  uint16_t hits;
  uint32_t packet_count;
  time_t last_seen;
  FlowType flow_type;
  FlowType previous_type;

  FlowPattern pattern;
  AgingInfo aging;

  // Performance tracking
  uint32_t cache_hits;
  uint16_t promotion_score; // 0-1000 scale

  struct FlowEntry *next;
} FlowEntry;

typedef struct {
  FlowEntry *buckets[HASH_TABLE_SIZE];
  int total_entries;
  uint64_t total_lookups;
  uint64_t collision_count;
} HashTable;

typedef struct {
  uint32_t counters[SKETCH_DEPTH][SKETCH_WIDTH];
  uint32_t seeds[SKETCH_DEPTH];
} FastSketch;

// Improved prediction cache
typedef struct {
  uint32_t ip;
  double prediction;
  ProcessingPath suggested_path;
  time_t timestamp;
  uint8_t confidence_level; // 0-255
} PredictionCache;

// Simplified aging manager
typedef struct {
  time_t last_aging_cycle;
  uint64_t flows_aged_out;
  uint64_t flows_demoted;
  uint64_t flows_promoted;
  double aging_pressure;
  double memory_utilization;

  // Burst tracking - fixed
  uint32_t burst_history[BURST_WINDOW_SIZE];
  int burst_index;
  uint32_t total_bursts;
  double current_burst_rate;
} AgingManager;

// Main table structure
typedef struct {
  HashTable *hash_table;
  FlowEntry *flow_pool;
  int pool_index;
  int pool_size;

  FlowEntry *fast_cache[CACHE_SIZE];
  FastSketch *sketch;

  MLModel *ml_model;
  PredictionCache prediction_cache[PREDICTION_CACHE_SIZE];
  int prediction_cache_index;

  AgingManager *aging_manager;

  // Statistics
  uint64_t total_processed;
  uint64_t cache_hits;
  uint64_t cache_misses;
  uint64_t path_counts[6];
  uint64_t ml_predictions;
  uint64_t ml_cache_hits;

  // Performance counters
  uint64_t ultra_fast_promotions;
  uint64_t confidence_updates;
  uint64_t pattern_updates;
} OptimizedTable;

OptimizedTable *g_table;

// Fast hash function
static inline uint32_t fast_hash(uint32_t key) {
  key ^= key >> 16;
  key *= 0x85ebca6b;
  key ^= key >> 13;
  key *= 0xc2b2ae35;
  key ^= key >> 16;
  return key;
}

// Initialize ML model with better defaults
MLModel *init_ml_model() {
  MLModel *model = (MLModel *)calloc(1, sizeof(MLModel));

  // Balanced weights for better performance
  model->weights[0] = 0.35; // confidence weight (increased)
  model->weights[1] = 0.20; // hits weight
  model->weights[2] = 0.15; // packet count weight
  model->weights[3] = 0.10; // recency weight
  model->weights[4] = 0.08; // pattern consistency
  model->weights[5] = 0.05; // burst score
  model->weights[6] = 0.04; // cache performance
  model->weights[7] = 0.03; // flow type

  model->bias = 0.2;
  model->learning_rate = 0.002;

  // Initialize normalization bounds
  for (int i = 0; i < ML_FEATURE_COUNT; i++) {
    model->feature_mins[i] = 0.0;
    model->feature_maxs[i] = 100.0;
  }
  // Adjust specific bounds
  model->feature_maxs[1] = 1000.0;  // hits can be higher
  model->feature_maxs[2] = 10000.0; // packet count can be much higher

  return model;
}

// Initialize aging manager
AgingManager *init_aging_manager() {
  AgingManager *manager = (AgingManager *)calloc(1, sizeof(AgingManager));
  manager->aging_pressure = 0.3;
  manager->memory_utilization = 0.0;
  manager->last_aging_cycle = time(NULL);
  manager->burst_index = 0;
  return manager;
}

// Initialize hash table
HashTable *init_hash_table() {
  HashTable *ht = (HashTable *)calloc(1, sizeof(HashTable));
  return ht;
}

// Fast sketch operations
FastSketch *init_fast_sketch() {
  FastSketch *sketch = (FastSketch *)calloc(1, sizeof(FastSketch));
  sketch->seeds[0] = 0x9e3779b9;
  sketch->seeds[1] = 0x85ebca6b;
  sketch->seeds[2] = 0xc2b2ae35;
  return sketch;
}

static inline void sketch_update_fast(FastSketch *sketch, uint32_t ip) {
  for (int i = 0; i < SKETCH_DEPTH; i++) {
    uint32_t pos = fast_hash(ip ^ sketch->seeds[i]) & (SKETCH_WIDTH - 1);
    sketch->counters[i][pos]++;
  }
}

static inline uint32_t sketch_query_fast(FastSketch *sketch, uint32_t ip) {
  uint32_t min_count = UINT32_MAX;
  for (int i = 0; i < SKETCH_DEPTH; i++) {
    uint32_t pos = fast_hash(ip ^ sketch->seeds[i]) & (SKETCH_WIDTH - 1);
    if (sketch->counters[i][pos] < min_count) {
      min_count = sketch->counters[i][pos];
    }
  }
  return min_count;
}

// Improved feature extraction
static inline void extract_ml_features(FlowEntry *flow,
                                       double features[ML_FEATURE_COUNT]) {
  time_t now = time(NULL);
  double time_diff =
      (double)(now - flow->last_seen + 1); // Avoid division by zero

  features[0] = (double)flow->confidence;
  features[1] = (double)flow->hits;
  features[2] = (double)flow->packet_count;
  features[3] = 100.0 / time_diff; // Higher for more recent
  features[4] = flow->pattern.path_consistency * 100.0;
  features[5] = flow->pattern.burst_score * 100.0;
  features[6] =
      flow->hits > 0 ? (double)flow->cache_hits / flow->hits * 100.0 : 0.0;
  features[7] = (double)flow->flow_type * 10.0;
}

// Improved feature normalization
static inline void normalize_features(MLModel *model,
                                      double features[ML_FEATURE_COUNT]) {
  for (int i = 0; i < ML_FEATURE_COUNT; i++) {
    double range = model->feature_maxs[i] - model->feature_mins[i];
    if (range > 1e-6) {
      features[i] = (features[i] - model->feature_mins[i]) / range;
    } else {
      features[i] = 0.5; // Default to middle if no range
    }
    // Clamp to [0, 1]
    if (features[i] > 1.0)
      features[i] = 1.0;
    if (features[i] < 0.0)
      features[i] = 0.0;
  }
}

// Enhanced ML predictor
static inline double enhanced_ml_predict(FlowEntry *flow) {
  if (!flow)
    return 0.0;

  double features[ML_FEATURE_COUNT];
  extract_ml_features(flow, features);
  normalize_features(g_table->ml_model, features);

  // Linear combination
  double prediction = g_table->ml_model->bias;
  for (int i = 0; i < ML_FEATURE_COUNT; i++) {
    prediction += g_table->ml_model->weights[i] * features[i];
  }

  // Sigmoid activation
  prediction = 1.0 / (1.0 + exp(-prediction));

  g_table->ml_predictions++;
  return prediction;
}

// Improved prediction cache
static inline double check_prediction_cache(uint32_t ip) {
  uint32_t cache_idx = fast_hash(ip) & (PREDICTION_CACHE_SIZE - 1);
  PredictionCache *cached = &g_table->prediction_cache[cache_idx];

  time_t now = time(NULL);
  if (cached->ip == ip && (now - cached->timestamp) < 30) { // 30 second cache
    g_table->ml_cache_hits++;
    return cached->prediction;
  }
  return -1.0;
}

static inline void update_prediction_cache(uint32_t ip, double prediction,
                                           ProcessingPath path) {
  uint32_t cache_idx = fast_hash(ip) & (PREDICTION_CACHE_SIZE - 1);
  PredictionCache *entry = &g_table->prediction_cache[cache_idx];

  entry->ip = ip;
  entry->prediction = prediction;
  entry->suggested_path = path;
  entry->timestamp = time(NULL);
  entry->confidence_level = (uint8_t)(prediction * 255);
}

// Simplified pattern update
static inline void update_flow_pattern(FlowEntry *flow, ProcessingPath path) {
  FlowPattern *pattern = &flow->pattern;

  // Update path history
  pattern->path_history[pattern->history_index] = (uint8_t)path;
  pattern->history_index = (pattern->history_index + 1) % ML_HISTORY_SIZE;
  if (!pattern->history_filled && pattern->history_index == 0) {
    pattern->history_filled = 1;
  }

  // Calculate consistency - only if we have enough history
  if (pattern->history_filled || pattern->history_index >= 4) {
    int size =
        pattern->history_filled ? ML_HISTORY_SIZE : pattern->history_index;
    int consistent_count = 0;
    uint8_t mode_path = pattern->path_history[0];

    // Find most common path
    int max_count = 0;
    for (int i = 0; i < size; i++) {
      int count = 1;
      for (int j = i + 1; j < size; j++) {
        if (pattern->path_history[j] == pattern->path_history[i])
          count++;
      }
      if (count > max_count) {
        max_count = count;
        mode_path = pattern->path_history[i];
      }
    }

    pattern->path_consistency = (double)max_count / size;
  }

  // Track consecutive fast paths for promotion
  if (path <= FAST_PATH) {
    pattern->consecutive_fast_paths++;
  } else {
    pattern->consecutive_fast_paths = 0;
  }

  // Calculate burst score based on path variability
  if (pattern->history_filled) {
    int transitions = 0;
    for (int i = 1; i < ML_HISTORY_SIZE; i++) {
      if (pattern->path_history[i] != pattern->path_history[i - 1]) {
        transitions++;
      }
    }
    pattern->burst_score = (double)transitions / (ML_HISTORY_SIZE - 1);
  }

  g_table->pattern_updates++;
}

// Better ML model adaptation
static inline void adapt_ml_model() {
  MLModel *model = g_table->ml_model;

  if (g_table->total_processed - model->last_adaptation <
      ML_ADAPTATION_INTERVAL) {
    return;
  }

  // Update accuracy using validation samples
  if (model->validation_samples > 0) {
    model->accuracy =
        (double)model->validation_correct / model->validation_samples;

    // Adjust learning rate based on performance
    if (model->accuracy > 0.85) {
      model->learning_rate *= 0.98; // Small decrease when performing well
    } else if (model->accuracy < 0.70) {
      model->learning_rate *= 1.05; // Increase when performing poorly
    }

    // Clamp learning rate
    if (model->learning_rate > 0.01)
      model->learning_rate = 0.01;
    if (model->learning_rate < 0.0005)
      model->learning_rate = 0.0005;

    // Reset validation counters
    model->validation_samples = 0;
    model->validation_correct = 0;
  }

  model->last_adaptation = g_table->total_processed;
}

// Simplified aging strategies
static inline void apply_aging_strategy(FlowEntry *flow,
                                        AgingStrategy strategy) {
  time_t now = time(NULL);
  double idle_time = (double)(now - flow->last_seen);

  switch (strategy) {
  case AGING_LINEAR:
    if (idle_time > 180) { // 3 minutes
      flow->confidence = flow->confidence > 3 ? flow->confidence - 3 : 0;
    }
    break;

  case AGING_EXPONENTIAL:
    if (idle_time > 60) {                       // 1 minute
      double decay = 1.0 - (idle_time / 600.0); // 10 minute decay
      if (decay < 0.1)
        decay = 0.1;
      flow->confidence = (uint16_t)(flow->confidence * decay);
    }
    break;

  case AGING_ADAPTIVE: {
    double ml_score = enhanced_ml_predict(flow);
    double protection = ml_score * 0.8; // Protect high-scoring flows
    double decay = (idle_time / 1200.0) * (1.0 - protection); // 20 minute base
    flow->confidence = (uint16_t)(flow->confidence * (1.0 - decay));
  } break;

  case AGING_AGGRESSIVE:
    if (idle_time > 90) { // 1.5 minutes
      flow->confidence = flow->confidence > 8 ? flow->confidence - 8 : 0;
      if (flow->confidence < 15) {
        flow->flow_type = DYING_FLOW;
      }
    }
    break;
  }
}

// Fixed burst detection
static inline int detect_burst_enhanced() {
  AgingManager *manager = g_table->aging_manager;

  // Simple burst detection based on packet rate
  static uint64_t last_packet_count = 0;
  static time_t last_check = 0;

  time_t now = time(NULL);
  if (now != last_check) {
    uint64_t packets_this_second = g_table->total_processed - last_packet_count;

    // Update burst history
    manager->burst_history[manager->burst_index] =
        (uint32_t)packets_this_second;
    manager->burst_index = (manager->burst_index + 1) % BURST_WINDOW_SIZE;

    // Calculate average rate
    uint32_t total = 0;
    for (int i = 0; i < BURST_WINDOW_SIZE; i++) {
      total += manager->burst_history[i];
    }
    manager->current_burst_rate = (double)total / BURST_WINDOW_SIZE;

    last_packet_count = g_table->total_processed;
    last_check = now;

    // Detect burst if current rate is significantly above average
    return (packets_this_second > manager->current_burst_rate * 2.0 &&
            packets_this_second > BURST_THRESHOLD);
  }

  return 0;
}

// Enhanced aging cycle
static inline void enhanced_aging_cycle() {
  AgingManager *manager = g_table->aging_manager;
  time_t now = time(NULL);

  if (now - manager->last_aging_cycle < 30) { // 30 second cycles
    return;
  }

  // Calculate memory pressure
  manager->memory_utilization =
      (double)g_table->pool_index / g_table->pool_size;

  // Adjust aging pressure
  if (manager->memory_utilization > 0.85) {
    manager->aging_pressure = 0.9;
  } else if (manager->memory_utilization > 0.70) {
    manager->aging_pressure = 0.6;
  } else {
    manager->aging_pressure = 0.3;
  }

  // Age a subset of flows
  int flows_to_age = (int)(g_table->pool_index * 0.1); // Age 10% each cycle

  for (int i = 0; i < flows_to_age && i < g_table->pool_index; i++) {
    int flow_idx = (int)(g_table->total_processed + i) % g_table->pool_index;
    FlowEntry *flow = &g_table->flow_pool[flow_idx];

    if (flow->ip != 0) { // Active flow
      apply_aging_strategy(flow, flow->aging.aging_strategy);

      // Track flow state changes
      if (flow->confidence < 10 && flow->flow_type != DYING_FLOW) {
        flow->previous_type = flow->flow_type;
        flow->flow_type = DYING_FLOW;
        manager->flows_demoted++;
      }
    }
  }

  manager->last_aging_cycle = now;
}

// Initialize optimized table
OptimizedTable *init_optimized_table() {
  OptimizedTable *table = (OptimizedTable *)calloc(1, sizeof(OptimizedTable));
  table->hash_table = init_hash_table();
  table->sketch = init_fast_sketch();
  table->ml_model = init_ml_model();
  table->aging_manager = init_aging_manager();

  table->pool_size =
      LARGE_FLOW_AREA_SIZE + BURSTY_FLOW_AREA_SIZE + MICRO_FLOW_AREA_SIZE;
  table->flow_pool = (FlowEntry *)calloc(table->pool_size, sizeof(FlowEntry));
  table->pool_index = 0;

  return table;
}

// Fast flow lookup
static inline FlowEntry *find_flow_fast(uint32_t ip) {
  uint32_t cache_idx = fast_hash(ip) & (CACHE_SIZE - 1);
  FlowEntry *cached = g_table->fast_cache[cache_idx];

  if (cached && cached->ip == ip) {
    g_table->cache_hits++;
    cached->cache_hits++;
    return cached;
  }

  g_table->hash_table->total_lookups++;
  uint32_t bucket = fast_hash(ip) & (HASH_TABLE_SIZE - 1);
  FlowEntry *entry = g_table->hash_table->buckets[bucket];

  while (entry) {
    if (entry->ip == ip) {
      g_table->fast_cache[cache_idx] = entry;
      return entry;
    }
    entry = entry->next;
    g_table->hash_table->collision_count++;
  }

  g_table->cache_misses++;
  return NULL;
}

// Enhanced flow creation
static inline FlowEntry *create_flow_fast(uint32_t ip) {
  if (g_table->pool_index >= g_table->pool_size) {
    return NULL;
  }

  FlowEntry *new_flow = &g_table->flow_pool[g_table->pool_index++];
  memset(new_flow, 0, sizeof(FlowEntry));

  new_flow->ip = ip;
  new_flow->confidence = 35; // Slightly higher starting confidence
  new_flow->hits = 1;
  new_flow->packet_count = 1;
  new_flow->last_seen = time(NULL);
  new_flow->flow_type = NORMAL_FLOW;
  new_flow->previous_type = NORMAL_FLOW;
  new_flow->promotion_score = 100; // Start with some promotion potential

  // Initialize aging
  new_flow->aging.creation_time = new_flow->last_seen;
  new_flow->aging.last_access_time = new_flow->last_seen;
  new_flow->aging.aging_strategy = AGING_EXPONENTIAL;
  new_flow->aging.aging_multiplier = 1.0;

  // Initialize pattern
  new_flow->pattern.path_consistency = 1.0;
  new_flow->pattern.burst_score = 0.0;

  // Add to hash table
  uint32_t bucket = fast_hash(ip) & (HASH_TABLE_SIZE - 1);
  new_flow->next = g_table->hash_table->buckets[bucket];
  g_table->hash_table->buckets[bucket] = new_flow;
  g_table->hash_table->total_entries++;

  return new_flow;
}

// Processing functions
static inline void ultra_fast_process(uint32_t ip) {
  volatile uint32_t r = ip;
  (void)r;
}

static inline void fast_process(uint32_t ip) {
  volatile uint32_t r = ip * 2 + 1;
  (void)r;
}

static inline void accelerated_process(uint32_t ip) {
  volatile int c = 0;
  uint32_t limit = (ip > 100) ? 10 : (uint32_t)sqrt(ip);
  for (uint32_t i = 2; i <= limit; i++) {
    if (ip % i == 0)
      c++;
  }
  (void)c;
}

static inline void slow_process(uint32_t ip) {
  volatile int c = 0;
  uint32_t limit = (uint32_t)sqrt(ip);
  for (uint32_t i = 1; i <= limit; i++) {
    if (ip % i == 0)
      c++;
  }
  (void)c;
}

// Improved burst promotion
static inline void maybe_promote_burst(FlowEntry *flow) {
  if (!flow)
    return;

  if (detect_burst_enhanced()) {
    double ml_score = enhanced_ml_predict(flow);

    // Promote based on ML score and current performance
    if (ml_score > 0.75 && flow->pattern.consecutive_fast_paths >= 3) {
      if (flow->confidence < CONFIDENCE_ULTRA_FAST) {
        flow->confidence = CONFIDENCE_ULTRA_FAST;
        flow->previous_type = flow->flow_type;
        flow->flow_type = PROMOTED_FLOW;
        flow->pattern.recent_promotions++;
        g_table->aging_manager->flows_promoted++;
        g_table->ultra_fast_promotions++;
      }
    } else if (ml_score > 0.55 && flow->pattern.consecutive_fast_paths >= 2) {
      if (flow->confidence < CONFIDENCE_FAST_TRACK) {
        flow->confidence = CONFIDENCE_FAST_TRACK;
        flow->flow_type = BURSTY_FLOW;
      }
    }
  }
}

// Enhanced path selection
static inline ProcessingPath select_path_enhanced(uint32_t ip,
                                                  FlowEntry *flow) {
  // Check prediction cache first for established flows
  if (flow && flow->hits > 2) {
    double cached_prediction = check_prediction_cache(ip);
    if (cached_prediction >= 0.0) {
      if (cached_prediction > 0.8)
        return ULTRA_FAST_PATH;
      if (cached_prediction > 0.6)
        return FAST_PATH;
      if (cached_prediction > 0.4)
        return ACCELERATED_PATH;
      return ADAPTIVE_PATH;
    }
  }

  // New flow handling
  if (!flow) {
    uint32_t sketch_count = sketch_query_fast(g_table->sketch, ip);
    return (sketch_count > 8 ? ACCELERATED_PATH : SLOW_PATH);
  }

  // First packet acceleration
  if (flow->hits == 1) {
    return ACCELERATED_PATH;
  }

  // ML-driven selection for established flows
  double ml_prediction = enhanced_ml_predict(flow);
  ProcessingPath selected_path;

  // Consider both confidence and ML prediction
  if (flow->confidence >= CONFIDENCE_ULTRA_FAST && ml_prediction > 0.7) {
    selected_path = ULTRA_FAST_PATH;
  } else if (flow->confidence >= CONFIDENCE_FAST_TRACK && ml_prediction > 0.5) {
    selected_path = FAST_PATH;
  } else if (ml_prediction > 0.6 || flow->pattern.consecutive_fast_paths >= 3) {
    selected_path = ADAPTIVE_PATH;
  } else {
    selected_path = ACCELERATED_PATH;
  }

  // Cache the prediction for future use
  if (flow->hits > 2) {
    update_prediction_cache(ip, ml_prediction, selected_path);
  }

  return selected_path;
}

// Validation for ML performance
static inline void validate_ml_prediction(FlowEntry *flow,
                                          ProcessingPath actual_path) {
  if (!flow || flow->hits < 5)
    return; // Only validate established flows

  double prediction = enhanced_ml_predict(flow);

  // Determine if the prediction was "correct" based on actual path taken
  int predicted_fast = (prediction > 0.6);
  int actual_fast = (actual_path <= FAST_PATH);

  g_table->ml_model->validation_samples++;
  if (predicted_fast == actual_fast) {
    g_table->ml_model->validation_correct++;
  }
}

// Main packet processing
static inline void process_packet_optimized(uint32_t ip) {
  // Update sketch
  sketch_update_fast(g_table->sketch, ip);

  // Lookup or create flow
  FlowEntry *flow = find_flow_fast(ip);
  if (!flow) {
    flow = create_flow_fast(ip);
    if (flow) {
      accelerated_process(ip);
      g_table->path_counts[ACCELERATED_PATH]++;
      update_flow_pattern(flow, ACCELERATED_PATH);
    }
    goto update_stats;
  }

  // Burst promotion
  maybe_promote_burst(flow);

  // Path selection
  ProcessingPath path = select_path_enhanced(ip, flow);
  g_table->path_counts[path]++;

  // Execute processing
  switch (path) {
  case ULTRA_FAST_PATH:
    ultra_fast_process(ip);
    break;
  case FAST_PATH:
    fast_process(ip);
    break;
  case ACCELERATED_PATH:
    accelerated_process(ip);
    break;
  case SLOW_PATH:
    slow_process(ip);
    break;
  case ADAPTIVE_PATH:
    if (enhanced_ml_predict(flow) > 0.75) {
      fast_process(ip);
    } else {
      accelerated_process(ip);
    }
    break;
  case DEEP_ANALYSIS_PATH:
    slow_process(ip);
    break;
  }

  // Update flow pattern and validate ML
  update_flow_pattern(flow, path);
  validate_ml_prediction(flow, path);

update_stats:
  // Update flow statistics
  if (flow) {
    flow->hits++;
    flow->packet_count++;
    flow->last_seen = time(NULL);
    flow->aging.last_access_time = flow->last_seen;
    flow->aging.total_accesses++;

    // Smart confidence updates
    if (flow->hits % 4 == 0 && flow->confidence < 100) {
      double ml_score = enhanced_ml_predict(flow);
      int base_boost = 4;
      int ml_boost = (int)(ml_score * 6.0); // 0-6 additional boost
      int total_boost = base_boost + ml_boost;

      flow->confidence = (flow->confidence + total_boost > 100)
                             ? 100
                             : flow->confidence + total_boost;
      g_table->confidence_updates++;
    }

    // Enhanced flow type classification
    if (flow->packet_count > 800 && flow->flow_type != LARGE_FLOW) {
      flow->previous_type = flow->flow_type;
      flow->flow_type = LARGE_FLOW;
      flow->aging.aging_strategy = AGING_ADAPTIVE;
    } else if (flow->pattern.burst_score > 0.6 && flow->hits > 10) {
      if (flow->flow_type != BURSTY_FLOW && flow->flow_type != PROMOTED_FLOW) {
        flow->previous_type = flow->flow_type;
        flow->flow_type = BURSTY_FLOW;
        flow->aging.aging_strategy = AGING_LINEAR;
      }
    } else if (flow->packet_count < 10 && flow->hits < 5) {
      flow->flow_type = MICRO_FLOW;
      flow->aging.aging_strategy = AGING_AGGRESSIVE;
    }

    // Anomaly detection - simplified
    if (flow->pattern.history_filled && flow->pattern.path_consistency < 0.3) {
      if (flow->flow_type != SUSPECTED_FLOW && flow->hits > 8) {
        flow->previous_type = flow->flow_type;
        flow->flow_type = SUSPECTED_FLOW;
      }
    }

    // Promotion score updates
    if (path <= FAST_PATH) {
      flow->promotion_score =
          (flow->promotion_score < 950) ? flow->promotion_score + 10 : 1000;
    } else if (path >= SLOW_PATH) {
      flow->promotion_score =
          (flow->promotion_score > 50) ? flow->promotion_score - 5 : 0;
    }
  }

  g_table->total_processed++;

  // Periodic maintenance
  if (g_table->total_processed % AGING_INTERVAL == 0) {
    enhanced_aging_cycle();
  }

  if (g_table->total_processed % ML_ADAPTATION_INTERVAL == 0) {
    adapt_ml_model();
  }
}

// Advanced flow lifecycle management
static inline void manage_flow_lifecycle() {
  time_t now = time(NULL);
  int promoted_count = 0;
  int demoted_count = 0;

  for (int i = 0; i < g_table->pool_index && i < 1000; i++) { // Limit scope
    FlowEntry *flow = &g_table->flow_pool[i];
    if (flow->ip == 0)
      continue;

    double idle_time = (double)(now - flow->last_seen);
    double ml_score = enhanced_ml_predict(flow);

    // Promote promising flows
    if (flow->flow_type == NORMAL_FLOW && ml_score > 0.75 &&
        flow->promotion_score > 700 && flow->hits > 8) {
      flow->previous_type = flow->flow_type;
      flow->flow_type = PROMOTED_FLOW;
      flow->confidence = CONFIDENCE_FAST_TRACK;
      promoted_count++;
    }

    // Demote underperforming promoted flows
    if (flow->flow_type == PROMOTED_FLOW &&
        (ml_score < 0.4 || idle_time > 300 || flow->promotion_score < 200)) {
      flow->flow_type = flow->previous_type;
      flow->confidence = flow->confidence > 15 ? flow->confidence - 15 : 10;
      demoted_count++;
    }

    // Age out dying flows
    if (flow->flow_type == DYING_FLOW && idle_time > 900) { // 15 minutes
      flow->confidence = 0;
      // In a real system, we'd reclaim this entry
    }
  }

  g_table->aging_manager->flows_promoted += promoted_count;
  g_table->aging_manager->flows_demoted += demoted_count;
}

// Enhanced statistics reporting
static inline void print_enhanced_statistics() {
  printf("\n=== ENHANCED ML & AGING STATISTICS ===\n");

  // ML Statistics
  MLModel *model = g_table->ml_model;
  double validation_accuracy = 0.0;
  if (model->validation_samples > 0) {
    validation_accuracy =
        (double)model->validation_correct / model->validation_samples;
  }

  printf("ML Model Performance:\n");
  printf("  Validation Accuracy: %.1f%% (%u correct / %u samples)\n",
         validation_accuracy * 100.0, model->validation_correct,
         model->validation_samples);
  printf("  Learning Rate: %.6f\n", model->learning_rate);
  printf("  Total ML Predictions: %llu\n", g_table->ml_predictions);
  printf("  Prediction Cache Hit Rate: %.1f%% (%llu hits)\n",
         g_table->ml_predictions > 0
             ? 100.0 * g_table->ml_cache_hits / g_table->ml_predictions
             : 0.0,
         g_table->ml_cache_hits);

  // Aging Statistics
  AgingManager *manager = g_table->aging_manager;
  printf("\nAging & Lifecycle Management:\n");
  printf("  Memory Utilization: %.1f%% (%d / %d flows)\n",
         manager->memory_utilization * 100.0, g_table->pool_index,
         g_table->pool_size);
  printf("  Aging Pressure: %.1f%%\n", manager->aging_pressure * 100.0);
  printf("  Flows Promoted: %llu\n", manager->flows_promoted);
  printf("  Flows Demoted: %llu\n", manager->flows_demoted);
  printf("  Flows Aged Out: %llu\n", manager->flows_aged_out);
  printf("  Current Burst Rate: %.1f packets/sec\n",
         manager->current_burst_rate);

  // Performance counters
  printf("\nPerformance Metrics:\n");
  printf("  Ultra-fast Promotions: %llu\n", g_table->ultra_fast_promotions);
  printf("  Confidence Updates: %llu\n", g_table->confidence_updates);
  printf("  Pattern Updates: %llu\n", g_table->pattern_updates);

  // Flow Type Distribution with enhanced metrics
  int flow_type_counts[7] = {0};
  double avg_confidence_by_type[7] = {0};
  double avg_ml_score_by_type[7] = {0};
  double avg_promotion_score_by_type[7] = {0};

  for (int i = 0; i < g_table->pool_index; i++) {
    FlowEntry *flow = &g_table->flow_pool[i];
    if (flow->ip == 0)
      continue;

    int type = (int)flow->flow_type;
    if (type >= 0 && type < 7) {
      flow_type_counts[type]++;
      avg_confidence_by_type[type] += flow->confidence;
      avg_ml_score_by_type[type] += enhanced_ml_predict(flow);
      avg_promotion_score_by_type[type] += flow->promotion_score;
    }
  }

  const char *flow_type_names[] = {"Normal", "Large",    "Bursty",   "Micro",
                                   "Dying",  "Promoted", "Suspected"};

  printf("\nFlow Type Distribution:\n");
  for (int i = 0; i < 7; i++) {
    if (flow_type_counts[i] > 0) {
      printf("  %-9s: %5d flows (%4.1f%%) | conf: %4.1f | ML: %.3f | promo: "
             "%4.0f\n",
             flow_type_names[i], flow_type_counts[i],
             100.0 * flow_type_counts[i] / g_table->pool_index,
             avg_confidence_by_type[i] / flow_type_counts[i],
             avg_ml_score_by_type[i] / flow_type_counts[i],
             avg_promotion_score_by_type[i] / flow_type_counts[i]);
    }
  }

  // Pattern Analysis
  double total_path_consistency = 0;
  double total_burst_score = 0;
  int flows_with_patterns = 0;
  int high_consistency_flows = 0;

  for (int i = 0; i < g_table->pool_index; i++) {
    FlowEntry *flow = &g_table->flow_pool[i];
    if (flow->ip == 0)
      continue;

    if (flow->pattern.history_filled || flow->pattern.history_index >= 4) {
      total_path_consistency += flow->pattern.path_consistency;
      total_burst_score += flow->pattern.burst_score;
      flows_with_patterns++;

      if (flow->pattern.path_consistency > 0.8) {
        high_consistency_flows++;
      }
    }
  }

  if (flows_with_patterns > 0) {
    printf("\nPattern Analysis:\n");
    printf("  Flows with Patterns: %d (%.1f%%)\n", flows_with_patterns,
           100.0 * flows_with_patterns / g_table->pool_index);
    printf("  Average Path Consistency: %.3f\n",
           total_path_consistency / flows_with_patterns);
    printf("  High Consistency Flows: %d (%.1f%%)\n", high_consistency_flows,
           100.0 * high_consistency_flows / flows_with_patterns);
    printf("  Average Burst Score: %.3f\n",
           total_burst_score / flows_with_patterns);
  }
}

// Fast dataset reader with flexible filename
int *read_dataset_fast(const char *fn, int *known, int *np, int *ir) {
  FILE *f = fopen(fn, "r");
  if (!f) {
    fprintf(stderr, "Failed to open dataset file: %s\n", fn);
    perror("Error details");
    return NULL;
  }

  if (fscanf(f, "%d %d %d", &INITIAL_KNOWN_SIZE, np, ir) != 3) {
    fprintf(stderr, "Error reading dataset header from: %s\n", fn);
    fclose(f);
    return NULL;
  }

  NUM_PACKETS = *np;
  IP_RANGE = *ir;

  printf("Dataset Info: Known=%d, Packets=%d, IP_Range=%d\n",
         INITIAL_KNOWN_SIZE, NUM_PACKETS, IP_RANGE);

  // Read known flows
  for (int i = 0; i < INITIAL_KNOWN_SIZE; i++) {
    int temp;
    if (fscanf(f, "%d", &temp) != 1) {
      fprintf(stderr, "Error reading known flow %d from: %s\n", i, fn);
      fclose(f);
      return NULL;
    }
    if (i < LARGE_FLOW_AREA_SIZE) {
      known[i] = temp;
    }
  }

  // Read packet data
  int *packets = malloc(NUM_PACKETS * sizeof(int));
  if (!packets) {
    fprintf(stderr, "Memory allocation failed for %d packets\n", NUM_PACKETS);
    fclose(f);
    return NULL;
  }

  for (int i = 0; i < NUM_PACKETS; i++) {
    if (fscanf(f, "%d", &packets[i]) != 1) {
      fprintf(stderr, "Error reading packet %d from: %s\n", i, fn);
      free(packets);
      fclose(f);
      return NULL;
    }
  }

  fclose(f);
  printf("Successfully loaded dataset: %s\n", fn);
  return packets;
}

// Usage function
void print_usage(const char *program_name) {
  printf("Enhanced ML-Driven Flow Processor v2.0\n");
  printf("Usage: %s [dataset_file]\n\n", program_name);
  printf("Arguments:\n");
  printf(
      "  dataset_file    Path to the dataset file (default: dataset.txt)\n\n");
  printf("Examples:\n");
  printf("  %s                           # Use default dataset.txt\n",
         program_name);
  printf("  %s tests/dataset_web.txt     # Test with web traffic\n",
         program_name);
  printf("  %s tests/dataset_ddos.txt    # Test with DDoS simulation\n",
         program_name);
  printf("  %s tests/dataset_gaming.txt  # Test with gaming traffic\n\n",
         program_name);
  printf("Available test datasets:\n");
  printf("  dataset_uniform.txt      - Uniform random (baseline)\n");
  printf("  dataset_web.txt         - Web traffic (Zipf 80/20)\n");
  printf("  dataset_datacenter.txt  - Datacenter east-west\n");
  printf("  dataset_ddos.txt        - DDoS attack simulation\n");
  printf("  dataset_streaming.txt   - Video streaming\n");
  printf("  dataset_iot.txt         - IoT sensor network\n");
  printf("  dataset_gaming.txt      - Gaming traffic\n");
  printf("  dataset_cdn.txt         - CDN edge traffic\n");
  printf("  dataset_enterprise.txt  - Enterprise mixed\n");
  printf("  dataset_pareto.txt      - Pareto heavy-tail\n");
}

int main(int argc, char *argv[]) {
  // Handle command line arguments
  const char *dataset_file = "dataset.txt"; // Default

  if (argc > 2) {
    printf("Error: Too many arguments\n\n");
    print_usage(argv[0]);
    return 1;
  }

  if (argc == 2) {
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
      print_usage(argv[0]);
      return 0;
    }
    dataset_file = argv[1];
  }

  printf("=== Enhanced ML-Driven Flow Processor v2.0 ===\n");
  printf("Dataset: %s\n", dataset_file);
  printf("Initializing optimized data structures...\n\n");

  g_table = init_optimized_table();
  if (!g_table) {
    fprintf(stderr, "Failed to initialize table\n");
    return 1;
  }

  int known[LARGE_FLOW_AREA_SIZE] = {0};
  int np, ir;
  int *packets = read_dataset_fast(dataset_file, known, &np, &ir);
  if (!packets) {
    fprintf(stderr, "Failed to read dataset: %s\n", dataset_file);
    fprintf(stderr, "Make sure the file exists and is in the correct format\n");
    return 1;
  }

  // Pre-populate known flows with enhanced initialization
  printf("Pre-populating %d known flows...\n", INITIAL_KNOWN_SIZE);
  for (int i = 0; i < INITIAL_KNOWN_SIZE && i < LARGE_FLOW_AREA_SIZE; i++) {
    if (known[i] > 0) {
      FlowEntry *flow = create_flow_fast((uint32_t)known[i]);
      if (flow) {
        flow->confidence = 75; // Higher starting confidence for known flows
        flow->hits = 12;
        flow->packet_count = 15;
        flow->flow_type = LARGE_FLOW;
        flow->aging.aging_strategy = AGING_ADAPTIVE;
        flow->promotion_score = 800; // High promotion potential

        // Initialize with good patterns
        flow->pattern.path_consistency = 0.85;
        flow->pattern.burst_score = 0.15;
        flow->pattern.consecutive_fast_paths = 5;
      }
    }
  }

  printf("Processing %d packets with enhanced ML and aging...\n", NUM_PACKETS);
  printf("Configuration: BURST_THRESHOLD=%d, ML_FEATURES=%d, CACHE_SIZE=%d\n\n",
         BURST_THRESHOLD, ML_FEATURE_COUNT, CACHE_SIZE);

  clock_t start_time = clock();

  for (int i = 0; i < NUM_PACKETS; i++) {
    process_packet_optimized((uint32_t)packets[i]);

    // Periodic lifecycle management (less frequent)
    if (i % 100000 == 0 && i > 0) {
      manage_flow_lifecycle();
    }

    if (i % 200000 == 0 && i > 0) {
      printf("Processed %d packets (%.1f%%) | Flows: %d | Cache hit: %.1f%%\n",
             i, 100.0 * i / NUM_PACKETS, g_table->pool_index,
             100.0 * g_table->cache_hits /
                 (g_table->cache_hits + g_table->cache_misses));
    }
  }

  clock_t end_time = clock();
  double total_seconds = (double)(end_time - start_time) / CLOCKS_PER_SEC;

  // Final lifecycle management
  manage_flow_lifecycle();

  printf("\n=== ENHANCED RESULTS ===\n");
  printf("Dataset: %s\n", dataset_file);
  printf("Parameters: KNOWN=%d, PACKETS=%d, IP_RANGE=%d\n", INITIAL_KNOWN_SIZE,
         NUM_PACKETS, IP_RANGE);
  printf("Total Processing Time: %.3f seconds\n", total_seconds);
  printf("Throughput: %.2f Mpps (%.0f packets/sec)\n",
         NUM_PACKETS / total_seconds / 1e6, NUM_PACKETS / total_seconds);
  printf("Average Packet Time: %.2f ns\n", total_seconds * 1e9 / NUM_PACKETS);
  printf("Total Flows Created: %d (%.2f%% of pool)\n", g_table->pool_index,
         100.0 * g_table->pool_index / g_table->pool_size);

  // Processing path distribution
  const char *path_names[] = {"Fast", "Accelerated", "Ultra-Fast",
                              "Slow", "Adaptive",    "Deep"};
  printf("\nProcessing Path Distribution:\n");
  for (int i = 0; i < 6; i++) {
    printf("  %-12s: %8llu (%5.2f%%)\n", path_names[i], g_table->path_counts[i],
           100.0 * g_table->path_counts[i] / NUM_PACKETS);
  }

  // Cache and hash performance
  uint64_t total_cache_ops = g_table->cache_hits + g_table->cache_misses;
  printf("\nCache & Hash Performance:\n");
  printf("  Cache Hit Rate: %.2f%% (%llu / %llu)\n",
         100.0 * g_table->cache_hits / total_cache_ops, g_table->cache_hits,
         total_cache_ops);
  printf("  Hash Collision Rate: %.2f%% (%llu / %llu)\n",
         100.0 * g_table->hash_table->collision_count /
             g_table->hash_table->total_lookups,
         g_table->hash_table->collision_count,
         g_table->hash_table->total_lookups);

  // Print detailed statistics
  print_enhanced_statistics();

  // Cleanup
  free(g_table->hash_table);
  free(g_table->sketch);
  free(g_table->ml_model);
  free(g_table->aging_manager);
  free(g_table->flow_pool);
  free(g_table);
  free(packets);

  printf("\n=== Processing Complete ===\n");
  return 0;
}
