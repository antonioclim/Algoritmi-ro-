/**
 * ============================================================================
 * SOLUȚIE EXERCISE 2: Multi-Method Anomaly Detector
 * ============================================================================
 * 
 * Săptămâna 19: Algoritmi pentru IoT și Stream Processing
 * Curs: Algoritmi și Tehnici de Programare, ASE București
 * 
 * DESCRIERE:
 *   Implementare completă a unui detector de anomalii folosind multiple
 *   metode: Z-score, IQR, și threshold absolut. Include rate limiting
 *   pentru prevenirea alert fatigue.
 * 
 * COMPILARE:
 *   gcc -Wall -Wextra -std=c11 -O2 -o exercise2_sol exercise2_sol.c -lm
 * 
 * RULARE:
 *   ./exercise2_sol [input.csv]
 * 
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <float.h>

/* ============================================================================
 * CONSTANTE ȘI CONFIGURARE
 * ============================================================================ */

#define MAX_WINDOW_SIZE      100
#define SENSOR_ID_LEN        32

// Parametri detecție
#define ZSCORE_THRESHOLD     2.5
#define IQR_MULTIPLIER       1.5
#define WARMUP_SAMPLES       30

// Rate limiting
#define MAX_ALERTS_PER_WINDOW 5
#define ALERT_WINDOW_MS       60000   // 1 minut
#define COOLDOWN_MS           30000   // 30 secunde

// Threshold-uri absolute (exemplu pentru temperatură)
#define ABSOLUTE_MIN         10.0
#define ABSOLUTE_MAX         40.0

/* ============================================================================
 * STRUCTURI DE DATE
 * ============================================================================ */

/**
 * Tip anomalie detectată
 */
typedef enum {
    ANOMALY_NONE       = 0,
    ANOMALY_ZSCORE     = 1 << 0,  // Z-score mare
    ANOMALY_IQR        = 1 << 1,  // În afara IQR bounds
    ANOMALY_THRESHOLD  = 1 << 2,  // Threshold absolut depășit
    ANOMALY_SPIKE      = 1 << 3,  // Schimbare bruscă
} AnomalyType;

/**
 * Rezultat detecție
 */
typedef struct {
    AnomalyType types;     // Bitmask al tipurilor detectate
    double zscore;         // Z-score calculat
    double iqr_distance;   // Distanța față de bounds IQR
    double spike_delta;    // Delta față de valoarea anterioară
    bool is_anomaly;       // Verdict final
    int severity;          // 0-3 (mai mare = mai sever)
} DetectionResult;

/**
 * Detector Z-score cu algoritmul Welford
 */
typedef struct {
    double mean;
    double M2;
    uint64_t count;
    double threshold;
} ZScoreDetector;

/**
 * Detector IQR cu buffer circular
 */
typedef struct {
    double* buffer;
    double* sorted;        // Buffer auxiliar pentru sortare
    int capacity;
    int head;
    int count;
    double k;              // Multiplicator IQR
    
    // Cache quartile
    double Q1, Q3, IQR;
    double lower_bound, upper_bound;
} IQRDetector;

/**
 * Detector spike (schimbări bruște)
 */
typedef struct {
    double last_value;
    double max_delta_ratio;  // Delta maxim ca procent din medie
    bool initialized;
} SpikeDetector;

/**
 * Rate limiter sliding window
 */
typedef struct {
    uint64_t* timestamps;
    int capacity;
    int head;
    int count;
    uint64_t window_ms;
    uint64_t cooldown_until;
    int suppressed_count;
} RateLimiter;

/**
 * Detector combinat
 */
typedef struct {
    ZScoreDetector zscore;
    IQRDetector iqr;
    SpikeDetector spike;
    RateLimiter limiter;
    
    // Threshold-uri absolute
    double absolute_min;
    double absolute_max;
    
    // Statistici
    int total_samples;
    int anomalies_detected;
    int alerts_sent;
    int alerts_suppressed;
    int anomalies_by_type[4];  // Index = log2(type)
} MultiMethodDetector;

/* ============================================================================
 * IMPLEMENTĂRI Z-SCORE DETECTOR
 * ============================================================================ */

void zscore_init(ZScoreDetector* d, double threshold) {
    d->mean = 0.0;
    d->M2 = 0.0;
    d->count = 0;
    d->threshold = threshold;
}

bool zscore_check(ZScoreDetector* d, double value, double* out_zscore) {
    d->count++;
    
    // Algoritmul Welford
    double delta = value - d->mean;
    d->mean += delta / d->count;
    double delta2 = value - d->mean;
    d->M2 += delta * delta2;
    
    *out_zscore = 0.0;
    
    if (d->count < WARMUP_SAMPLES) {
        return false;
    }
    
    double variance = d->M2 / d->count;
    double stddev = sqrt(variance);
    
    if (stddev < 1e-10) {
        return false;
    }
    
    *out_zscore = fabs(value - d->mean) / stddev;
    return *out_zscore > d->threshold;
}

double zscore_get_mean(const ZScoreDetector* d) { return d->mean; }
double zscore_get_stddev(const ZScoreDetector* d) {
    if (d->count < 2) return 0.0;
    return sqrt(d->M2 / d->count);
}

/* ============================================================================
 * IMPLEMENTĂRI IQR DETECTOR
 * ============================================================================ */

int compare_doubles(const void* a, const void* b) {
    double da = *(const double*)a;
    double db = *(const double*)b;
    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}

bool iqr_init(IQRDetector* d, int window_size, double k) {
    d->buffer = (double*)malloc(window_size * sizeof(double));
    d->sorted = (double*)malloc(window_size * sizeof(double));
    
    if (!d->buffer || !d->sorted) {
        free(d->buffer);
        free(d->sorted);
        return false;
    }
    
    d->capacity = window_size;
    d->head = 0;
    d->count = 0;
    d->k = k;
    
    d->Q1 = d->Q3 = d->IQR = 0.0;
    d->lower_bound = -DBL_MAX;
    d->upper_bound = DBL_MAX;
    
    return true;
}

void iqr_destroy(IQRDetector* d) {
    free(d->buffer);
    free(d->sorted);
}

void iqr_update_bounds(IQRDetector* d) {
    if (d->count < 4) return;
    
    // Copiem și sortăm
    memcpy(d->sorted, d->buffer, d->count * sizeof(double));
    qsort(d->sorted, d->count, sizeof(double), compare_doubles);
    
    // Calculăm quartilele
    int n = d->count;
    d->Q1 = d->sorted[n / 4];
    d->Q3 = d->sorted[3 * n / 4];
    d->IQR = d->Q3 - d->Q1;
    
    // Limite pentru outlieri
    d->lower_bound = d->Q1 - d->k * d->IQR;
    d->upper_bound = d->Q3 + d->k * d->IQR;
}

bool iqr_check(IQRDetector* d, double value, double* out_distance) {
    // Adăugăm valoarea la buffer
    if (d->count < d->capacity) {
        d->buffer[d->count] = value;
        d->count++;
    } else {
        d->buffer[d->head] = value;
        d->head = (d->head + 1) % d->capacity;
    }
    
    // Actualizăm bounds
    iqr_update_bounds(d);
    
    *out_distance = 0.0;
    
    if (d->count < d->capacity / 2) {
        return false;  // Warmup
    }
    
    // Verificăm dacă e outlier
    if (value < d->lower_bound) {
        *out_distance = d->lower_bound - value;
        return true;
    } else if (value > d->upper_bound) {
        *out_distance = value - d->upper_bound;
        return true;
    }
    
    return false;
}

/* ============================================================================
 * IMPLEMENTĂRI SPIKE DETECTOR
 * ============================================================================ */

void spike_init(SpikeDetector* d, double max_delta_ratio) {
    d->last_value = 0.0;
    d->max_delta_ratio = max_delta_ratio;
    d->initialized = false;
}

bool spike_check(SpikeDetector* d, double value, double mean, double* out_delta) {
    *out_delta = 0.0;
    
    if (!d->initialized) {
        d->last_value = value;
        d->initialized = true;
        return false;
    }
    
    double delta = fabs(value - d->last_value);
    *out_delta = delta;
    
    d->last_value = value;
    
    // Spike dacă delta > ratio * mean
    if (mean > 1e-10) {
        return delta > d->max_delta_ratio * mean;
    }
    
    return false;
}

/* ============================================================================
 * IMPLEMENTĂRI RATE LIMITER
 * ============================================================================ */

bool rate_limiter_init(RateLimiter* rl, int max_alerts, uint64_t window_ms) {
    rl->timestamps = (uint64_t*)malloc(max_alerts * sizeof(uint64_t));
    if (!rl->timestamps) return false;
    
    rl->capacity = max_alerts;
    rl->head = 0;
    rl->count = 0;
    rl->window_ms = window_ms;
    rl->cooldown_until = 0;
    rl->suppressed_count = 0;
    
    return true;
}

void rate_limiter_destroy(RateLimiter* rl) {
    free(rl->timestamps);
}

bool rate_limiter_allow(RateLimiter* rl, uint64_t timestamp) {
    // Verificăm cooldown
    if (timestamp < rl->cooldown_until) {
        rl->suppressed_count++;
        return false;
    }
    
    // Curățăm timestamps expirate
    while (rl->count > 0) {
        uint64_t oldest = rl->timestamps[rl->head];
        if (timestamp - oldest > rl->window_ms) {
            rl->head = (rl->head + 1) % rl->capacity;
            rl->count--;
        } else {
            break;
        }
    }
    
    // Verificăm limita
    if (rl->count >= rl->capacity) {
        rl->cooldown_until = timestamp + COOLDOWN_MS;
        rl->suppressed_count++;
        return false;
    }
    
    // Înregistrăm timestamp
    int idx = (rl->head + rl->count) % rl->capacity;
    rl->timestamps[idx] = timestamp;
    rl->count++;
    
    return true;
}

/* ============================================================================
 * IMPLEMENTĂRI DETECTOR COMBINAT
 * ============================================================================ */

bool detector_init(MultiMethodDetector* d) {
    zscore_init(&d->zscore, ZSCORE_THRESHOLD);
    
    if (!iqr_init(&d->iqr, MAX_WINDOW_SIZE, IQR_MULTIPLIER)) {
        return false;
    }
    
    spike_init(&d->spike, 0.3);  // 30% din medie
    
    if (!rate_limiter_init(&d->limiter, MAX_ALERTS_PER_WINDOW, ALERT_WINDOW_MS)) {
        iqr_destroy(&d->iqr);
        return false;
    }
    
    d->absolute_min = ABSOLUTE_MIN;
    d->absolute_max = ABSOLUTE_MAX;
    
    d->total_samples = 0;
    d->anomalies_detected = 0;
    d->alerts_sent = 0;
    d->alerts_suppressed = 0;
    memset(d->anomalies_by_type, 0, sizeof(d->anomalies_by_type));
    
    return true;
}

void detector_destroy(MultiMethodDetector* d) {
    iqr_destroy(&d->iqr);
    rate_limiter_destroy(&d->limiter);
}

DetectionResult detector_check(MultiMethodDetector* d, double value, uint64_t timestamp) {
    DetectionResult result = {0};
    
    d->total_samples++;
    
    // 1. Z-score check
    double zscore;
    if (zscore_check(&d->zscore, value, &zscore)) {
        result.types |= ANOMALY_ZSCORE;
        d->anomalies_by_type[0]++;
    }
    result.zscore = zscore;
    
    // 2. IQR check
    double iqr_dist;
    if (iqr_check(&d->iqr, value, &iqr_dist)) {
        result.types |= ANOMALY_IQR;
        d->anomalies_by_type[1]++;
    }
    result.iqr_distance = iqr_dist;
    
    // 3. Threshold absolut
    if (value < d->absolute_min || value > d->absolute_max) {
        result.types |= ANOMALY_THRESHOLD;
        d->anomalies_by_type[2]++;
    }
    
    // 4. Spike check
    double spike_delta;
    if (spike_check(&d->spike, value, zscore_get_mean(&d->zscore), &spike_delta)) {
        result.types |= ANOMALY_SPIKE;
        d->anomalies_by_type[3]++;
    }
    result.spike_delta = spike_delta;
    
    // Verdict final: anomalie dacă cel puțin o metodă a detectat
    result.is_anomaly = (result.types != ANOMALY_NONE);
    
    // Calculăm severitatea
    result.severity = __builtin_popcount(result.types);
    if (result.types & ANOMALY_THRESHOLD) result.severity++;  // Threshold e mai sever
    
    if (result.is_anomaly) {
        d->anomalies_detected++;
        
        // Rate limiting pentru alertare
        if (rate_limiter_allow(&d->limiter, timestamp)) {
            d->alerts_sent++;
        } else {
            d->alerts_suppressed++;
        }
    }
    
    return result;
}

/* ============================================================================
 * AFIȘARE ȘI RAPORTARE
 * ============================================================================ */

const char* anomaly_type_to_string(AnomalyType type) {
    static char buffer[128];
    buffer[0] = '\0';
    
    if (type & ANOMALY_ZSCORE)    strcat(buffer, "ZSCORE ");
    if (type & ANOMALY_IQR)       strcat(buffer, "IQR ");
    if (type & ANOMALY_THRESHOLD) strcat(buffer, "THRESHOLD ");
    if (type & ANOMALY_SPIKE)     strcat(buffer, "SPIKE ");
    
    if (buffer[0] == '\0') strcpy(buffer, "NONE");
    
    return buffer;
}

void detector_print_result(const DetectionResult* r, double value, uint64_t timestamp) {
    if (!r->is_anomaly) return;
    
    printf("[ANOMALY] ts=%lu value=%.2f severity=%d types=%s\n",
           (unsigned long)timestamp, value, r->severity,
           anomaly_type_to_string(r->types));
    printf("          zscore=%.2f iqr_dist=%.2f spike_delta=%.2f\n",
           r->zscore, r->iqr_distance, r->spike_delta);
}

void detector_print_report(const MultiMethodDetector* d) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║            RAPORT DETECTOR ANOMALII                         ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ Total samples procesate: %-10d                         ║\n", d->total_samples);
    printf("║ Anomalii detectate:      %-10d (%.2f%%)                ║\n", 
           d->anomalies_detected, 
           100.0 * d->anomalies_detected / (d->total_samples > 0 ? d->total_samples : 1));
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ ANOMALII PE TIP:                                            ║\n");
    printf("║   Z-score:    %-6d                                        ║\n", d->anomalies_by_type[0]);
    printf("║   IQR:        %-6d                                        ║\n", d->anomalies_by_type[1]);
    printf("║   Threshold:  %-6d                                        ║\n", d->anomalies_by_type[2]);
    printf("║   Spike:      %-6d                                        ║\n", d->anomalies_by_type[3]);
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ RATE LIMITING:                                              ║\n");
    printf("║   Alerte trimise:     %-6d                                ║\n", d->alerts_sent);
    printf("║   Alerte suprimate:   %-6d                                ║\n", d->alerts_suppressed);
    printf("║   Eficiență limiter:  %.1f%%                                ║\n",
           d->alerts_suppressed > 0 ? 
           100.0 * d->alerts_suppressed / (d->alerts_sent + d->alerts_suppressed) : 0.0);
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ STATISTICI DISTRIBUȚIE:                                     ║\n");
    printf("║   Media:     %.2f                                          ║\n", 
           zscore_get_mean(&d->zscore));
    printf("║   Std Dev:   %.2f                                          ║\n",
           zscore_get_stddev(&d->zscore));
    printf("║   IQR:       %.2f  [Q1=%.2f, Q3=%.2f]                      ║\n",
           d->iqr.IQR, d->iqr.Q1, d->iqr.Q3);
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
}

/* ============================================================================
 * GENERARE DATE TEST
 * ============================================================================ */

void generate_and_process_test_data(MultiMethodDetector* d, int n_samples) {
    srand(42);
    
    uint64_t timestamp = 1704067200000;  // 2024-01-01 08:00:00
    
    printf("Procesare %d samples...\n\n", n_samples);
    
    for (int i = 0; i < n_samples; i++) {
        // Generare valoare cu zgomot
        double base = 25.0 + 3.0 * sin(2.0 * M_PI * i / 200);
        
        // Box-Muller pentru zgomot Gaussian
        double u1 = (rand() + 1.0) / (RAND_MAX + 2.0);
        double u2 = (rand() + 1.0) / (RAND_MAX + 2.0);
        double noise = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2) * 0.8;
        
        double value = base + noise;
        
        // Injectare anomalii
        int anomaly_type = rand() % 100;
        if (anomaly_type < 2) {
            // Outlier major (spike)
            value += (rand() % 2 ? 1 : -1) * (15 + rand() % 10);
        } else if (anomaly_type < 4) {
            // Outlier moderat
            value += (rand() % 2 ? 1 : -1) * (5 + rand() % 5);
        } else if (anomaly_type < 5) {
            // Valoare în afara threshold absolut
            value = (rand() % 2) ? 5.0 : 45.0;
        }
        
        // Procesare
        DetectionResult result = detector_check(d, value, timestamp);
        
        // Afișare anomalii
        if (result.is_anomaly) {
            detector_print_result(&result, value, timestamp);
        }
        
        timestamp += 1000;  // +1 secundă
    }
}

/* ============================================================================
 * PROCESARE CSV
 * ============================================================================ */

int process_csv_file(MultiMethodDetector* d, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Eroare: Nu se poate deschide %s\n", filename);
        return -1;
    }
    
    char line[256];
    
    // Skip header
    if (fgets(line, sizeof(line), f) == NULL) {
        fclose(f);
        return -1;
    }
    
    while (fgets(line, sizeof(line), f)) {
        uint64_t timestamp;
        double value;
        
        // Format: timestamp,value sau timestamp,sensor_id,value
        if (sscanf(line, "%lu,%lf", &timestamp, &value) == 2 ||
            sscanf(line, "%lu,%*[^,],%lf", &timestamp, &value) == 2) {
            
            DetectionResult result = detector_check(d, value, timestamp);
            
            if (result.is_anomaly) {
                detector_print_result(&result, value, timestamp);
            }
        }
    }
    
    fclose(f);
    return 0;
}

/* ============================================================================
 * MAIN
 * ============================================================================ */

int main(int argc, char* argv[]) {
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║     MULTI-METHOD ANOMALY DETECTOR - SOLUȚIE                 ║\n");
    printf("║     ATP Week 19 - IoT Stream Processing                     ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
    
    MultiMethodDetector detector;
    
    if (!detector_init(&detector)) {
        fprintf(stderr, "Eroare la inițializarea detectorului\n");
        return 1;
    }
    
    if (argc > 1) {
        printf("Procesare fișier: %s\n\n", argv[1]);
        process_csv_file(&detector, argv[1]);
    } else {
        printf("Mod demo - generare date test\n\n");
        generate_and_process_test_data(&detector, 2000);
    }
    
    // Raport final
    detector_print_report(&detector);
    
    detector_destroy(&detector);
    
    return 0;
}
