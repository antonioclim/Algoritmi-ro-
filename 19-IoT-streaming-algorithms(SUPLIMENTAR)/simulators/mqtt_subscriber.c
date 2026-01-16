/**
 * ============================================================================
 * MQTT SUBSCRIBER - Exemplu Recepție Mesaje
 * ============================================================================
 * 
 * Săptămâna 19: Algoritmi pentru IoT și Stream Processing
 * Curs: Algoritmi și Tehnici de Programare, ASE București
 * 
 * DESCRIERE:
 *   Demonstrează subscriberea la topic-uri MQTT cu suport wildcards.
 *   Afișează mesajele primite cu timestamp și statistici.
 * 
 * WILDCARDS MQTT:
 *   + (plus)   - potrivește exact un nivel
 *   # (hash)   - potrivește zero sau mai multe nivele (doar la final)
 *   
 *   Exemple:
 *     sensors/+/temperature  - orice senzor, topic temperature
 *     sensors/#              - toate topic-urile sub sensors/
 *     home/+/+/status        - ex: home/floor1/room1/status
 * 
 * COMPILARE:
 *   # Cu simulare POSIX:
 *   gcc -o mqtt_subscriber mqtt_subscriber.c mqtt_posix.c -lpthread -lrt
 *   
 *   # Cu Paho MQTT:
 *   gcc -DUSE_PAHO_MQTT -o mqtt_subscriber mqtt_subscriber.c mqtt_paho.c \
 *       -lpaho-mqtt3c -lpthread
 * 
 * UTILIZARE:
 *   ./mqtt_subscriber [topic_filter...]
 *   ./mqtt_subscriber sensors/#
 *   ./mqtt_subscriber sensors/+/temperature sensors/+/humidity
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
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

/* ============================================================================
 * CULORI ANSI
 * ============================================================================ */

#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RED     "\033[31m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_DIM     "\033[2m"

/* ============================================================================
 * CONFIGURAȚIE
 * ============================================================================ */

#define MAX_TOPICS      16
#define MAX_TOPIC_LEN   256

/* ============================================================================
 * STRUCTURI
 * ============================================================================ */

/** Statistici per topic */
typedef struct {
    char topic_filter[MAX_TOPIC_LEN];
    uint64_t message_count;
    uint64_t total_bytes;
    uint64_t first_msg_time;
    uint64_t last_msg_time;
} TopicStats;

/** Configurație subscriber */
typedef struct {
    const char* broker;
    int port;
    MqttQoS qos;
    bool verbose;
    bool quiet;
    bool show_timestamp;
    bool json_output;
    int max_messages;
    
    TopicStats topics[MAX_TOPICS];
    int topic_count;
    pthread_mutex_t stats_mutex;
} SubscriberConfig;

/* ============================================================================
 * VARIABILE GLOBALE
 * ============================================================================ */

static volatile sig_atomic_t g_running = 1;
static MqttClient g_client = NULL;
static SubscriberConfig g_config;

/* ============================================================================
 * FUNCȚII STATISTICI
 * ============================================================================ */

/**
 * Actualizează statistici pentru un topic
 */
static void update_stats(const char* topic, size_t payload_len) {
    pthread_mutex_lock(&g_config.stats_mutex);
    
    /* Găsește sau creează entry pentru topic */
    int idx = -1;
    for (int i = 0; i < g_config.topic_count; i++) {
        /* Verifică potrivire cu orice pattern subscris */
        if (mqtt_topic_matches(topic, g_config.topics[i].topic_filter)) {
            idx = i;
            break;
        }
    }
    
    if (idx >= 0) {
        TopicStats* stats = &g_config.topics[idx];
        uint64_t now = mqtt_get_timestamp_ms();
        
        if (stats->message_count == 0) {
            stats->first_msg_time = now;
        }
        stats->last_msg_time = now;
        stats->message_count++;
        stats->total_bytes += payload_len;
    }
    
    pthread_mutex_unlock(&g_config.stats_mutex);
}

/**
 * Afișează statisticile finale
 */
static void print_statistics(void) {
    printf("\n" COLOR_BOLD "═══════════════════════════════════════════════════════════\n");
    printf("                    SUBSCRIPTION STATISTICS                    \n");
    printf("═══════════════════════════════════════════════════════════" COLOR_RESET "\n\n");
    
    pthread_mutex_lock(&g_config.stats_mutex);
    
    uint64_t total_messages = 0;
    uint64_t total_bytes = 0;
    
    for (int i = 0; i < g_config.topic_count; i++) {
        TopicStats* stats = &g_config.topics[i];
        
        total_messages += stats->message_count;
        total_bytes += stats->total_bytes;
        
        if (stats->message_count > 0) {
            double duration_sec = (stats->last_msg_time - stats->first_msg_time) / 1000.0;
            double msg_rate = duration_sec > 0 ? stats->message_count / duration_sec : 0;
            double avg_size = (double)stats->total_bytes / stats->message_count;
            
            printf(COLOR_CYAN "%-40s" COLOR_RESET "\n", stats->topic_filter);
            printf("  Messages: " COLOR_GREEN "%lu" COLOR_RESET "\n", 
                   (unsigned long)stats->message_count);
            printf("  Bytes:    %lu (avg %.1f bytes/msg)\n",
                   (unsigned long)stats->total_bytes, avg_size);
            printf("  Rate:     %.2f msg/sec\n", msg_rate);
            printf("\n");
        }
    }
    
    pthread_mutex_unlock(&g_config.stats_mutex);
    
    printf(COLOR_BOLD "TOTAL: %lu messages, %lu bytes" COLOR_RESET "\n\n",
           (unsigned long)total_messages, (unsigned long)total_bytes);
}

/* ============================================================================
 * FUNCȚII CALLBACK
 * ============================================================================ */

/**
 * Selectează culoare bazată pe topic
 */
static const char* get_topic_color(const char* topic) {
    /* Colorează bazat pe tip senzor */
    if (strstr(topic, "temperature")) return COLOR_RED;
    if (strstr(topic, "humidity"))    return COLOR_BLUE;
    if (strstr(topic, "motion"))      return COLOR_YELLOW;
    if (strstr(topic, "light"))       return COLOR_CYAN;
    if (strstr(topic, "pressure"))    return COLOR_MAGENTA;
    return COLOR_GREEN;
}

/**
 * Callback pentru mesaje primite
 */
static void message_callback(const MqttMessage* msg, void* user_data) {
    (void)user_data;
    
    if (!g_running) return;
    
    /* Actualizează statistici */
    update_stats(msg->topic, msg->payload_length);
    
    if (g_config.quiet) return;  /* Mod silențios - doar statistici */
    
    /* Formatare output */
    if (g_config.json_output) {
        /* Output JSON pentru parsare automată */
        printf("{\"topic\":\"%s\",\"payload\":\"", msg->topic);
        /* Escape payload pentru JSON */
        for (size_t i = 0; i < msg->payload_length; i++) {
            char c = ((char*)msg->payload)[i];
            if (c == '"' || c == '\\') {
                putchar('\\');
            }
            if (c >= 32 && c < 127) {
                putchar(c);
            } else {
                printf("\\x%02x", (unsigned char)c);
            }
        }
        printf("\",\"timestamp\":%lu,\"qos\":%d,\"retained\":%s}\n",
               (unsigned long)msg->timestamp_ms,
               msg->qos,
               msg->retained ? "true" : "false");
        fflush(stdout);
        return;
    }
    
    /* Output human-readable */
    const char* color = get_topic_color(msg->topic);
    
    /* Timestamp */
    if (g_config.show_timestamp) {
        time_t ts = (time_t)(msg->timestamp_ms / 1000);
        struct tm* tm_info = localtime(&ts);
        char time_str[32];
        strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);
        printf(COLOR_DIM "[%s.%03d]" COLOR_RESET " ",
               time_str, (int)(msg->timestamp_ms % 1000));
    }
    
    /* Topic */
    printf("%s%s%s", color, msg->topic, COLOR_RESET);
    
    /* Flags */
    if (msg->retained) {
        printf(" " COLOR_YELLOW "[R]" COLOR_RESET);
    }
    if (msg->qos > 0) {
        printf(" " COLOR_DIM "[Q%d]" COLOR_RESET, msg->qos);
    }
    
    printf(" → ");
    
    /* Payload */
    if (msg->payload_length == 0) {
        printf(COLOR_DIM "(empty)" COLOR_RESET);
    } else if (g_config.verbose || msg->payload_length < 200) {
        /* Afișează payload-ul dacă e scurt sau în mod verbose */
        fwrite(msg->payload, 1, msg->payload_length, stdout);
    } else {
        /* Truncează payload-ul lung */
        fwrite(msg->payload, 1, 100, stdout);
        printf("... " COLOR_DIM "(%zu bytes total)" COLOR_RESET,
               msg->payload_length);
    }
    
    printf("\n");
    fflush(stdout);
}

/**
 * Callback pentru schimbări conexiune
 */
static void connection_callback(MqttConnectionState state, int reason, void* user_data) {
    (void)user_data;
    
    switch (state) {
        case MQTT_STATE_CONNECTED:
            if (!g_config.quiet) {
                printf(COLOR_GREEN "✓ Connected to broker" COLOR_RESET "\n");
            }
            break;
        case MQTT_STATE_DISCONNECTED:
            printf(COLOR_YELLOW "✗ Disconnected (reason: %d)" COLOR_RESET "\n", reason);
            g_running = 0;
            break;
        case MQTT_STATE_RECONNECTING:
            printf(COLOR_CYAN "↻ Reconnecting..." COLOR_RESET "\n");
            break;
        case MQTT_STATE_ERROR:
            printf(COLOR_RED "✗ Error: %s" COLOR_RESET "\n", mqtt_error_string(reason));
            break;
        default:
            break;
    }
}

/* ============================================================================
 * FUNCȚII AUXILIARE
 * ============================================================================ */

/**
 * Handler pentru SIGINT
 */
static void signal_handler(int sig) {
    (void)sig;
    g_running = 0;
    printf("\n" COLOR_YELLOW "Stopping subscriber..." COLOR_RESET "\n");
}

/**
 * Afișează ajutor
 */
static void print_help(const char* prog) {
    printf(COLOR_BOLD "MQTT Subscriber Example" COLOR_RESET "\n\n");
    printf("Backend: %s\n\n", mqtt_get_backend_name());
    
    printf("Usage: %s [options] [topic_filter...]\n\n", prog);
    
    printf("Options:\n");
    printf("  -b <host>    Broker address (default: localhost)\n");
    printf("  -p <port>    Broker port (default: 1883)\n");
    printf("  -q <qos>     QoS level 0-2 (default: 0)\n");
    printf("  -n <count>   Exit after N messages\n");
    printf("  -t           Show timestamps\n");
    printf("  -j           JSON output format\n");
    printf("  -v           Verbose (show full payloads)\n");
    printf("  -s           Silent (statistics only)\n");
    printf("  -h           Show this help\n");
    
    printf("\nTopic Wildcards:\n");
    printf("  +            Match exactly one level\n");
    printf("  #            Match zero or more levels (end only)\n");
    
    printf("\nExamples:\n");
    printf("  %s sensors/#                    All sensor topics\n", prog);
    printf("  %s sensors/+/temperature        Any sensor's temperature\n", prog);
    printf("  %s home/+/+/status -t -v        With timestamps, verbose\n", prog);
}

/* ============================================================================
 * MAIN
 * ============================================================================ */

int main(int argc, char* argv[]) {
    /* Inițializare configurație */
    memset(&g_config, 0, sizeof(g_config));
    g_config.broker = "localhost";
    g_config.port = 1883;
    g_config.qos = MQTT_QOS_AT_MOST_ONCE;
    g_config.verbose = false;
    g_config.quiet = false;
    g_config.show_timestamp = false;
    g_config.json_output = false;
    g_config.max_messages = 0;
    g_config.topic_count = 0;
    pthread_mutex_init(&g_config.stats_mutex, NULL);
    
    /* Parsare argumente */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            g_config.broker = argv[++i];
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            g_config.port = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc) {
            int q = atoi(argv[++i]);
            if (q >= 0 && q <= 2) g_config.qos = (MqttQoS)q;
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            g_config.max_messages = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-t") == 0) {
            g_config.show_timestamp = true;
        } else if (strcmp(argv[i], "-j") == 0) {
            g_config.json_output = true;
        } else if (strcmp(argv[i], "-v") == 0) {
            g_config.verbose = true;
        } else if (strcmp(argv[i], "-s") == 0) {
            g_config.quiet = true;
        } else if (argv[i][0] != '-' && g_config.topic_count < MAX_TOPICS) {
            /* Topic filter */
            strncpy(g_config.topics[g_config.topic_count].topic_filter,
                    argv[i], MAX_TOPIC_LEN - 1);
            g_config.topic_count++;
        }
    }
    
    /* Topic implicit dacă nu s-a specificat niciunul */
    if (g_config.topic_count == 0) {
        strncpy(g_config.topics[0].topic_filter, "#", MAX_TOPIC_LEN);
        g_config.topic_count = 1;
    }
    
    /* Banner */
    if (!g_config.json_output) {
        printf("\n" COLOR_BOLD COLOR_GREEN "MQTT Subscriber" COLOR_RESET "\n");
        printf("Backend: " COLOR_CYAN "%s" COLOR_RESET "\n", mqtt_get_backend_name());
        printf("Broker:  %s:%d\n", g_config.broker, g_config.port);
        printf("Topics:  ");
        for (int i = 0; i < g_config.topic_count; i++) {
            printf(COLOR_CYAN "%s" COLOR_RESET "%s",
                   g_config.topics[i].topic_filter,
                   i < g_config.topic_count - 1 ? ", " : "");
        }
        printf("\n\n");
    }
    
    /* Setup signals */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    /* Creează client */
    MqttError err = mqtt_client_create(&g_client, NULL);
    if (err != MQTT_SUCCESS) {
        fprintf(stderr, COLOR_RED "Error creating client: %s" COLOR_RESET "\n",
                mqtt_error_string(err));
        return 1;
    }
    
    mqtt_set_connection_callback(g_client, connection_callback, NULL);
    
    /* Conectare */
    MqttConnectOptions opts;
    mqtt_options_init(&opts);
    opts.broker_address = g_config.broker;
    opts.broker_port = g_config.port;
    
    err = mqtt_client_connect(g_client, &opts);
    if (err != MQTT_SUCCESS) {
        fprintf(stderr, COLOR_RED "Connection failed: %s" COLOR_RESET "\n",
                mqtt_error_string(err));
        mqtt_client_destroy(g_client);
        return 1;
    }
    
    /* Subscrie la toate topic-urile */
    for (int i = 0; i < g_config.topic_count; i++) {
        err = mqtt_subscribe(g_client, 
                            g_config.topics[i].topic_filter,
                            g_config.qos,
                            message_callback,
                            NULL);
        
        if (err != MQTT_SUCCESS) {
            fprintf(stderr, COLOR_RED "Subscribe failed for '%s': %s" COLOR_RESET "\n",
                    g_config.topics[i].topic_filter, mqtt_error_string(err));
        } else if (!g_config.quiet && !g_config.json_output) {
            printf(COLOR_GREEN "✓ Subscribed to: %s" COLOR_RESET "\n",
                   g_config.topics[i].topic_filter);
        }
    }
    
    if (!g_config.quiet && !g_config.json_output) {
        printf("\nWaiting for messages (Ctrl+C to stop)...\n\n");
    }
    
    /* Loop principal */
    uint64_t total_received = 0;
    
    while (g_running && mqtt_client_is_connected(g_client)) {
        err = mqtt_process(g_client, 100);
        
        if (err != MQTT_SUCCESS && err != MQTT_ERR_TIMEOUT) {
            if (g_running) {
                fprintf(stderr, COLOR_RED "Process error: %s" COLOR_RESET "\n",
                        mqtt_error_string(err));
            }
            break;
        }
        
        /* Verifică limită mesaje */
        if (g_config.max_messages > 0) {
            pthread_mutex_lock(&g_config.stats_mutex);
            total_received = 0;
            for (int i = 0; i < g_config.topic_count; i++) {
                total_received += g_config.topics[i].message_count;
            }
            pthread_mutex_unlock(&g_config.stats_mutex);
            
            if (total_received >= (uint64_t)g_config.max_messages) {
                if (!g_config.quiet) {
                    printf(COLOR_YELLOW "Message limit reached (%d)" COLOR_RESET "\n",
                           g_config.max_messages);
                }
                break;
            }
        }
    }
    
    /* Cleanup */
    if (!g_config.json_output) {
        print_statistics();
    }
    
    mqtt_client_disconnect(g_client);
    mqtt_client_destroy(g_client);
    pthread_mutex_destroy(&g_config.stats_mutex);
    
    if (!g_config.json_output) {
        printf(COLOR_GREEN "Subscriber terminated." COLOR_RESET "\n");
    }
    
    return 0;
}
