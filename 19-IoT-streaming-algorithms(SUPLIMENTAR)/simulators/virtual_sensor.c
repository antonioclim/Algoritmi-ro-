/**
 * ============================================================================
 * VIRTUAL SENSOR SIMULATOR - Generare Date IoT Realiste
 * ============================================================================
 * 
 * Săptămâna 19: Algoritmi pentru IoT și Stream Processing
 * Curs: Algoritmi și Tehnici de Programare, ASE București
 * 
 * DESCRIERE:
 *   Simulator complet pentru senzori IoT care generează date realiste
 *   și le publică prin MQTT. Suportă ambele backend-uri:
 *   - Paho MQTT C (broker real)
 *   - POSIX Simulation (fără dependențe)
 * 
 * TIPURI DE SENZORI:
 *   1. Temperatură - variație sinusoidală + zgomot Gaussian
 *   2. Umiditate - corelată invers cu temperatura
 *   3. Mișcare PIR - evenimente binare cu pattern temporal
 *   4. Lumină - ciclu zi/noapte simulat
 *   5. Presiune atmosferică - variație lentă
 * 
 * CARACTERISTICI REALISTE:
 *   - Zgomot Gaussian (Box-Muller transform)
 *   - Drift senzor în timp
 *   - Spike-uri aleatorii (anomalii)
 *   - Corelații între senzori (temp-umiditate)
 *   - Latență variabilă de rețea
 * 
 * PROTOCOL OUTPUT:
 *   JSON simplificat pentru ușurință în parsare:
 *   {"sensor_id":"temp_001","value":23.5,"unit":"C","ts":1704067200}
 * 
 * COMPILARE:
 *   # Cu simulare POSIX:
 *   gcc -o virtual_sensor virtual_sensor.c mqtt_posix.c -lpthread -lrt -lm
 *   
 *   # Cu Paho MQTT:
 *   gcc -DUSE_PAHO_MQTT -o virtual_sensor virtual_sensor.c mqtt_paho.c \
 *       -lpaho-mqtt3c -lpthread -lm
 * 
 * UTILIZARE:
 *   ./virtual_sensor [opțiuni]
 *   
 *   Opțiuni:
 *     -b <host>     Adresa broker (default: localhost)
 *     -p <port>     Port broker (default: 1883)
 *     -i <ms>       Interval publicare (default: 1000)
 *     -n <count>    Număr mesaje (default: infinit)
 *     -t <topic>    Topic prefix (default: sensors/)
 *     -a <prob>     Probabilitate anomalie 0-1 (default: 0.02)
 *     -s <seed>     Seed random (default: time)
 *     -v            Verbose mode
 *     -h            Help
 * 
 * AUTOR: Generated for ASE-CSIE ATP Course
 * VERSIUNE: 1.0 (Ianuarie 2026)
 * ============================================================================
 */

#define _POSIX_C_SOURCE 200809L

#include "mqtt_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>

/* ============================================================================
 * CONSTANTE ȘI CONFIGURARE
 * ============================================================================ */

#define MAX_SENSORS         10
#define MAX_TOPIC_LENGTH    128
#define MAX_PAYLOAD_LENGTH  512

#define DEFAULT_BROKER      "localhost"
#define DEFAULT_PORT        1883
#define DEFAULT_INTERVAL_MS 1000
#define DEFAULT_TOPIC_PREFIX "sensors/"
#define DEFAULT_ANOMALY_PROB 0.02

/* Parametri fizici realiști */
#define TEMP_BASE           22.0    /* Temperatură bază (°C) */
#define TEMP_AMPLITUDE      3.0     /* Amplitudine variație zilnică */
#define TEMP_NOISE_STDDEV   0.5     /* Deviație standard zgomot */
#define TEMP_SPIKE_MAGNITUDE 15.0   /* Magnitudine spike anomalie */

#define HUMIDITY_BASE       50.0    /* Umiditate bază (%) */
#define HUMIDITY_NOISE_STDDEV 2.0   /* Zgomot umiditate */
#define HUMIDITY_TEMP_COEF  -1.5    /* Coeficient corelație temp-umiditate */

#define PRESSURE_BASE       1013.25 /* Presiune bază (hPa) */
#define PRESSURE_NOISE_STDDEV 0.5   /* Zgomot presiune */

#define LIGHT_DAY_VALUE     800.0   /* Lumină zi (lux) */
#define LIGHT_NIGHT_VALUE   10.0    /* Lumină noapte (lux) */

/* ============================================================================
 * CULORI ANSI
 * ============================================================================ */

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

/* ============================================================================
 * TIPURI DE DATE
 * ============================================================================ */

/** Tipuri de senzori suportați */
typedef enum {
    SENSOR_TEMPERATURE,
    SENSOR_HUMIDITY,
    SENSOR_MOTION,
    SENSOR_LIGHT,
    SENSOR_PRESSURE,
    SENSOR_TYPE_COUNT
} SensorType;

/** Unități de măsură */
static const char* SENSOR_UNITS[] = {
    "C",        /* Celsius pentru temperatură */
    "%",        /* Procent pentru umiditate */
    "bool",     /* Boolean pentru mișcare */
    "lux",      /* Lux pentru lumină */
    "hPa"       /* Hectopascali pentru presiune */
};

/** Nume tipuri senzori */
static const char* SENSOR_TYPE_NAMES[] = {
    "temperature",
    "humidity",
    "motion",
    "light",
    "pressure"
};

/** Structură senzor */
typedef struct {
    char id[32];                    /**< ID unic senzor */
    SensorType type;                /**< Tip senzor */
    char topic[MAX_TOPIC_LENGTH];   /**< Topic MQTT complet */
    
    /* Parametri simulare */
    double base_value;              /**< Valoare de bază */
    double noise_stddev;            /**< Deviație standard zgomot */
    double drift;                   /**< Drift acumulat */
    double drift_rate;              /**< Rata de drift per secundă */
    
    /* Stare curentă */
    double last_value;              /**< Ultima valoare generată */
    uint64_t last_publish_ms;       /**< Timestamp ultima publicare */
    uint64_t message_count;         /**< Număr mesaje publicate */
    
    /* Pentru senzori corelați */
    int correlation_sensor_idx;     /**< Index senzor corelat (-1 dacă nu) */
    double correlation_coef;        /**< Coeficient corelație */
} VirtualSensor;

/** Configurație simulator */
typedef struct {
    char broker_address[256];
    int broker_port;
    int interval_ms;
    int message_count;              /**< 0 = infinit */
    char topic_prefix[MAX_TOPIC_LENGTH];
    double anomaly_probability;
    unsigned int random_seed;
    bool verbose;
    
    /* Senzori */
    VirtualSensor sensors[MAX_SENSORS];
    int sensor_count;
} SimulatorConfig;

/* ============================================================================
 * VARIABILE GLOBALE
 * ============================================================================ */

static volatile sig_atomic_t g_running = 1;
static MqttClient g_client = NULL;
static SimulatorConfig g_config;

/* Stare generator random (pentru Box-Muller) */
static bool g_has_spare = false;
static double g_spare_value = 0.0;

/* ============================================================================
 * FUNCȚII MATEMATICE
 * ============================================================================ */

/**
 * Generează număr aleatoriu Gaussian folosind transformarea Box-Muller
 * 
 * Metoda Box-Muller transformă două numere uniforme U(0,1) în
 * două numere cu distribuție normală N(0,1):
 * 
 *   Z1 = √(-2 ln U1) × cos(2π U2)
 *   Z2 = √(-2 ln U1) × sin(2π U2)
 * 
 * @param mean Media distribuției
 * @param stddev Deviația standard
 * @return Valoare aleatorie N(mean, stddev²)
 */
static double gaussian_random(double mean, double stddev) {
    if (g_has_spare) {
        g_has_spare = false;
        return mean + stddev * g_spare_value;
    }
    
    double u1, u2;
    do {
        u1 = (double)rand() / RAND_MAX;
        u2 = (double)rand() / RAND_MAX;
    } while (u1 <= 1e-10);  /* Evită log(0) */
    
    double magnitude = sqrt(-2.0 * log(u1));
    double angle = 2.0 * M_PI * u2;
    
    g_spare_value = magnitude * sin(angle);
    g_has_spare = true;
    
    return mean + stddev * magnitude * cos(angle);
}

/**
 * Generează spike (anomalie) cu probabilitate dată
 * 
 * @param probability Probabilitate spike (0-1)
 * @param magnitude Magnitudine maximă spike
 * @return Valoare spike (0 dacă nu s-a produs)
 */
static double random_spike(double probability, double magnitude) {
    double r = (double)rand() / RAND_MAX;
    
    if (r < probability) {
        /* Spike pozitiv sau negativ cu probabilitate egală */
        double sign = (rand() % 2 == 0) ? 1.0 : -1.0;
        return sign * magnitude * (0.5 + 0.5 * (double)rand() / RAND_MAX);
    }
    
    return 0.0;
}

/**
 * Calculează ora simulată din zi (0-24) bazată pe timestamp
 * 
 * Pentru simulare accelerată, o "zi" poate fi mai scurtă
 */
static double get_simulated_hour(uint64_t timestamp_ms) {
    /* Simulare: 1 minut real = 1 oră simulată */
    /* Deci o zi completă = 24 minute */
    return fmod((double)(timestamp_ms / 1000) / 60.0, 24.0);
}

/* ============================================================================
 * FUNCȚII GENERARE VALORI SENZORI
 * ============================================================================ */

/**
 * Generează valoare temperatură cu variație sinusoidală
 */
static double generate_temperature(VirtualSensor* sensor, uint64_t timestamp_ms) {
    double hour = get_simulated_hour(timestamp_ms);
    
    /* Variație sinusoidală: maxim la 14:00, minim la 02:00 */
    double phase = (hour - 14.0) * M_PI / 12.0;
    double sinusoidal = TEMP_AMPLITUDE * cos(phase);
    
    /* Zgomot Gaussian */
    double noise = gaussian_random(0, TEMP_NOISE_STDDEV);
    
    /* Spike aleatoriu */
    double spike = random_spike(g_config.anomaly_probability, TEMP_SPIKE_MAGNITUDE);
    
    /* Drift lent */
    sensor->drift += sensor->drift_rate * g_config.interval_ms / 1000.0;
    
    double value = sensor->base_value + sinusoidal + noise + spike + sensor->drift;
    
    /* Limitare la valori realiste */
    if (value < -40.0) value = -40.0;
    if (value > 60.0) value = 60.0;
    
    return value;
}

/**
 * Generează valoare umiditate corelată cu temperatura
 */
static double generate_humidity(VirtualSensor* sensor, uint64_t timestamp_ms) {
    (void)timestamp_ms;
    
    double base = sensor->base_value;
    double noise = gaussian_random(0, HUMIDITY_NOISE_STDDEV);
    
    /* Corelație negativă cu temperatura */
    double temp_effect = 0.0;
    if (sensor->correlation_sensor_idx >= 0) {
        VirtualSensor* temp_sensor = &g_config.sensors[sensor->correlation_sensor_idx];
        double temp_delta = temp_sensor->last_value - TEMP_BASE;
        temp_effect = sensor->correlation_coef * temp_delta;
    }
    
    double value = base + temp_effect + noise;
    
    /* Limitare la 0-100% */
    if (value < 0.0) value = 0.0;
    if (value > 100.0) value = 100.0;
    
    return value;
}

/**
 * Generează event mișcare (PIR)
 */
static double generate_motion(VirtualSensor* sensor, uint64_t timestamp_ms) {
    (void)sensor;
    
    double hour = get_simulated_hour(timestamp_ms);
    
    /* Probabilitate mișcare mai mare în timpul zilei */
    double base_prob;
    if (hour >= 8.0 && hour <= 22.0) {
        base_prob = 0.3;  /* 30% șansă mișcare ziua */
    } else {
        base_prob = 0.05; /* 5% șansă mișcare noaptea */
    }
    
    return ((double)rand() / RAND_MAX < base_prob) ? 1.0 : 0.0;
}

/**
 * Generează valoare lumină (ciclu zi/noapte)
 */
static double generate_light(VirtualSensor* sensor, uint64_t timestamp_ms) {
    (void)sensor;
    
    double hour = get_simulated_hour(timestamp_ms);
    
    /* Tranziție graduală: răsărit 6-8, apus 18-20 */
    double daylight;
    if (hour < 6.0) {
        daylight = 0.0;
    } else if (hour < 8.0) {
        daylight = (hour - 6.0) / 2.0;  /* Răsărit gradual */
    } else if (hour < 18.0) {
        daylight = 1.0;
    } else if (hour < 20.0) {
        daylight = 1.0 - (hour - 18.0) / 2.0;  /* Apus gradual */
    } else {
        daylight = 0.0;
    }
    
    double base = LIGHT_NIGHT_VALUE + daylight * (LIGHT_DAY_VALUE - LIGHT_NIGHT_VALUE);
    double noise = gaussian_random(0, base * 0.1);  /* 10% zgomot relativ */
    
    double value = base + noise;
    if (value < 0.0) value = 0.0;
    
    return value;
}

/**
 * Generează valoare presiune atmosferică
 */
static double generate_pressure(VirtualSensor* sensor, uint64_t timestamp_ms) {
    (void)timestamp_ms;
    
    /* Presiunea variază foarte lent */
    sensor->drift += sensor->drift_rate * g_config.interval_ms / 1000.0;
    
    /* Reset drift periodic pentru a rămâne în limite realiste */
    if (fabs(sensor->drift) > 20.0) {
        sensor->drift *= 0.9;
    }
    
    double noise = gaussian_random(0, PRESSURE_NOISE_STDDEV);
    double value = sensor->base_value + sensor->drift + noise;
    
    /* Limite realiste: 980-1040 hPa */
    if (value < 980.0) value = 980.0;
    if (value > 1040.0) value = 1040.0;
    
    return value;
}

/**
 * Generează valoare pentru orice tip de senzor
 */
static double generate_sensor_value(VirtualSensor* sensor, uint64_t timestamp_ms) {
    switch (sensor->type) {
        case SENSOR_TEMPERATURE:
            return generate_temperature(sensor, timestamp_ms);
        case SENSOR_HUMIDITY:
            return generate_humidity(sensor, timestamp_ms);
        case SENSOR_MOTION:
            return generate_motion(sensor, timestamp_ms);
        case SENSOR_LIGHT:
            return generate_light(sensor, timestamp_ms);
        case SENSOR_PRESSURE:
            return generate_pressure(sensor, timestamp_ms);
        default:
            return 0.0;
    }
}

/* ============================================================================
 * FUNCȚII FORMATARE ȘI PUBLICARE
 * ============================================================================ */

/**
 * Formatează mesaj JSON pentru senzor
 */
static int format_json_message(VirtualSensor* sensor, double value, 
                                char* buffer, size_t buffer_size) {
    uint64_t timestamp = mqtt_get_timestamp_ms() / 1000;  /* Unix seconds */
    
    /* Formatare specială pentru motion (integer) */
    if (sensor->type == SENSOR_MOTION) {
        return snprintf(buffer, buffer_size,
            "{\"sensor_id\":\"%s\",\"type\":\"%s\",\"value\":%d,"
            "\"unit\":\"%s\",\"ts\":%lu}",
            sensor->id,
            SENSOR_TYPE_NAMES[sensor->type],
            (int)value,
            SENSOR_UNITS[sensor->type],
            (unsigned long)timestamp);
    }
    
    /* Formatare cu o zecimală pentru alți senzori */
    return snprintf(buffer, buffer_size,
        "{\"sensor_id\":\"%s\",\"type\":\"%s\",\"value\":%.1f,"
        "\"unit\":\"%s\",\"ts\":%lu}",
        sensor->id,
        SENSOR_TYPE_NAMES[sensor->type],
        value,
        SENSOR_UNITS[sensor->type],
        (unsigned long)timestamp);
}

/**
 * Publică valoare senzor
 */
static MqttError publish_sensor_value(VirtualSensor* sensor) {
    uint64_t now = mqtt_get_timestamp_ms();
    
    /* Generează valoare */
    double value = generate_sensor_value(sensor, now);
    sensor->last_value = value;
    
    /* Formatează mesaj */
    char payload[MAX_PAYLOAD_LENGTH];
    format_json_message(sensor, value, payload, sizeof(payload));
    
    /* Publică */
    MqttError err = mqtt_publish_string(g_client, sensor->topic, payload,
                                        MQTT_QOS_AT_MOST_ONCE, false);
    
    if (err == MQTT_SUCCESS) {
        sensor->last_publish_ms = now;
        sensor->message_count++;
        
        if (g_config.verbose) {
            /* Culoare bazată pe tip senzor */
            const char* color;
            switch (sensor->type) {
                case SENSOR_TEMPERATURE: color = COLOR_RED; break;
                case SENSOR_HUMIDITY:    color = COLOR_BLUE; break;
                case SENSOR_MOTION:      color = COLOR_YELLOW; break;
                case SENSOR_LIGHT:       color = COLOR_CYAN; break;
                case SENSOR_PRESSURE:    color = COLOR_MAGENTA; break;
                default:                 color = COLOR_RESET;
            }
            
            printf("%s[%s]%s %s -> %.2f %s\n",
                   color, sensor->id, COLOR_RESET,
                   sensor->topic, value, SENSOR_UNITS[sensor->type]);
        }
    }
    
    return err;
}

/* ============================================================================
 * FUNCȚII INIȚIALIZARE
 * ============================================================================ */

/**
 * Inițializează configurația cu valori implicite
 */
static void init_default_config(void) {
    strncpy(g_config.broker_address, DEFAULT_BROKER, sizeof(g_config.broker_address));
    g_config.broker_port = DEFAULT_PORT;
    g_config.interval_ms = DEFAULT_INTERVAL_MS;
    g_config.message_count = 0;  /* Infinit */
    strncpy(g_config.topic_prefix, DEFAULT_TOPIC_PREFIX, sizeof(g_config.topic_prefix));
    g_config.anomaly_probability = DEFAULT_ANOMALY_PROB;
    g_config.random_seed = (unsigned int)time(NULL);
    g_config.verbose = false;
    g_config.sensor_count = 0;
}

/**
 * Adaugă senzor la configurație
 */
static void add_sensor(SensorType type, const char* id_suffix) {
    if (g_config.sensor_count >= MAX_SENSORS) return;
    
    VirtualSensor* sensor = &g_config.sensors[g_config.sensor_count];
    
    /* ID și topic */
    snprintf(sensor->id, sizeof(sensor->id), "%s_%s",
             SENSOR_TYPE_NAMES[type], id_suffix);
    snprintf(sensor->topic, sizeof(sensor->topic), "%s%s/%s",
             g_config.topic_prefix, SENSOR_TYPE_NAMES[type], sensor->id);
    
    sensor->type = type;
    sensor->last_value = 0.0;
    sensor->last_publish_ms = 0;
    sensor->message_count = 0;
    sensor->correlation_sensor_idx = -1;
    sensor->correlation_coef = 0.0;
    sensor->drift = 0.0;
    
    /* Parametri specifici tipului */
    switch (type) {
        case SENSOR_TEMPERATURE:
            sensor->base_value = TEMP_BASE;
            sensor->noise_stddev = TEMP_NOISE_STDDEV;
            sensor->drift_rate = 0.001;  /* 0.001°C/sec drift */
            break;
            
        case SENSOR_HUMIDITY:
            sensor->base_value = HUMIDITY_BASE;
            sensor->noise_stddev = HUMIDITY_NOISE_STDDEV;
            sensor->drift_rate = 0.0;
            break;
            
        case SENSOR_MOTION:
            sensor->base_value = 0.0;
            sensor->noise_stddev = 0.0;
            sensor->drift_rate = 0.0;
            break;
            
        case SENSOR_LIGHT:
            sensor->base_value = LIGHT_DAY_VALUE / 2;
            sensor->noise_stddev = 50.0;
            sensor->drift_rate = 0.0;
            break;
            
        case SENSOR_PRESSURE:
            sensor->base_value = PRESSURE_BASE;
            sensor->noise_stddev = PRESSURE_NOISE_STDDEV;
            sensor->drift_rate = 0.01;  /* 0.01 hPa/sec drift */
            break;
            
        default:
            break;
    }
    
    g_config.sensor_count++;
}

/**
 * Configurează corelații între senzori
 */
static void setup_correlations(void) {
    /* Caută senzor temperatură */
    int temp_idx = -1;
    for (int i = 0; i < g_config.sensor_count; i++) {
        if (g_config.sensors[i].type == SENSOR_TEMPERATURE) {
            temp_idx = i;
            break;
        }
    }
    
    /* Leagă senzorii de umiditate de temperatură */
    if (temp_idx >= 0) {
        for (int i = 0; i < g_config.sensor_count; i++) {
            if (g_config.sensors[i].type == SENSOR_HUMIDITY) {
                g_config.sensors[i].correlation_sensor_idx = temp_idx;
                g_config.sensors[i].correlation_coef = HUMIDITY_TEMP_COEF;
            }
        }
    }
}

/**
 * Creează set standard de senzori
 */
static void create_default_sensors(void) {
    add_sensor(SENSOR_TEMPERATURE, "001");
    add_sensor(SENSOR_HUMIDITY, "001");
    add_sensor(SENSOR_MOTION, "001");
    add_sensor(SENSOR_LIGHT, "001");
    add_sensor(SENSOR_PRESSURE, "001");
    
    setup_correlations();
}

/* ============================================================================
 * FUNCȚII SIGNAL ȘI CLEANUP
 * ============================================================================ */

/**
 * Handler pentru SIGINT/SIGTERM
 */
static void signal_handler(int sig) {
    (void)sig;
    printf("\n" COLOR_YELLOW "Stopping simulator..." COLOR_RESET "\n");
    g_running = 0;
}

/**
 * Afișează statistici finale
 */
static void print_statistics(void) {
    printf("\n" COLOR_BOLD "=== Simulation Statistics ===" COLOR_RESET "\n\n");
    
    uint64_t total_messages = 0;
    
    for (int i = 0; i < g_config.sensor_count; i++) {
        VirtualSensor* sensor = &g_config.sensors[i];
        
        const char* color;
        switch (sensor->type) {
            case SENSOR_TEMPERATURE: color = COLOR_RED; break;
            case SENSOR_HUMIDITY:    color = COLOR_BLUE; break;
            case SENSOR_MOTION:      color = COLOR_YELLOW; break;
            case SENSOR_LIGHT:       color = COLOR_CYAN; break;
            case SENSOR_PRESSURE:    color = COLOR_MAGENTA; break;
            default:                 color = COLOR_RESET;
        }
        
        printf("%s%-20s%s: %lu messages, last value: %.2f %s\n",
               color, sensor->id, COLOR_RESET,
               (unsigned long)sensor->message_count,
               sensor->last_value,
               SENSOR_UNITS[sensor->type]);
        
        total_messages += sensor->message_count;
    }
    
    printf("\n" COLOR_GREEN "Total messages published: %lu" COLOR_RESET "\n\n",
           (unsigned long)total_messages);
}

/* ============================================================================
 * FUNCȚII HELP ȘI PARSARE ARGUMENTE
 * ============================================================================ */

/**
 * Afișează mesaj de ajutor
 */
static void print_help(const char* program_name) {
    printf(COLOR_BOLD "Virtual Sensor Simulator" COLOR_RESET "\n");
    printf("Generează date IoT realiste și le publică prin MQTT\n\n");
    
    printf(COLOR_CYAN "Backend: %s" COLOR_RESET "\n\n", mqtt_get_backend_name());
    
    printf("Utilizare: %s [opțiuni]\n\n", program_name);
    
    printf("Opțiuni:\n");
    printf("  -b <host>     Adresa broker (default: %s)\n", DEFAULT_BROKER);
    printf("  -p <port>     Port broker (default: %d)\n", DEFAULT_PORT);
    printf("  -i <ms>       Interval publicare ms (default: %d)\n", DEFAULT_INTERVAL_MS);
    printf("  -n <count>    Număr mesaje (default: infinit)\n");
    printf("  -t <topic>    Topic prefix (default: %s)\n", DEFAULT_TOPIC_PREFIX);
    printf("  -a <prob>     Probabilitate anomalie 0-1 (default: %.2f)\n", DEFAULT_ANOMALY_PROB);
    printf("  -s <seed>     Seed random\n");
    printf("  -v            Verbose mode\n");
    printf("  -h            Afișează acest mesaj\n");
    
    printf("\nExemple:\n");
    printf("  %s -v                      # Mod verbose, valori implicite\n", program_name);
    printf("  %s -b 192.168.1.100 -p 1883  # Broker remote\n", program_name);
    printf("  %s -i 500 -n 100 -a 0.1     # 100 msg la 500ms, 10%% anomalii\n", program_name);
}

/**
 * Parsează argumentele liniei de comandă
 */
static int parse_arguments(int argc, char* argv[]) {
    int opt;
    
    while ((opt = getopt(argc, argv, "b:p:i:n:t:a:s:vh")) != -1) {
        switch (opt) {
            case 'b':
                strncpy(g_config.broker_address, optarg, 
                        sizeof(g_config.broker_address) - 1);
                break;
            case 'p':
                g_config.broker_port = atoi(optarg);
                if (g_config.broker_port <= 0 || g_config.broker_port > 65535) {
                    fprintf(stderr, "Port invalid: %s\n", optarg);
                    return -1;
                }
                break;
            case 'i':
                g_config.interval_ms = atoi(optarg);
                if (g_config.interval_ms < 10) {
                    fprintf(stderr, "Interval prea mic (min 10ms)\n");
                    return -1;
                }
                break;
            case 'n':
                g_config.message_count = atoi(optarg);
                break;
            case 't':
                strncpy(g_config.topic_prefix, optarg,
                        sizeof(g_config.topic_prefix) - 1);
                break;
            case 'a':
                g_config.anomaly_probability = atof(optarg);
                if (g_config.anomaly_probability < 0 || g_config.anomaly_probability > 1) {
                    fprintf(stderr, "Probabilitate invalidă (0-1)\n");
                    return -1;
                }
                break;
            case 's':
                g_config.random_seed = (unsigned int)atoi(optarg);
                break;
            case 'v':
                g_config.verbose = true;
                break;
            case 'h':
                print_help(argv[0]);
                exit(0);
            default:
                print_help(argv[0]);
                return -1;
        }
    }
    
    return 0;
}

/* ============================================================================
 * FUNCȚIA PRINCIPALĂ
 * ============================================================================ */

int main(int argc, char* argv[]) {
    /* Inițializare */
    init_default_config();
    
    if (parse_arguments(argc, argv) != 0) {
        return 1;
    }
    
    /* Inițializează generator random */
    srand(g_config.random_seed);
    
    /* Creează senzori */
    create_default_sensors();
    
    /* Configurează signal handlers */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    /* Banner */
    printf("\n");
    printf(COLOR_BOLD COLOR_GREEN "╔══════════════════════════════════════════════╗" COLOR_RESET "\n");
    printf(COLOR_BOLD COLOR_GREEN "║     VIRTUAL SENSOR SIMULATOR                 ║" COLOR_RESET "\n");
    printf(COLOR_BOLD COLOR_GREEN "║     ATP Week 19 - IoT Stream Processing      ║" COLOR_RESET "\n");
    printf(COLOR_BOLD COLOR_GREEN "╚══════════════════════════════════════════════╝" COLOR_RESET "\n");
    printf("\n");
    printf("Backend: " COLOR_CYAN "%s" COLOR_RESET "\n", mqtt_get_backend_name());
    printf("Broker:  " COLOR_YELLOW "%s:%d" COLOR_RESET "\n", 
           g_config.broker_address, g_config.broker_port);
    printf("Sensors: " COLOR_MAGENTA "%d" COLOR_RESET "\n", g_config.sensor_count);
    printf("Interval: %d ms\n", g_config.interval_ms);
    printf("Anomaly prob: %.1f%%\n", g_config.anomaly_probability * 100);
    printf("\n");
    
    /* Creează client MQTT */
    MqttError err = mqtt_client_create(&g_client, NULL);
    if (err != MQTT_SUCCESS) {
        fprintf(stderr, COLOR_RED "Eroare creare client: %s" COLOR_RESET "\n",
                mqtt_error_string(err));
        return 1;
    }
    
    /* Conectare */
    MqttConnectOptions opts;
    mqtt_options_init(&opts);
    opts.broker_address = g_config.broker_address;
    opts.broker_port = g_config.broker_port;
    
    printf("Connecting to broker...\n");
    
    err = mqtt_client_connect(g_client, &opts);
    if (err != MQTT_SUCCESS) {
        fprintf(stderr, COLOR_RED "Eroare conectare: %s" COLOR_RESET "\n",
                mqtt_error_string(err));
        mqtt_client_destroy(g_client);
        return 1;
    }
    
    printf(COLOR_GREEN "Connected!" COLOR_RESET " Starting sensor simulation...\n\n");
    
    /* Loop principal */
    int total_published = 0;
    
    while (g_running) {
        /* Publică pentru fiecare senzor */
        for (int i = 0; i < g_config.sensor_count && g_running; i++) {
            err = publish_sensor_value(&g_config.sensors[i]);
            
            if (err != MQTT_SUCCESS) {
                fprintf(stderr, COLOR_RED "Eroare publicare: %s" COLOR_RESET "\n",
                        mqtt_error_string(err));
            }
        }
        
        total_published++;
        
        /* Verifică limită mesaje */
        if (g_config.message_count > 0 && total_published >= g_config.message_count) {
            printf(COLOR_YELLOW "Message limit reached (%d)" COLOR_RESET "\n",
                   g_config.message_count);
            break;
        }
        
        /* Așteaptă interval */
        usleep(g_config.interval_ms * 1000);
    }
    
    /* Cleanup */
    print_statistics();
    
    mqtt_client_disconnect(g_client);
    mqtt_client_destroy(g_client);
    
    printf(COLOR_GREEN "Simulator stopped cleanly." COLOR_RESET "\n");
    
    return 0;
}
