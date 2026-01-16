/**
 * ============================================================================
 * SOLUȚIE EXERCISE 1: Smart Building Temperature Monitor
 * ============================================================================
 * 
 * Săptămâna 19: Algoritmi pentru IoT și Stream Processing
 * Curs: Algoritmi și Tehnici de Programare, ASE București
 * 
 * DESCRIERE:
 *   Implementare completă a sistemului de monitorizare temperatură pentru
 *   o clădire inteligentă. Demonstrează: EMA filtering, tumbling windows,
 *   detecție threshold, și generare rapoarte.
 * 
 * COMPILARE:
 *   gcc -Wall -Wextra -std=c11 -O2 -o exercise1_sol exercise1_sol.c -lm
 * 
 * RULARE:
 *   ./exercise1_sol [input.csv]
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

#define MAX_SENSORS          10
#define SENSOR_ID_LEN        32
#define WINDOW_SIZE_MS       300000    // 5 minute
#define REPORT_INTERVAL_MS   600000    // 10 minute

// Praguri temperatură (°C)
#define TEMP_COMFORT_LOW     18.0
#define TEMP_COMFORT_HIGH    26.0
#define TEMP_CRITICAL_LOW    15.0
#define TEMP_CRITICAL_HIGH   30.0

// Parametri algoritmi
#define EMA_ALPHA            0.2
#define ZSCORE_THRESHOLD     2.5
#define WARMUP_SAMPLES       20

/* ============================================================================
 * STRUCTURI DE DATE
 * ============================================================================ */

/**
 * Filtru Exponential Moving Average
 */
typedef struct {
    double alpha;
    double value;
    bool initialized;
} EMAFilter;

/**
 * Detector anomalii Z-score cu Welford
 */
typedef struct {
    double mean;
    double M2;
    uint64_t count;
    double threshold;
} ZScoreDetector;

/**
 * Fereastră Tumbling pentru agregare
 */
typedef struct {
    uint64_t window_size_ms;
    uint64_t window_start;
    double sum;
    double min;
    double max;
    double M2;            // Pentru varianță
    double running_mean;  // Media running pentru Welford
    int count;
} TumblingWindow;

/**
 * Statistici fereastră emisă
 */
typedef struct {
    uint64_t window_start;
    uint64_t window_end;
    double avg;
    double min;
    double max;
    double stddev;
    int sample_count;
} WindowStats;

/**
 * Nivel alertă
 */
typedef enum {
    ALERT_NONE = 0,
    ALERT_INFO,
    ALERT_WARNING,
    ALERT_CRITICAL,
    ALERT_ANOMALY
} AlertLevel;

/**
 * Context complet pentru un senzor
 */
typedef struct {
    char sensor_id[SENSOR_ID_LEN];
    bool active;
    
    // Filtre și detectoare
    EMAFilter filter;
    ZScoreDetector detector;
    TumblingWindow window;
    
    // Statistici cumulative
    int total_readings;
    int windows_completed;
    int warnings_count;
    int critical_count;
    int anomalies_count;
    
    double global_min;
    double global_max;
    double global_sum;
} SensorContext;

/**
 * Sistem monitorizare complet
 */
typedef struct {
    SensorContext sensors[MAX_SENSORS];
    int sensor_count;
    
    uint64_t start_time;
    uint64_t last_report_time;
    int total_readings;
    int total_alerts;
} MonitoringSystem;

/* ============================================================================
 * IMPLEMENTĂRI EMA
 * ============================================================================ */

void ema_init(EMAFilter* f, double alpha) {
    f->alpha = alpha;
    f->value = 0.0;
    f->initialized = false;
}

double ema_update(EMAFilter* f, double value) {
    if (!f->initialized) {
        f->value = value;
        f->initialized = true;
    } else {
        // EMA(t) = α × x(t) + (1-α) × EMA(t-1)
        f->value = f->alpha * value + (1.0 - f->alpha) * f->value;
    }
    return f->value;
}

/* ============================================================================
 * IMPLEMENTĂRI Z-SCORE DETECTOR
 * ============================================================================ */

void zscore_init(ZScoreDetector* d, double threshold) {
    d->mean = 0.0;
    d->M2 = 0.0;
    d->count = 0;
    d->threshold = threshold;
}

/**
 * Verifică dacă valoarea este anomalie și actualizează statisticile.
 * Folosește algoritmul Welford pentru stabilitate numerică.
 * 
 * @return Z-score-ul calculat (0 dacă în warmup)
 */
double zscore_update(ZScoreDetector* d, double value, bool* is_anomaly) {
    d->count++;
    
    // Welford's online algorithm
    double delta = value - d->mean;
    d->mean += delta / d->count;
    double delta2 = value - d->mean;
    d->M2 += delta * delta2;
    
    *is_anomaly = false;
    
    // Warmup period
    if (d->count < WARMUP_SAMPLES) {
        return 0.0;
    }
    
    // Calculăm deviația standard
    double variance = d->M2 / d->count;
    double stddev = sqrt(variance);
    
    if (stddev < 1e-10) {
        return 0.0;
    }
    
    // Z-score
    double zscore = fabs(value - d->mean) / stddev;
    *is_anomaly = (zscore > d->threshold);
    
    return zscore;
}

double zscore_get_stddev(const ZScoreDetector* d) {
    if (d->count < 2) return 0.0;
    return sqrt(d->M2 / d->count);
}

/* ============================================================================
 * IMPLEMENTĂRI TUMBLING WINDOW
 * ============================================================================ */

void window_init(TumblingWindow* w, uint64_t size_ms) {
    w->window_size_ms = size_ms;
    w->window_start = 0;
    w->sum = 0.0;
    w->min = DBL_MAX;
    w->max = -DBL_MAX;
    w->M2 = 0.0;
    w->running_mean = 0.0;
    w->count = 0;
}

void window_reset(TumblingWindow* w, uint64_t new_start) {
    w->window_start = new_start;
    w->sum = 0.0;
    w->min = DBL_MAX;
    w->max = -DBL_MAX;
    w->M2 = 0.0;
    w->running_mean = 0.0;
    w->count = 0;
}

/**
 * Adaugă valoare la fereastră.
 * 
 * @return true dacă fereastra s-a închis și stats conține rezultatele
 */
bool window_add(TumblingWindow* w, double value, uint64_t timestamp, WindowStats* stats) {
    bool window_closed = false;
    
    // Prima valoare - inițializare window_start
    if (w->count == 0 && w->window_start == 0) {
        // Aliniere la granulație
        w->window_start = timestamp - (timestamp % w->window_size_ms);
    }
    
    // Verificăm dacă timestamp-ul e în fereastra curentă
    if (timestamp >= w->window_start + w->window_size_ms) {
        // Emitem statisticile ferestrei curente (dacă avem date)
        if (w->count > 0) {
            stats->window_start = w->window_start;
            stats->window_end = w->window_start + w->window_size_ms;
            stats->avg = w->sum / w->count;
            stats->min = w->min;
            stats->max = w->max;
            stats->sample_count = w->count;
            
            // Deviația standard din Welford
            if (w->count > 1) {
                stats->stddev = sqrt(w->M2 / w->count);
            } else {
                stats->stddev = 0.0;
            }
            
            window_closed = true;
        }
        
        // Resetăm pentru noua fereastră
        uint64_t new_start = timestamp - (timestamp % w->window_size_ms);
        window_reset(w, new_start);
    }
    
    // Adăugăm valoarea la fereastra curentă
    w->count++;
    w->sum += value;
    
    if (value < w->min) w->min = value;
    if (value > w->max) w->max = value;
    
    // Welford pentru varianță
    double delta = value - w->running_mean;
    w->running_mean += delta / w->count;
    double delta2 = value - w->running_mean;
    w->M2 += delta * delta2;
    
    return window_closed;
}

/* ============================================================================
 * VERIFICARE PRAGURI
 * ============================================================================ */

AlertLevel check_temperature_thresholds(double filtered_value, bool is_anomaly) {
    if (filtered_value < TEMP_CRITICAL_LOW || filtered_value > TEMP_CRITICAL_HIGH) {
        return ALERT_CRITICAL;
    }
    
    if (is_anomaly) {
        return ALERT_ANOMALY;
    }
    
    if (filtered_value < TEMP_COMFORT_LOW || filtered_value > TEMP_COMFORT_HIGH) {
        return ALERT_WARNING;
    }
    
    return ALERT_NONE;
}

const char* alert_level_to_string(AlertLevel level) {
    switch (level) {
        case ALERT_NONE:     return "OK";
        case ALERT_INFO:     return "INFO";
        case ALERT_WARNING:  return "WARNING";
        case ALERT_CRITICAL: return "CRITICAL";
        case ALERT_ANOMALY:  return "ANOMALY";
        default:             return "UNKNOWN";
    }
}

/* ============================================================================
 * CONTEXT SENZOR
 * ============================================================================ */

void sensor_init(SensorContext* ctx, const char* sensor_id) {
    strncpy(ctx->sensor_id, sensor_id, SENSOR_ID_LEN - 1);
    ctx->sensor_id[SENSOR_ID_LEN - 1] = '\0';
    ctx->active = true;
    
    ema_init(&ctx->filter, EMA_ALPHA);
    zscore_init(&ctx->detector, ZSCORE_THRESHOLD);
    window_init(&ctx->window, WINDOW_SIZE_MS);
    
    ctx->total_readings = 0;
    ctx->windows_completed = 0;
    ctx->warnings_count = 0;
    ctx->critical_count = 0;
    ctx->anomalies_count = 0;
    
    ctx->global_min = DBL_MAX;
    ctx->global_max = -DBL_MAX;
    ctx->global_sum = 0.0;
}

/* ============================================================================
 * SISTEM MONITORIZARE
 * ============================================================================ */

void system_init(MonitoringSystem* sys) {
    memset(sys, 0, sizeof(MonitoringSystem));
    sys->start_time = 0;
    sys->last_report_time = 0;
}

SensorContext* system_get_sensor(MonitoringSystem* sys, const char* sensor_id) {
    // Caută senzor existent
    for (int i = 0; i < sys->sensor_count; i++) {
        if (strcmp(sys->sensors[i].sensor_id, sensor_id) == 0) {
            return &sys->sensors[i];
        }
    }
    
    // Creează senzor nou
    if (sys->sensor_count < MAX_SENSORS) {
        SensorContext* ctx = &sys->sensors[sys->sensor_count];
        sensor_init(ctx, sensor_id);
        sys->sensor_count++;
        return ctx;
    }
    
    return NULL;  // Limită atinsă
}

/**
 * Procesează o citire de la un senzor.
 */
void system_process_reading(MonitoringSystem* sys, 
                           const char* sensor_id,
                           double value,
                           uint64_t timestamp) {
    
    // Inițializare timp start
    if (sys->start_time == 0) {
        sys->start_time = timestamp;
        sys->last_report_time = timestamp;
    }
    
    // Găsește sau creează context senzor
    SensorContext* ctx = system_get_sensor(sys, sensor_id);
    if (!ctx) {
        fprintf(stderr, "Warning: Nu se poate adăuga senzor %s\n", sensor_id);
        return;
    }
    
    // 1. Filtrare EMA
    double filtered = ema_update(&ctx->filter, value);
    
    // 2. Detecție anomalii
    bool is_anomaly;
    double zscore = zscore_update(&ctx->detector, value, &is_anomaly);
    
    // 3. Verificare praguri
    AlertLevel alert = check_temperature_thresholds(filtered, is_anomaly);
    
    // 4. Actualizare statistici
    ctx->total_readings++;
    sys->total_readings++;
    
    if (value < ctx->global_min) ctx->global_min = value;
    if (value > ctx->global_max) ctx->global_max = value;
    ctx->global_sum += value;
    
    if (alert == ALERT_WARNING) ctx->warnings_count++;
    if (alert == ALERT_CRITICAL) ctx->critical_count++;
    if (is_anomaly) ctx->anomalies_count++;
    if (alert != ALERT_NONE) sys->total_alerts++;
    
    // 5. Agregare în fereastră
    WindowStats stats;
    if (window_add(&ctx->window, filtered, timestamp, &stats)) {
        ctx->windows_completed++;
        
        // Afișare statistici fereastră
        printf("[WINDOW] %s: avg=%.2f°C, min=%.2f, max=%.2f, σ=%.3f, n=%d\n",
               sensor_id, stats.avg, stats.min, stats.max, 
               stats.stddev, stats.sample_count);
    }
    
    // 6. Afișare alertă (dacă există)
    if (alert != ALERT_NONE) {
        printf("[%s] %s: raw=%.2f°C, filtered=%.2f°C, zscore=%.2f\n",
               alert_level_to_string(alert), sensor_id, 
               value, filtered, zscore);
    }
}

/**
 * Generează raport periodic.
 */
void system_generate_report(MonitoringSystem* sys, uint64_t timestamp) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║           RAPORT MONITORIZARE TEMPERATURĂ                    ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    
    time_t start_sec = sys->start_time / 1000;
    time_t end_sec = timestamp / 1000;
    
    printf("║ Perioadă: %s", ctime(&start_sec));
    printf("║        → %s", ctime(&end_sec));
    printf("║ Senzori activi: %d                                           ║\n", 
           sys->sensor_count);
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    
    for (int i = 0; i < sys->sensor_count; i++) {
        SensorContext* ctx = &sys->sensors[i];
        
        double avg = ctx->total_readings > 0 ? 
                     ctx->global_sum / ctx->total_readings : 0.0;
        double stddev = zscore_get_stddev(&ctx->detector);
        
        printf("║ SENZOR: %-20s                              ║\n", ctx->sensor_id);
        printf("║   Citiri: %-6d  Ferestre: %-4d                           ║\n",
               ctx->total_readings, ctx->windows_completed);
        printf("║   Temp: min=%.1f°C, max=%.1f°C, avg=%.1f°C, σ=%.2f°C     ║\n",
               ctx->global_min, ctx->global_max, avg, stddev);
        printf("║   Alerte: %d warning, %d critical, %d anomalii            ║\n",
               ctx->warnings_count, ctx->critical_count, ctx->anomalies_count);
        printf("╟──────────────────────────────────────────────────────────────╢\n");
    }
    
    printf("║ TOTAL: %d citiri, %d alerte                                  ║\n",
           sys->total_readings, sys->total_alerts);
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
}

/* ============================================================================
 * GENERARE DATE TEST
 * ============================================================================ */

/**
 * Generează date simulate pentru testare.
 */
void generate_test_data(MonitoringSystem* sys, int n_samples) {
    srand(42);  // Seed pentru reproducibilitate
    
    const char* sensors[] = {"room_101", "room_102", "room_103", "lobby", "server_room"};
    int n_sensors = 5;
    
    uint64_t timestamp = 1704067200000;  // 2024-01-01 08:00:00
    
    printf("Generare %d citiri de test...\n\n", n_samples * n_sensors);
    
    for (int i = 0; i < n_samples; i++) {
        for (int s = 0; s < n_sensors; s++) {
            // Temperatură de bază cu variație
            double base = 22.0 + 2.0 * sin(2.0 * M_PI * i / 100);
            
            // Zgomot Gaussian (Box-Muller)
            double u1 = (rand() + 1.0) / (RAND_MAX + 2.0);
            double u2 = (rand() + 1.0) / (RAND_MAX + 2.0);
            double noise = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2) * 0.5;
            
            double value = base + noise;
            
            // Injectare anomalii rare
            if (rand() % 100 < 2) {  // 2% anomalii
                value += (rand() % 2 ? 1 : -1) * (8 + rand() % 5);
            }
            
            // Server room e mai cald
            if (s == 4) value += 3.0;
            
            system_process_reading(sys, sensors[s], value, timestamp);
        }
        
        timestamp += 1000;  // +1 secundă
        
        // Raport periodic
        if ((i + 1) % 600 == 0) {  // La fiecare 10 minute simulate
            system_generate_report(sys, timestamp);
        }
    }
}

/* ============================================================================
 * PROCESARE FIȘIER CSV
 * ============================================================================ */

int process_csv_file(MonitoringSystem* sys, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Eroare: Nu se poate deschide %s\n", filename);
        return -1;
    }
    
    char line[256];
    int line_num = 0;
    
    // Skip header
    if (fgets(line, sizeof(line), f) == NULL) {
        fclose(f);
        return -1;
    }
    
    while (fgets(line, sizeof(line), f)) {
        line_num++;
        
        uint64_t timestamp;
        char sensor_id[SENSOR_ID_LEN];
        double value;
        
        // Parse CSV: timestamp,sensor_id,temperature
        if (sscanf(line, "%lu,%31[^,],%lf", &timestamp, sensor_id, &value) == 3) {
            system_process_reading(sys, sensor_id, value, timestamp);
            
            // Raport periodic
            if (sys->total_readings % 1000 == 0) {
                system_generate_report(sys, timestamp);
            }
        } else {
            fprintf(stderr, "Warning: Linie invalidă %d: %s", line_num, line);
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
    printf("║     SMART BUILDING TEMPERATURE MONITOR - SOLUȚIE            ║\n");
    printf("║     ATP Week 19 - IoT Stream Processing                     ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
    
    MonitoringSystem sys;
    system_init(&sys);
    
    if (argc > 1) {
        // Procesare fișier CSV
        printf("Procesare fișier: %s\n\n", argv[1]);
        if (process_csv_file(&sys, argv[1]) < 0) {
            return 1;
        }
    } else {
        // Generare date test
        printf("Mod demo - generare date test\n\n");
        generate_test_data(&sys, 1000);  // 1000 citiri per senzor
    }
    
    // Raport final
    printf("\n=== RAPORT FINAL ===\n");
    system_generate_report(&sys, sys.start_time + 3600000);  // +1 oră
    
    return 0;
}
