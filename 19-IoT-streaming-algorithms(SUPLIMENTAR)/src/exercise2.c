/**
 * =============================================================================
 * EXERCIȚIUL 2: ANOMALY DETECTOR CU RATE LIMITING
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unui detector de anomalii în flux cu protecție împotriva
 *   alertelor excesive (rate limiting) și cooldown între alerte consecutive.
 *
 * CERINȚE:
 *   1. Z-score detector cu fereastră adaptivă pentru μ și σ
 *   2. Rate limiting: maxim 3 alerte per senzor pe minut
 *   3. Cooldown de 30 secunde între alerte consecutive pentru același senzor
 *   4. Log cu context (valoare, timestamp, z-score)
 *   5. Statistici finale: total alerte, rata estimată de false positives
 *
 * EXEMPLU INPUT (CSV din stdin):
 *   timestamp,sensor_id,value
 *   1000,TEMP_01,22.5
 *   2000,TEMP_01,22.3
 *   3000,TEMP_01,45.0
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c -lm
 * EXECUȚIE:  ./exercise2 < data/sensor_temperature.csv
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <float.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_SENSORS 32
#define MAX_LINE_LENGTH 256
#define WARMUP_SAMPLES 20

#define ZSCORE_THRESHOLD 2.5
#define MAX_ALERTS_PER_MINUTE 3
#define COOLDOWN_SECONDS 30

/* Culori pentru output */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_CYAN    "\033[0;36m"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Statistici running folosind algoritmul Welford
 */
typedef struct {
    size_t count;
    double mean;
    double m2;      /* Suma pătratelor diferențelor de la medie */
} RunningStats;

/**
 * TODO 1: Structură pentru rate limiter
 *
 * Hint: Trebuie să ții evidența alertelor din ultimul minut
 */
typedef struct {
    /* YOUR CODE HERE */
    uint64_t alert_timestamps[MAX_ALERTS_PER_MINUTE + 1];
    size_t alert_count;
    uint64_t last_alert_time;
} RateLimiter;

/**
 * Structură pentru un senzor monitorizat
 */
typedef struct {
    char sensor_id[32];
    RunningStats stats;
    RateLimiter rate_limiter;
    size_t total_readings;
    size_t total_alerts;
    size_t suppressed_alerts;
    bool active;
} SensorMonitor;

/**
 * Context pentru o alertă
 */
typedef struct {
    uint64_t timestamp;
    char sensor_id[32];
    double value;
    double zscore;
    double mean;
    double stddev;
} AlertContext;

/* =============================================================================
 * FUNCȚII STATISTICI (FURNIZATE)
 * =============================================================================
 */

/**
 * Inițializează statistici running
 */
void stats_init(RunningStats *stats) {
    stats->count = 0;
    stats->mean = 0.0;
    stats->m2 = 0.0;
}

/**
 * Actualizează statistici cu o nouă valoare (Welford's algorithm)
 */
void stats_update(RunningStats *stats, double value) {
    stats->count++;
    double delta = value - stats->mean;
    stats->mean += delta / stats->count;
    double delta2 = value - stats->mean;
    stats->m2 += delta * delta2;
}

/**
 * Calculează deviația standard
 */
double stats_stddev(RunningStats *stats) {
    if (stats->count < 2) return 0.0;
    return sqrt(stats->m2 / (stats->count - 1));
}

/**
 * Calculează Z-score pentru o valoare
 */
double calculate_zscore(RunningStats *stats, double value) {
    double stddev = stats_stddev(stats);
    if (stddev < 0.0001) return 0.0;
    return (value - stats->mean) / stddev;
}

/* =============================================================================
 * FUNCȚII DE IMPLEMENTAT
 * =============================================================================
 */

/**
 * TODO 2: Inițializează rate limiter
 */
void rate_limiter_init(RateLimiter *rl) {
    /* YOUR CODE HERE */
    for (int i = 0; i <= MAX_ALERTS_PER_MINUTE; i++) {
        rl->alert_timestamps[i] = 0;
    }
    rl->alert_count = 0;
    rl->last_alert_time = 0;
}

/**
 * TODO 3: Verifică dacă o alertă poate fi emisă
 *
 * Verificări:
 * 1. Cooldown: au trecut cel puțin COOLDOWN_SECONDS de la ultima alertă
 * 2. Rate limit: nu s-au emis mai mult de MAX_ALERTS_PER_MINUTE în ultimul minut
 *
 * @param rl Rate limiter
 * @param current_time Timestamp curent (în milisecunde)
 * @return true dacă alerta poate fi emisă
 */
bool rate_limiter_can_alert(RateLimiter *rl, uint64_t current_time) {
    /* YOUR CODE HERE */
    
    /* Convertește în secunde pentru ușurință */
    uint64_t current_sec = current_time / 1000;
    uint64_t last_sec = rl->last_alert_time / 1000;
    
    /* Verifică cooldown */
    if (rl->last_alert_time > 0 && (current_sec - last_sec) < COOLDOWN_SECONDS) {
        return false;
    }
    
    /* Numără alertele din ultimul minut */
    size_t recent_alerts = 0;
    for (size_t i = 0; i < rl->alert_count && i <= MAX_ALERTS_PER_MINUTE; i++) {
        uint64_t alert_sec = rl->alert_timestamps[i] / 1000;
        if (current_sec - alert_sec < 60) {
            recent_alerts++;
        }
    }
    
    return recent_alerts < MAX_ALERTS_PER_MINUTE;
}

/**
 * TODO 4: Înregistrează o alertă emisă
 */
void rate_limiter_record_alert(RateLimiter *rl, uint64_t current_time) {
    /* YOUR CODE HERE */
    
    /* Shift timestamps și adaugă noul timestamp */
    for (int i = MAX_ALERTS_PER_MINUTE - 1; i > 0; i--) {
        rl->alert_timestamps[i] = rl->alert_timestamps[i - 1];
    }
    rl->alert_timestamps[0] = current_time;
    
    if (rl->alert_count < MAX_ALERTS_PER_MINUTE + 1) {
        rl->alert_count++;
    }
    
    rl->last_alert_time = current_time;
}

/**
 * TODO 5: Găsește sau creează monitorul pentru un senzor
 */
SensorMonitor* find_or_create_monitor(SensorMonitor monitors[], size_t *count, 
                                       const char *sensor_id) {
    /* YOUR CODE HERE */
    
    /* Caută senzorul existent */
    for (size_t i = 0; i < *count; i++) {
        if (monitors[i].active && strcmp(monitors[i].sensor_id, sensor_id) == 0) {
            return &monitors[i];
        }
    }
    
    /* Creează unul nou dacă e loc */
    if (*count < MAX_SENSORS) {
        SensorMonitor *m = &monitors[*count];
        strncpy(m->sensor_id, sensor_id, sizeof(m->sensor_id) - 1);
        stats_init(&m->stats);
        rate_limiter_init(&m->rate_limiter);
        m->total_readings = 0;
        m->total_alerts = 0;
        m->suppressed_alerts = 0;
        m->active = true;
        (*count)++;
        return m;
    }
    
    return NULL;
}

/**
 * TODO 6: Procesează o citire și detectează anomalii
 *
 * @param monitor Monitorul senzorului
 * @param value Valoarea citită
 * @param timestamp Timestamp-ul citirii
 * @param alert_ctx Pointer pentru contextul alertei (dacă se emite)
 * @return 0 = fără anomalie, 1 = alertă emisă, -1 = alertă suprimată (rate limit)
 */
int process_reading(SensorMonitor *monitor, double value, uint64_t timestamp,
                    AlertContext *alert_ctx) {
    /* YOUR CODE HERE */
    
    monitor->total_readings++;
    
    /* În perioada de warmup, doar acumulăm statistici */
    if (monitor->stats.count < WARMUP_SAMPLES) {
        stats_update(&monitor->stats, value);
        return 0;
    }
    
    /* Calculează Z-score înainte de update */
    double zscore = calculate_zscore(&monitor->stats, value);
    
    /* Actualizează statisticile */
    stats_update(&monitor->stats, value);
    
    /* Verifică dacă e anomalie */
    if (fabs(zscore) > ZSCORE_THRESHOLD) {
        /* Verifică rate limiting */
        if (rate_limiter_can_alert(&monitor->rate_limiter, timestamp)) {
            /* Emite alerta */
            rate_limiter_record_alert(&monitor->rate_limiter, timestamp);
            monitor->total_alerts++;
            
            if (alert_ctx) {
                alert_ctx->timestamp = timestamp;
                strncpy(alert_ctx->sensor_id, monitor->sensor_id, 
                       sizeof(alert_ctx->sensor_id) - 1);
                alert_ctx->value = value;
                alert_ctx->zscore = zscore;
                alert_ctx->mean = monitor->stats.mean;
                alert_ctx->stddev = stats_stddev(&monitor->stats);
            }
            
            return 1;  /* Alertă emisă */
        } else {
            monitor->suppressed_alerts++;
            return -1;  /* Alertă suprimată */
        }
    }
    
    return 0;  /* Fără anomalie */
}

/**
 * Afișează alertă formatată
 */
void print_alert(AlertContext *ctx) {
    printf("%s[ANOMALIE]%s [T=%lu ms] Senzor %s: valoare=%.2f, z-score=%.2f "
           "(μ=%.2f, σ=%.2f)\n",
           COLOR_RED, COLOR_RESET,
           (unsigned long)ctx->timestamp,
           ctx->sensor_id,
           ctx->value,
           ctx->zscore,
           ctx->mean,
           ctx->stddev);
}

/**
 * Parsează o linie CSV
 * Format: timestamp,sensor_id,value
 */
bool parse_csv_line(const char *line, uint64_t *timestamp, 
                    char *sensor_id, double *value) {
    /* Skip header */
    if (strncmp(line, "timestamp", 9) == 0) return false;
    
    char *endptr;
    *timestamp = strtoull(line, &endptr, 10);
    if (*endptr != ',') return false;
    
    const char *id_start = endptr + 1;
    const char *id_end = strchr(id_start, ',');
    if (!id_end) return false;
    
    size_t id_len = id_end - id_start;
    if (id_len >= 32) id_len = 31;
    strncpy(sensor_id, id_start, id_len);
    sensor_id[id_len] = '\0';
    
    *value = strtod(id_end + 1, NULL);
    
    return true;
}

/* =============================================================================
 * FUNCȚIE PRINCIPALĂ
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 2: ANOMALY DETECTOR CU RATE LIMITING                           ║\n");
    printf("║     Detecție anomalii Z-score cu protecție împotriva alertelor excesive       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("Configurație:\n");
    printf("  • Prag Z-score: %.1f\n", ZSCORE_THRESHOLD);
    printf("  • Max alerte/minut: %d\n", MAX_ALERTS_PER_MINUTE);
    printf("  • Cooldown: %d secunde\n", COOLDOWN_SECONDS);
    printf("  • Warmup: %d samples\n\n", WARMUP_SAMPLES);
    
    /* Array de monitoare pentru senzori */
    SensorMonitor monitors[MAX_SENSORS];
    size_t monitor_count = 0;
    
    /* Statistici globale */
    size_t total_lines = 0;
    size_t total_alerts = 0;
    size_t total_suppressed = 0;
    
    /* Citește din stdin sau generează date de test */
    char line[MAX_LINE_LENGTH];
    bool has_input = false;
    
    /* Verifică dacă avem input */
    printf("Citire date (stdin sau test generat)...\n\n");
    
    /* Dacă nu avem input, generăm date de test */
    if (!fgets(line, sizeof(line), stdin)) {
        printf("Nu s-au găsit date de intrare. Generăm date de test...\n\n");
        
        /* Generăm date simulate */
        srand((unsigned int)time(NULL));
        
        for (int t = 0; t < 300; t++) {
            uint64_t timestamp = t * 1000;
            
            /* Valoare normală cu ocazionale anomalii */
            double value = 22.0 + ((rand() % 100) / 100.0 - 0.5);
            
            /* Injectăm anomalii la anumite momente */
            if (t == 50 || t == 100 || t == 101 || t == 102 || 
                t == 150 || t == 200 || t == 250) {
                value = (rand() % 2) ? 35.0 : 10.0;
            }
            
            SensorMonitor *m = find_or_create_monitor(monitors, &monitor_count, "TEMP_TEST");
            AlertContext ctx;
            int result = process_reading(m, value, timestamp, &ctx);
            
            if (result == 1) {
                print_alert(&ctx);
                total_alerts++;
            } else if (result == -1) {
                total_suppressed++;
            }
            
            total_lines++;
        }
    } else {
        /* Procesăm input-ul */
        has_input = true;
        
        do {
            uint64_t timestamp;
            char sensor_id[32];
            double value;
            
            if (parse_csv_line(line, &timestamp, sensor_id, &value)) {
                SensorMonitor *m = find_or_create_monitor(monitors, &monitor_count, sensor_id);
                if (m) {
                    AlertContext ctx;
                    int result = process_reading(m, value, timestamp, &ctx);
                    
                    if (result == 1) {
                        print_alert(&ctx);
                        total_alerts++;
                    } else if (result == -1) {
                        total_suppressed++;
                    }
                }
                total_lines++;
            }
        } while (fgets(line, sizeof(line), stdin));
    }
    
    /* Raport final */
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    RAPORT FINAL                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("%-15s %-10s %-12s %-12s %-10s\n", 
           "Senzor", "Citiri", "Alerte", "Suprimate", "Rată (%)");
    printf("%-15s %-10s %-12s %-12s %-10s\n", 
           "------", "------", "------", "---------", "--------");
    
    for (size_t i = 0; i < monitor_count; i++) {
        SensorMonitor *m = &monitors[i];
        double rate = 0.0;
        if (m->total_readings > 0) {
            rate = (double)(m->total_alerts + m->suppressed_alerts) / m->total_readings * 100;
        }
        
        printf("%-15s %-10zu %-12zu %-12zu %-10.2f\n",
               m->sensor_id,
               m->total_readings,
               m->total_alerts,
               m->suppressed_alerts,
               rate);
    }
    
    printf("\n");
    printf("%s=== SUMAR ===%s\n", COLOR_GREEN, COLOR_RESET);
    printf("Total linii procesate: %zu\n", total_lines);
    printf("Total alerte emise: %zu\n", total_alerts);
    printf("Total alerte suprimate (rate limit): %zu\n", total_suppressed);
    
    if (total_suppressed > 0) {
        printf("\n%sNotă:%s Rate limiting-ul a prevenit %zu alerte potențial redundante.\n",
               COLOR_CYAN, COLOR_RESET, total_suppressed);
    }
    
    printf("\n%s✓ Procesare completă!%s\n\n", COLOR_GREEN, COLOR_RESET);
    
    return 0;
}
