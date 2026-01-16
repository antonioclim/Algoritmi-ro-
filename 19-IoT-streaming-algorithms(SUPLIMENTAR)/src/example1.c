/**
 * =============================================================================
 * SĂPTĂMÂNA 19: ALGORITMI PENTRU IOT ȘI STREAM PROCESSING
 * Exemplu Complet și Demonstrativ
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Simularea senzorilor cu zgomot gaussian
 *   2. Filtre digitale: Moving Average, EMA, Kalman 1D
 *   3. Ferestre temporale: Tumbling și Sliding
 *   4. Detecție anomalii: Z-score și IQR
 *   5. Pipeline complet de procesare senzori
 *   6. Simulare protocol binar IoT
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o example1 example1.c -lm
 * Execuție:  ./example1
 * Valgrind:  valgrind --leak-check=full ./example1
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
 * CONSTANTE ȘI DEFINIȚII
 * =============================================================================
 */

#define MAX_SENSORS 32
#define MAX_WINDOW_SIZE 1000
#define BUFFER_SIZE 256

/* Coduri de culoare ANSI pentru output frumos */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_MAGENTA "\033[0;35m"
#define COLOR_CYAN    "\033[0;36m"
#define COLOR_BOLD    "\033[1m"

/* =============================================================================
 * PARTEA 1: STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Structură pentru citirea unui senzor
 */
typedef struct {
    uint64_t timestamp_ms;      /* Timestamp în milisecunde */
    char sensor_id[32];         /* Identificator senzor */
    double value;               /* Valoarea citită */
    uint8_t quality;            /* Calitatea citirii (0-100) */
} SensorReading;

/**
 * Configurația unui senzor pentru simulare
 */
typedef struct {
    char sensor_id[32];         /* Identificator unic */
    char sensor_type[32];       /* Tip: temperature, humidity, motion */
    double base_value;          /* Valoare de bază */
    double min_val;             /* Valoare minimă */
    double max_val;             /* Valoare maximă */
    double noise_stddev;        /* Deviație standard pentru zgomot */
    double drift_rate;          /* Rată de drift pe secundă */
    double anomaly_prob;        /* Probabilitate anomalie per citire */
} SensorConfig;

/**
 * Moving Average Filter
 */
typedef struct {
    double *buffer;             /* Buffer circular pentru valori */
    size_t window_size;         /* Dimensiunea ferestrei */
    size_t count;               /* Numărul de elemente adăugate */
    size_t head;                /* Index pentru scriere */
    double sum;                 /* Sumă curentă (pentru eficiență) */
} MovingAverage;

/**
 * Exponential Moving Average Filter
 */
typedef struct {
    double alpha;               /* Factor de netezire (0 < α < 1) */
    double ema;                 /* Valoarea EMA curentă */
    bool initialized;           /* Dacă a fost inițializat */
} EMAFilter;

/**
 * Kalman Filter 1D simplificat
 */
typedef struct {
    double estimate;            /* Estimarea curentă */
    double error_estimate;      /* Eroarea estimării (P) */
    double error_measure;       /* Eroarea măsurătorii (R) */
    double process_noise;       /* Zgomotul procesului (Q) */
} Kalman1D;

/**
 * Statistici running pentru Z-score (Welford's algorithm)
 */
typedef struct {
    size_t count;               /* Numărul de elemente */
    double mean;                /* Media curentă */
    double m2;                  /* Suma pătratelor diferențelor */
    double min_val;             /* Valoare minimă */
    double max_val;             /* Valoare maximă */
} RunningStats;

/**
 * Z-Score Detector pentru anomalii
 */
typedef struct {
    RunningStats stats;         /* Statistici running */
    double threshold;           /* Prag Z-score (tipic 2 sau 3) */
    size_t warmup_samples;      /* Număr samples înainte de detecție */
} ZScoreDetector;

/**
 * IQR Detector pentru anomalii
 */
typedef struct {
    double *buffer;             /* Buffer pentru ultimele valori */
    size_t buffer_size;         /* Dimensiune buffer */
    size_t count;               /* Elemente în buffer */
    size_t head;                /* Index scriere */
    double k;                   /* Multiplicator IQR (tipic 1.5) */
} IQRDetector;

/**
 * Fereastră tumbling pentru agregare
 */
typedef struct {
    double *values;             /* Valorile în fereastră */
    uint64_t *timestamps;       /* Timestamps */
    size_t capacity;            /* Capacitate maximă */
    size_t count;               /* Elemente curente */
    uint64_t window_start_ms;   /* Început fereastră */
    uint64_t window_size_ms;    /* Dimensiune fereastră în ms */
} TumblingWindow;

/**
 * Rezultatul agregării unei ferestre
 */
typedef struct {
    uint64_t window_start;      /* Timestamp început */
    uint64_t window_end;        /* Timestamp sfârșit */
    size_t count;               /* Număr elemente */
    double sum;                 /* Suma */
    double mean;                /* Media */
    double min;                 /* Minimum */
    double max;                 /* Maximum */
    double stddev;              /* Deviație standard */
} WindowAggregate;

/**
 * Pachet binar pentru protocol IoT simulat
 */
typedef struct __attribute__((packed)) {
    uint8_t msg_type;           /* Tip mesaj */
    uint32_t sensor_id;         /* ID senzor (numeric) */
    uint64_t timestamp;         /* Timestamp */
    float value;                /* Valoare (float pentru economie) */
    uint16_t checksum;          /* Checksum simplu */
} SensorPacket;

/* =============================================================================
 * PARTEA 2: UTILITĂȚI GENERALE
 * =============================================================================
 */

/**
 * Generează un număr aleator cu distribuție normală (Box-Muller)
 */
double random_gaussian(double mean, double stddev) {
    static bool has_spare = false;
    static double spare;
    
    if (has_spare) {
        has_spare = false;
        return spare * stddev + mean;
    }
    
    double u, v, s;
    do {
        u = (rand() / (double)RAND_MAX) * 2.0 - 1.0;
        v = (rand() / (double)RAND_MAX) * 2.0 - 1.0;
        s = u * u + v * v;
    } while (s >= 1.0 || s == 0.0);
    
    s = sqrt(-2.0 * log(s) / s);
    spare = v * s;
    has_spare = true;
    
    return mean + stddev * u * s;
}

/**
 * Obține timestamp-ul curent în milisecunde (simulat)
 */
static uint64_t simulated_time_ms = 0;

uint64_t get_timestamp_ms(void) {
    return simulated_time_ms;
}

void advance_time_ms(uint64_t delta) {
    simulated_time_ms += delta;
}

/**
 * Afișează separator frumos
 */
void print_separator(const char *title) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  %-61s║\n", title);
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/**
 * Afișează subsecțiune
 */
void print_subsection(const char *title) {
    printf("\n%s--- %s ---%s\n\n", COLOR_CYAN, title, COLOR_RESET);
}

/* =============================================================================
 * PARTEA 3: SIMULARE SENZORI
 * =============================================================================
 */

/**
 * Creează configurație pentru senzor de temperatură
 */
SensorConfig create_temperature_sensor(const char *id) {
    SensorConfig config;
    strncpy(config.sensor_id, id, sizeof(config.sensor_id) - 1);
    strncpy(config.sensor_type, "temperature", sizeof(config.sensor_type) - 1);
    config.base_value = 22.0;       /* 22°C bază */
    config.min_val = -10.0;
    config.max_val = 50.0;
    config.noise_stddev = 0.3;      /* ±0.3°C zgomot */
    config.drift_rate = 0.001;      /* Drift ușor */
    config.anomaly_prob = 0.02;     /* 2% șansă anomalie */
    return config;
}

/**
 * Creează configurație pentru senzor de umiditate
 */
SensorConfig create_humidity_sensor(const char *id) {
    SensorConfig config;
    strncpy(config.sensor_id, id, sizeof(config.sensor_id) - 1);
    strncpy(config.sensor_type, "humidity", sizeof(config.sensor_type) - 1);
    config.base_value = 45.0;       /* 45% bază */
    config.min_val = 0.0;
    config.max_val = 100.0;
    config.noise_stddev = 2.0;      /* ±2% zgomot */
    config.drift_rate = 0.002;
    config.anomaly_prob = 0.01;
    return config;
}

/**
 * Generează o citire de la senzor
 */
SensorReading generate_reading(SensorConfig *config, uint64_t timestamp) {
    SensorReading reading;
    
    reading.timestamp_ms = timestamp;
    strncpy(reading.sensor_id, config->sensor_id, sizeof(reading.sensor_id) - 1);
    
    /* Valoare de bază cu zgomot */
    double value = config->base_value + random_gaussian(0, config->noise_stddev);
    
    /* Adaugă drift temporal */
    value += config->drift_rate * (timestamp / 1000.0);
    
    /* Posibilă anomalie */
    if ((rand() / (double)RAND_MAX) < config->anomaly_prob) {
        /* Anomalie: salt brusc */
        double spike = (rand() % 2 == 0 ? 1 : -1) * 
                       (5.0 + random_gaussian(0, 2.0)) * config->noise_stddev;
        value += spike * 10;
    }
    
    /* Limitare la interval valid */
    if (value < config->min_val) value = config->min_val;
    if (value > config->max_val) value = config->max_val;
    
    reading.value = value;
    reading.quality = 95 + rand() % 6;  /* 95-100 */
    
    return reading;
}

/* =============================================================================
 * PARTEA 4: FILTRE DIGITALE
 * =============================================================================
 */

/**
 * Creează filtru Moving Average
 */
MovingAverage* ma_create(size_t window_size) {
    MovingAverage *ma = (MovingAverage*)malloc(sizeof(MovingAverage));
    if (!ma) return NULL;
    
    ma->buffer = (double*)calloc(window_size, sizeof(double));
    if (!ma->buffer) {
        free(ma);
        return NULL;
    }
    
    ma->window_size = window_size;
    ma->count = 0;
    ma->head = 0;
    ma->sum = 0.0;
    
    return ma;
}

/**
 * Actualizează Moving Average cu o nouă valoare
 */
double ma_update(MovingAverage *ma, double value) {
    /* Scade valoarea veche din sumă (dacă buffer-ul e plin) */
    if (ma->count >= ma->window_size) {
        ma->sum -= ma->buffer[ma->head];
    }
    
    /* Adaugă noua valoare */
    ma->buffer[ma->head] = value;
    ma->sum += value;
    
    /* Actualizează index și contor */
    ma->head = (ma->head + 1) % ma->window_size;
    if (ma->count < ma->window_size) {
        ma->count++;
    }
    
    /* Returnează media */
    return ma->sum / ma->count;
}

/**
 * Eliberează filtrul Moving Average
 */
void ma_free(MovingAverage *ma) {
    if (ma) {
        free(ma->buffer);
        free(ma);
    }
}

/**
 * Creează filtru EMA
 */
EMAFilter* ema_create(double alpha) {
    if (alpha <= 0.0 || alpha >= 1.0) {
        fprintf(stderr, "Eroare: alpha trebuie să fie în (0, 1)\n");
        return NULL;
    }
    
    EMAFilter *ema = (EMAFilter*)malloc(sizeof(EMAFilter));
    if (!ema) return NULL;
    
    ema->alpha = alpha;
    ema->ema = 0.0;
    ema->initialized = false;
    
    return ema;
}

/**
 * Actualizează EMA cu o nouă valoare
 */
double ema_update(EMAFilter *ema, double value) {
    if (!ema->initialized) {
        ema->ema = value;
        ema->initialized = true;
    } else {
        /* EMA_t = α * x_t + (1-α) * EMA_{t-1} */
        ema->ema = ema->alpha * value + (1.0 - ema->alpha) * ema->ema;
    }
    return ema->ema;
}

/**
 * Eliberează filtrul EMA
 */
void ema_free(EMAFilter *ema) {
    free(ema);
}

/**
 * Creează Kalman Filter 1D
 */
Kalman1D* kalman_create(double initial_estimate, double error_estimate, 
                         double error_measure, double process_noise) {
    Kalman1D *k = (Kalman1D*)malloc(sizeof(Kalman1D));
    if (!k) return NULL;
    
    k->estimate = initial_estimate;
    k->error_estimate = error_estimate;
    k->error_measure = error_measure;
    k->process_noise = process_noise;
    
    return k;
}

/**
 * Actualizează Kalman Filter cu o nouă măsurătoare
 */
double kalman_update(Kalman1D *k, double measurement) {
    /* Predicție (simplificată - fără model dinamic) */
    /* error_estimate rămâne la fel + process_noise */
    k->error_estimate += k->process_noise;
    
    /* Update */
    /* Kalman Gain: K = P / (P + R) */
    double kalman_gain = k->error_estimate / (k->error_estimate + k->error_measure);
    
    /* Estimare: x = x + K * (z - x) */
    k->estimate = k->estimate + kalman_gain * (measurement - k->estimate);
    
    /* Update eroare: P = (1 - K) * P */
    k->error_estimate = (1.0 - kalman_gain) * k->error_estimate;
    
    return k->estimate;
}

/**
 * Eliberează Kalman Filter
 */
void kalman_free(Kalman1D *k) {
    free(k);
}

/* =============================================================================
 * PARTEA 5: STATISTICI RUNNING ȘI DETECȚIE ANOMALII
 * =============================================================================
 */

/**
 * Inițializează statistici running
 */
void stats_init(RunningStats *stats) {
    stats->count = 0;
    stats->mean = 0.0;
    stats->m2 = 0.0;
    stats->min_val = DBL_MAX;
    stats->max_val = -DBL_MAX;
}

/**
 * Actualizează statistici cu o nouă valoare (Welford's algorithm)
 */
void stats_update(RunningStats *stats, double value) {
    stats->count++;
    
    /* Welford's online algorithm pentru medie și varianță */
    double delta = value - stats->mean;
    stats->mean += delta / stats->count;
    double delta2 = value - stats->mean;
    stats->m2 += delta * delta2;
    
    /* Min/Max */
    if (value < stats->min_val) stats->min_val = value;
    if (value > stats->max_val) stats->max_val = value;
}

/**
 * Obține deviația standard
 */
double stats_stddev(RunningStats *stats) {
    if (stats->count < 2) return 0.0;
    return sqrt(stats->m2 / (stats->count - 1));
}

/**
 * Creează Z-score detector
 */
ZScoreDetector* zscore_create(double threshold, size_t warmup) {
    ZScoreDetector *z = (ZScoreDetector*)malloc(sizeof(ZScoreDetector));
    if (!z) return NULL;
    
    stats_init(&z->stats);
    z->threshold = threshold;
    z->warmup_samples = warmup;
    
    return z;
}

/**
 * Verifică dacă o valoare este anomalie folosind Z-score
 * Returnează Z-score-ul (0 dacă încă în warmup)
 */
double zscore_check(ZScoreDetector *z, double value, bool *is_anomaly) {
    stats_update(&z->stats, value);
    
    if (z->stats.count < z->warmup_samples) {
        *is_anomaly = false;
        return 0.0;
    }
    
    double stddev = stats_stddev(&z->stats);
    if (stddev < 0.0001) {
        *is_anomaly = false;
        return 0.0;
    }
    
    double zscore = (value - z->stats.mean) / stddev;
    *is_anomaly = fabs(zscore) > z->threshold;
    
    return zscore;
}

/**
 * Eliberează Z-score detector
 */
void zscore_free(ZScoreDetector *z) {
    free(z);
}

/* =============================================================================
 * PARTEA 6: FERESTRE TEMPORALE
 * =============================================================================
 */

/**
 * Creează fereastră tumbling
 */
TumblingWindow* window_create(size_t capacity, uint64_t window_size_ms) {
    TumblingWindow *w = (TumblingWindow*)malloc(sizeof(TumblingWindow));
    if (!w) return NULL;
    
    w->values = (double*)malloc(capacity * sizeof(double));
    w->timestamps = (uint64_t*)malloc(capacity * sizeof(uint64_t));
    if (!w->values || !w->timestamps) {
        free(w->values);
        free(w->timestamps);
        free(w);
        return NULL;
    }
    
    w->capacity = capacity;
    w->count = 0;
    w->window_start_ms = 0;
    w->window_size_ms = window_size_ms;
    
    return w;
}

/**
 * Adaugă valoare în fereastră
 * Returnează true dacă fereastra s-a închis și trebuie agregată
 */
bool window_add(TumblingWindow *w, double value, uint64_t timestamp, 
                WindowAggregate *aggregate) {
    /* Dacă e prima valoare, setează începutul ferestrei */
    if (w->count == 0) {
        w->window_start_ms = timestamp;
    }
    
    /* Verifică dacă fereastra s-a închis */
    if (timestamp >= w->window_start_ms + w->window_size_ms) {
        /* Calculează agregarea */
        if (aggregate && w->count > 0) {
            aggregate->window_start = w->window_start_ms;
            aggregate->window_end = w->window_start_ms + w->window_size_ms;
            aggregate->count = w->count;
            
            /* Calculează statistici */
            double sum = 0.0, min = DBL_MAX, max = -DBL_MAX;
            for (size_t i = 0; i < w->count; i++) {
                sum += w->values[i];
                if (w->values[i] < min) min = w->values[i];
                if (w->values[i] > max) max = w->values[i];
            }
            aggregate->sum = sum;
            aggregate->mean = sum / w->count;
            aggregate->min = min;
            aggregate->max = max;
            
            /* Deviație standard */
            double variance = 0.0;
            for (size_t i = 0; i < w->count; i++) {
                double diff = w->values[i] - aggregate->mean;
                variance += diff * diff;
            }
            aggregate->stddev = (w->count > 1) ? sqrt(variance / (w->count - 1)) : 0.0;
        }
        
        /* Resetează fereastra */
        w->count = 0;
        w->window_start_ms = timestamp;
        
        /* Adaugă valoarea curentă în noua fereastră */
        w->values[w->count] = value;
        w->timestamps[w->count] = timestamp;
        w->count++;
        
        return true;  /* Fereastră închisă */
    }
    
    /* Adaugă în fereastră curentă */
    if (w->count < w->capacity) {
        w->values[w->count] = value;
        w->timestamps[w->count] = timestamp;
        w->count++;
    }
    
    return false;
}

/**
 * Eliberează fereastră
 */
void window_free(TumblingWindow *w) {
    if (w) {
        free(w->values);
        free(w->timestamps);
        free(w);
    }
}

/* =============================================================================
 * PARTEA 7: PROTOCOL BINAR IOT (SIMULARE)
 * =============================================================================
 */

/**
 * Calculează checksum simplu pentru pachet
 */
uint16_t calculate_checksum(const uint8_t *data, size_t len) {
    uint16_t sum = 0;
    for (size_t i = 0; i < len - sizeof(uint16_t); i++) {
        sum += data[i];
    }
    return sum;
}

/**
 * Creează pachet binar din citire senzor
 */
SensorPacket create_packet(SensorReading *reading, uint32_t numeric_id) {
    SensorPacket packet;
    
    packet.msg_type = 0x01;  /* Tip: citire senzor */
    packet.sensor_id = numeric_id;
    packet.timestamp = reading->timestamp_ms;
    packet.value = (float)reading->value;
    
    /* Calculează checksum */
    packet.checksum = calculate_checksum((uint8_t*)&packet, sizeof(packet));
    
    return packet;
}

/**
 * Validează pachet primit
 */
bool validate_packet(SensorPacket *packet) {
    uint16_t expected = calculate_checksum((uint8_t*)packet, sizeof(*packet));
    return packet->checksum == expected;
}

/**
 * Afișează pachet în format hexadecimal
 */
void print_packet_hex(SensorPacket *packet) {
    uint8_t *bytes = (uint8_t*)packet;
    printf("Pachet [%zu bytes]: ", sizeof(*packet));
    for (size_t i = 0; i < sizeof(*packet); i++) {
        printf("%02X ", bytes[i]);
    }
    printf("\n");
}

/* =============================================================================
 * PARTEA 8: PIPELINE COMPLET
 * =============================================================================
 */

/**
 * Structură pentru un canal de procesare senzor
 */
typedef struct {
    SensorConfig config;
    EMAFilter *filter;
    ZScoreDetector *anomaly_detector;
    TumblingWindow *window;
    size_t readings_processed;
    size_t anomalies_detected;
} SensorChannel;

/**
 * Creează canal de procesare pentru un senzor
 */
SensorChannel* channel_create(SensorConfig config, double ema_alpha, 
                               double zscore_threshold, uint64_t window_ms) {
    SensorChannel *ch = (SensorChannel*)malloc(sizeof(SensorChannel));
    if (!ch) return NULL;
    
    ch->config = config;
    ch->filter = ema_create(ema_alpha);
    ch->anomaly_detector = zscore_create(zscore_threshold, 30);
    ch->window = window_create(1000, window_ms);
    ch->readings_processed = 0;
    ch->anomalies_detected = 0;
    
    if (!ch->filter || !ch->anomaly_detector || !ch->window) {
        ema_free(ch->filter);
        zscore_free(ch->anomaly_detector);
        window_free(ch->window);
        free(ch);
        return NULL;
    }
    
    return ch;
}

/**
 * Procesează o citire prin pipeline
 */
void channel_process(SensorChannel *ch, SensorReading reading, 
                     bool verbose, WindowAggregate *agg_out) {
    ch->readings_processed++;
    
    /* 1. Filtrare */
    double filtered = ema_update(ch->filter, reading.value);
    
    /* 2. Detecție anomalie */
    bool is_anomaly;
    double zscore = zscore_check(ch->anomaly_detector, filtered, &is_anomaly);
    
    if (is_anomaly) {
        ch->anomalies_detected++;
        if (verbose) {
            printf("%s[ANOMALIE]%s Senzor %s: valoare=%.2f, filtrat=%.2f, z=%.2f\n",
                   COLOR_RED, COLOR_RESET, reading.sensor_id,
                   reading.value, filtered, zscore);
        }
    }
    
    /* 3. Agregare în fereastră */
    WindowAggregate agg;
    if (window_add(ch->window, filtered, reading.timestamp_ms, &agg)) {
        if (verbose) {
            printf("%s[FEREASTRĂ]%s Senzor %s: count=%zu, mean=%.2f, stddev=%.3f\n",
                   COLOR_BLUE, COLOR_RESET, reading.sensor_id,
                   agg.count, agg.mean, agg.stddev);
        }
        if (agg_out) *agg_out = agg;
    }
}

/**
 * Eliberează canal de procesare
 */
void channel_free(SensorChannel *ch) {
    if (ch) {
        ema_free(ch->filter);
        zscore_free(ch->anomaly_detector);
        window_free(ch->window);
        free(ch);
    }
}

/* =============================================================================
 * PARTEA 9: DEMONSTRAȚII
 * =============================================================================
 */

/**
 * Demo 1: Comparație filtre
 */
void demo_filters(void) {
    print_separator("DEMO 1: COMPARAȚIE FILTRE (MA vs EMA vs Kalman)");
    
    /* Date cu zgomot */
    double signal[] = {20.0, 20.5, 21.0, 20.8, 35.0, 21.2, 20.9, 21.1, 20.7, 21.0};
    size_t n = sizeof(signal) / sizeof(signal[0]);
    
    MovingAverage *ma = ma_create(3);
    EMAFilter *ema = ema_create(0.3);
    Kalman1D *kalman = kalman_create(20.0, 1.0, 1.0, 0.1);
    
    printf("%-10s %-10s %-10s %-10s %-10s\n", "Input", "MA(3)", "EMA(0.3)", "Kalman", "Notă");
    printf("%-10s %-10s %-10s %-10s %-10s\n", "-----", "-----", "--------", "------", "----");
    
    for (size_t i = 0; i < n; i++) {
        double ma_val = ma_update(ma, signal[i]);
        double ema_val = ema_update(ema, signal[i]);
        double kalman_val = kalman_update(kalman, signal[i]);
        
        const char *note = (signal[i] > 30) ? "← SPIKE" : "";
        
        printf("%-10.2f %-10.2f %-10.2f %-10.2f %s\n", 
               signal[i], ma_val, ema_val, kalman_val, note);
    }
    
    printf("\n%sObservație:%s Kalman și EMA reacționează mai rapid la spike\n",
           COLOR_CYAN, COLOR_RESET);
    printf("dar revin mai repede la normal după ce spike-ul trece.\n");
    
    ma_free(ma);
    ema_free(ema);
    kalman_free(kalman);
}

/**
 * Demo 2: Detecție anomalii
 */
void demo_anomaly_detection(void) {
    print_separator("DEMO 2: DETECȚIE ANOMALII (Z-Score)");
    
    ZScoreDetector *detector = zscore_create(2.5, 10);
    
    printf("Simulăm 50 de citiri cu câteva anomalii injectate...\n\n");
    printf("%-8s %-10s %-10s %-10s %-12s\n", "Index", "Valoare", "Z-Score", "Anomalie", "Notă");
    printf("%-8s %-10s %-10s %-10s %-12s\n", "-----", "-------", "-------", "--------", "----");
    
    for (int i = 0; i < 50; i++) {
        double value = random_gaussian(22.0, 0.5);
        const char *note = "";
        
        /* Injectăm anomalii la pozițiile 15, 30, 45 */
        if (i == 15) { value = 28.0; note = "← spike +"; }
        if (i == 30) { value = 16.0; note = "← spike -"; }
        if (i == 45) { value = 30.0; note = "← spike ++"; }
        
        bool is_anomaly;
        double zscore = zscore_check(detector, value, &is_anomaly);
        
        if (i < 12 || is_anomaly || note[0] != '\0') {
            printf("%-8d %-10.2f %-10.2f %-10s %s%s%s\n", 
                   i, value, zscore,
                   is_anomaly ? "DA" : "nu",
                   is_anomaly ? COLOR_RED : "",
                   note,
                   is_anomaly ? COLOR_RESET : "");
        }
    }
    
    printf("\n%sNotă:%s Primele 10 citiri sunt pentru warmup (nu se detectează anomalii).\n",
           COLOR_CYAN, COLOR_RESET);
    
    zscore_free(detector);
}

/**
 * Demo 3: Pipeline complet
 */
void demo_full_pipeline(void) {
    print_separator("DEMO 3: PIPELINE COMPLET IOT");
    
    /* Creăm senzori */
    SensorConfig temp_config = create_temperature_sensor("TEMP_001");
    SensorConfig hum_config = create_humidity_sensor("HUM_001");
    
    /* Creăm canale de procesare */
    SensorChannel *temp_ch = channel_create(temp_config, 0.2, 2.5, 5000);  /* 5s window */
    SensorChannel *hum_ch = channel_create(hum_config, 0.3, 2.5, 5000);
    
    printf("Simulăm 60 de secunde de citiri (1 citire/secundă per senzor)...\n\n");
    
    /* Simulăm 60 de secunde */
    for (int i = 0; i < 60; i++) {
        uint64_t timestamp = i * 1000;  /* ms */
        
        SensorReading temp_reading = generate_reading(&temp_config, timestamp);
        SensorReading hum_reading = generate_reading(&hum_config, timestamp);
        
        WindowAggregate agg;
        channel_process(temp_ch, temp_reading, true, &agg);
        channel_process(hum_ch, hum_reading, false, &agg);
    }
    
    /* Statistici finale */
    printf("\n%s=== STATISTICI FINALE ===%s\n", COLOR_GREEN, COLOR_RESET);
    printf("Senzor TEMP_001: %zu citiri procesate, %zu anomalii detectate\n",
           temp_ch->readings_processed, temp_ch->anomalies_detected);
    printf("Senzor HUM_001:  %zu citiri procesate, %zu anomalii detectate\n",
           hum_ch->readings_processed, hum_ch->anomalies_detected);
    
    channel_free(temp_ch);
    channel_free(hum_ch);
}

/**
 * Demo 4: Protocol binar
 */
void demo_binary_protocol(void) {
    print_separator("DEMO 4: PROTOCOL BINAR IOT");
    
    SensorReading reading = {
        .timestamp_ms = 1705400000000ULL,  /* Un timestamp realist */
        .sensor_id = "TEMP_001",
        .value = 23.45,
        .quality = 98
    };
    
    SensorPacket packet = create_packet(&reading, 1);
    
    printf("Citire originală:\n");
    printf("  Senzor: %s\n", reading.sensor_id);
    printf("  Timestamp: %lu ms\n", reading.timestamp_ms);
    printf("  Valoare: %.2f\n", reading.value);
    printf("\n");
    
    printf("Pachet binar (%zu bytes):\n", sizeof(packet));
    print_packet_hex(&packet);
    printf("\n");
    
    printf("Validare pachet: %s\n", 
           validate_packet(&packet) ? "✓ VALID" : "✗ INVALID");
    
    printf("\n%sNotă:%s Protocolul binar economisește bandă comparativ cu JSON.\n",
           COLOR_CYAN, COLOR_RESET);
    printf("JSON: ~80 bytes vs Binar: %zu bytes (reducere ~%.0f%%)\n",
           sizeof(packet), (1.0 - sizeof(packet) / 80.0) * 100);
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(void) {
    /* Inițializare generator de numere aleatoare */
    srand((unsigned int)time(NULL));
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 19: ALGORITMI PENTRU IOT ȘI STREAM PROCESSING                   ║\n");
    printf("║     Exemplu Demonstrativ Complet                                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("%sAcest exemplu demonstrează:%s\n", COLOR_BOLD, COLOR_RESET);
    printf("  1. Comparație filtre digitale (MA, EMA, Kalman)\n");
    printf("  2. Detecție anomalii cu Z-score\n");
    printf("  3. Pipeline complet de procesare IoT\n");
    printf("  4. Protocol binar pentru economie de bandă\n");
    printf("\n");
    
    /* Demonstrații */
    demo_filters();
    demo_anomaly_detection();
    demo_full_pipeline();
    demo_binary_protocol();
    
    print_separator("TOATE DEMONSTRAȚIILE COMPLETATE CU SUCCES!");
    
    printf("%sConcluzii:%s\n", COLOR_BOLD, COLOR_RESET);
    printf("• Filtrele EMA și Kalman sunt preferate pentru memorie O(1)\n");
    printf("• Z-score cu running stats permite detecție în streaming\n");
    printf("• Ferestrele temporale permit agregare cu memorie limitată\n");
    printf("• Protocoalele binare reduc semnificativ consumul de bandă\n");
    printf("\n");
    
    return 0;
}
