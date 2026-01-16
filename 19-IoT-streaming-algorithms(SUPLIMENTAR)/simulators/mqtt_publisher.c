/**
 * ============================================================================
 * MQTT PUBLISHER - Exemplu Publicare Mesaje
 * ============================================================================
 * 
 * Săptămâna 19: Algoritmi pentru IoT și Stream Processing
 * Curs: Algoritmi și Tehnici de Programare, ASE București
 * 
 * DESCRIERE:
 *   Demonstrează utilizarea API-ului MQTT uniform pentru publicare.
 *   Funcționează identic cu ambele backend-uri (Paho/POSIX).
 * 
 * COMPILARE:
 *   # Cu simulare POSIX (fără dependențe):
 *   gcc -o mqtt_publisher mqtt_publisher.c mqtt_posix.c -lpthread -lrt
 *   
 *   # Cu Paho MQTT (broker real):
 *   gcc -DUSE_PAHO_MQTT -o mqtt_publisher mqtt_publisher.c mqtt_paho.c \
 *       -lpaho-mqtt3c -lpthread
 * 
 * UTILIZARE:
 *   ./mqtt_publisher [topic] [message]
 *   ./mqtt_publisher                    # Publicare interactivă
 *   echo "test" | ./mqtt_publisher sensors/temp  # Pipe
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

/* ============================================================================
 * CULORI ANSI
 * ============================================================================ */

#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RED     "\033[31m"
#define COLOR_BOLD    "\033[1m"

/* ============================================================================
 * VARIABILE GLOBALE
 * ============================================================================ */

static volatile sig_atomic_t g_running = 1;
static MqttClient g_client = NULL;

/* ============================================================================
 * FUNCȚII
 * ============================================================================ */

/**
 * Handler pentru SIGINT
 */
static void signal_handler(int sig) {
    (void)sig;
    g_running = 0;
    printf("\n" COLOR_YELLOW "Exiting..." COLOR_RESET "\n");
}

/**
 * Callback pentru schimbări stare conexiune
 */
static void connection_callback(MqttConnectionState state, int reason, void* user_data) {
    (void)user_data;
    
    switch (state) {
        case MQTT_STATE_CONNECTED:
            printf(COLOR_GREEN "✓ Connected to broker" COLOR_RESET "\n");
            break;
        case MQTT_STATE_DISCONNECTED:
            printf(COLOR_YELLOW "✗ Disconnected (reason: %d)" COLOR_RESET "\n", reason);
            break;
        case MQTT_STATE_RECONNECTING:
            printf(COLOR_CYAN "↻ Reconnecting..." COLOR_RESET "\n");
            break;
        case MQTT_STATE_ERROR:
            printf(COLOR_RED "✗ Connection error: %s" COLOR_RESET "\n",
                   mqtt_error_string(reason));
            break;
        default:
            break;
    }
}

/**
 * Publică un mesaj și afișează rezultatul
 */
static int publish_message(const char* topic, const char* message, MqttQoS qos) {
    printf("Publishing to " COLOR_CYAN "%s" COLOR_RESET ": %s\n", topic, message);
    
    MqttError err = mqtt_publish_string(g_client, topic, message, qos, false);
    
    if (err == MQTT_SUCCESS) {
        printf(COLOR_GREEN "✓ Published successfully" COLOR_RESET "\n");
        return 0;
    } else {
        printf(COLOR_RED "✗ Publish failed: %s" COLOR_RESET "\n", mqtt_error_string(err));
        return -1;
    }
}

/**
 * Mod interactiv - citește topic și mesaj de la stdin
 */
static void interactive_mode(void) {
    char topic[256];
    char message[1024];
    
    printf("\n" COLOR_BOLD "Interactive Publisher Mode" COLOR_RESET "\n");
    printf("Enter 'quit' to exit, or 'topic:message' to publish\n\n");
    
    while (g_running && mqtt_client_is_connected(g_client)) {
        printf(COLOR_CYAN "topic:message> " COLOR_RESET);
        fflush(stdout);
        
        char line[1280];
        if (!fgets(line, sizeof(line), stdin)) {
            break;  /* EOF */
        }
        
        /* Elimină newline */
        line[strcspn(line, "\n")] = '\0';
        
        if (strlen(line) == 0) continue;
        
        if (strcmp(line, "quit") == 0 || strcmp(line, "exit") == 0) {
            break;
        }
        
        /* Parsează topic:message */
        char* colon = strchr(line, ':');
        if (!colon) {
            printf(COLOR_YELLOW "Format: topic:message" COLOR_RESET "\n");
            continue;
        }
        
        *colon = '\0';
        strncpy(topic, line, sizeof(topic) - 1);
        strncpy(message, colon + 1, sizeof(message) - 1);
        
        publish_message(topic, message, MQTT_QOS_AT_MOST_ONCE);
    }
}

/**
 * Afișează ajutor
 */
static void print_help(const char* prog) {
    printf(COLOR_BOLD "MQTT Publisher Example" COLOR_RESET "\n\n");
    printf("Backend: %s\n\n", mqtt_get_backend_name());
    printf("Usage:\n");
    printf("  %s                        Interactive mode\n", prog);
    printf("  %s <topic> <message>      Publish single message\n", prog);
    printf("  %s <topic> -              Read message from stdin\n", prog);
    printf("\nOptions:\n");
    printf("  -b <host>    Broker address (default: localhost)\n");
    printf("  -p <port>    Broker port (default: 1883)\n");
    printf("  -q <qos>     QoS level 0-2 (default: 0)\n");
    printf("  -r           Publish as retained\n");
    printf("  -h           Show this help\n");
}

/* ============================================================================
 * MAIN
 * ============================================================================ */

int main(int argc, char* argv[]) {
    const char* broker = "localhost";
    int port = 1883;
    MqttQoS qos = MQTT_QOS_AT_MOST_ONCE;
    bool retained = false;
    const char* topic = NULL;
    const char* message = NULL;
    
    /* Parsare argumente simple */
    int i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            broker = argv[++i];
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            port = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc) {
            int q = atoi(argv[++i]);
            if (q >= 0 && q <= 2) qos = (MqttQoS)q;
        } else if (strcmp(argv[i], "-r") == 0) {
            retained = true;
        } else if (!topic) {
            topic = argv[i];
        } else if (!message) {
            message = argv[i];
        }
        i++;
    }
    
    /* Banner */
    printf("\n" COLOR_BOLD COLOR_GREEN "MQTT Publisher" COLOR_RESET "\n");
    printf("Backend: " COLOR_CYAN "%s" COLOR_RESET "\n", mqtt_get_backend_name());
    printf("Broker:  %s:%d\n\n", broker, port);
    
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
    opts.broker_address = broker;
    opts.broker_port = port;
    
    err = mqtt_client_connect(g_client, &opts);
    if (err != MQTT_SUCCESS) {
        fprintf(stderr, COLOR_RED "Connection failed: %s" COLOR_RESET "\n",
                mqtt_error_string(err));
        mqtt_client_destroy(g_client);
        return 1;
    }
    
    /* Decide modul de operare */
    if (topic && message) {
        /* Publicare singulară */
        if (strcmp(message, "-") == 0) {
            /* Citește mesaj din stdin */
            char stdin_msg[4096];
            size_t total = 0;
            size_t n;
            while ((n = fread(stdin_msg + total, 1, sizeof(stdin_msg) - total - 1, stdin)) > 0) {
                total += n;
                if (total >= sizeof(stdin_msg) - 1) break;
            }
            stdin_msg[total] = '\0';
            /* Elimină trailing newline */
            if (total > 0 && stdin_msg[total-1] == '\n') {
                stdin_msg[total-1] = '\0';
            }
            err = mqtt_publish_string(g_client, topic, stdin_msg, qos, retained);
        } else {
            err = mqtt_publish_string(g_client, topic, message, qos, retained);
        }
        
        if (err == MQTT_SUCCESS) {
            printf(COLOR_GREEN "✓ Message published to %s" COLOR_RESET "\n", topic);
        } else {
            printf(COLOR_RED "✗ Publish failed: %s" COLOR_RESET "\n", mqtt_error_string(err));
        }
    } else if (topic) {
        /* Topic fără mesaj - citește din stdin linie cu linie */
        char line[4096];
        while (g_running && fgets(line, sizeof(line), stdin)) {
            line[strcspn(line, "\n")] = '\0';
            if (strlen(line) > 0) {
                publish_message(topic, line, qos);
            }
        }
    } else {
        /* Mod interactiv */
        interactive_mode();
    }
    
    /* Cleanup */
    mqtt_client_disconnect(g_client);
    mqtt_client_destroy(g_client);
    
    printf(COLOR_GREEN "Publisher terminated." COLOR_RESET "\n");
    
    return 0;
}
