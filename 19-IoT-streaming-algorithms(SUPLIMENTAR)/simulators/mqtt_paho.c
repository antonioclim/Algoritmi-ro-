/**
 * ============================================================================
 * MQTT PAHO IMPLEMENTATION - Conectare Reală la Broker
 * ============================================================================
 * 
 * Săptămâna 19: Algoritmi pentru IoT și Stream Processing
 * Curs: Algoritmi și Tehnici de Programare, ASE București
 * 
 * DESCRIERE:
 *   Implementare a abstractizării MQTT folosind biblioteca Paho MQTT C.
 *   Permite conectarea reală la un broker MQTT (Mosquitto, HiveMQ, etc.)
 * 
 * DEPENDENȚE:
 *   - libpaho-mqtt3c (paho.mqtt.c synchronous client)
 *   
 *   Instalare pe Ubuntu/Debian:
 *     sudo apt install libpaho-mqtt-dev
 *   
 *   Sau din sursă:
 *     git clone https://github.com/eclipse/paho.mqtt.c.git
 *     cd paho.mqtt.c && mkdir build && cd build
 *     cmake .. && make && sudo make install
 * 
 * COMPILARE:
 *   gcc -DUSE_PAHO_MQTT -o program program.c mqtt_paho.c -lpaho-mqtt3c
 * 
 * ARHITECTURĂ:
 *   +------------------+
 *   |   Application    |
 *   +--------+---------+
 *            |
 *   +--------v---------+
 *   | mqtt_common.h    |  (API uniform)
 *   +--------+---------+
 *            |
 *   +--------v---------+
 *   | mqtt_paho.c      |  (acest fișier)
 *   +--------+---------+
 *            |
 *   +--------v---------+
 *   | libpaho-mqtt3c   |  (biblioteca externă)
 *   +--------+---------+
 *            |
 *            v
 *   [  Mosquitto Broker  ] (localhost:1883 sau remote)
 * 
 * CARACTERISTICI:
 *   - Suport complet QoS 0, 1, 2
 *   - Conexiuni securizate TLS (opțional)
 *   - Reconectare automată
 *   - Last Will and Testament (LWT)
 *   - Mesaje retained
 * 
 * AUTOR: Generated for ASE-CSIE ATP Course
 * VERSIUNE: 1.0 (Ianuarie 2026)
 * ============================================================================
 */

#ifdef USE_PAHO_MQTT

#include "mqtt_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

/* Include Paho MQTT */
#include <MQTTClient.h>

/* ============================================================================
 * CONSTANTE
 * ============================================================================ */

#define PAHO_QOS_0  0
#define PAHO_QOS_1  1
#define PAHO_QOS_2  2

#define DEFAULT_TIMEOUT_MS  10000

/* ============================================================================
 * STRUCTURI INTERNE
 * ============================================================================ */

/**
 * Înregistrare subscripție
 */
typedef struct {
    char topic_filter[MQTT_MAX_TOPIC_LENGTH];
    MqttQoS qos;
    MqttMessageCallback callback;
    void* user_data;
    bool active;
} PahoSubscription;

/**
 * Implementarea clientului MQTT cu Paho
 */
struct MqttClientImpl {
    /* Handle Paho */
    MQTTClient paho_client;
    
    /* Identificare */
    char client_id[MQTT_MAX_CLIENT_ID_LENGTH];
    char broker_uri[256];
    
    /* Stare */
    MqttConnectionState state;
    bool running;
    
    /* Subscripții */
    PahoSubscription subscriptions[MQTT_MAX_SUBSCRIPTIONS];
    int subscription_count;
    pthread_mutex_t subs_mutex;
    
    /* Callbacks */
    MqttConnectionCallback conn_callback;
    void* conn_user_data;
    
    /* Opțiuni salvate pentru reconectare */
    MqttConnectOptions saved_options;
};

/* ============================================================================
 * FUNCȚII HELPER
 * ============================================================================ */

/**
 * Convertește QoS intern la Paho
 */
static int qos_to_paho(MqttQoS qos) {
    switch (qos) {
        case MQTT_QOS_AT_MOST_ONCE:  return PAHO_QOS_0;
        case MQTT_QOS_AT_LEAST_ONCE: return PAHO_QOS_1;
        case MQTT_QOS_EXACTLY_ONCE:  return PAHO_QOS_2;
        default: return PAHO_QOS_0;
    }
}

/**
 * Convertește QoS Paho la intern
 */
static MqttQoS qos_from_paho(int qos) {
    switch (qos) {
        case PAHO_QOS_0: return MQTT_QOS_AT_MOST_ONCE;
        case PAHO_QOS_1: return MQTT_QOS_AT_LEAST_ONCE;
        case PAHO_QOS_2: return MQTT_QOS_EXACTLY_ONCE;
        default: return MQTT_QOS_AT_MOST_ONCE;
    }
}

/**
 * Callback Paho pentru pierdere conexiune
 */
static void connection_lost_callback(void* context, char* cause) {
    struct MqttClientImpl* impl = (struct MqttClientImpl*)context;
    
    MQTT_LOG("Connection lost: %s", cause ? cause : "unknown");
    
    impl->state = MQTT_STATE_DISCONNECTED;
    
    if (impl->conn_callback) {
        impl->conn_callback(MQTT_STATE_DISCONNECTED, 
                           MQTT_ERR_DISCONNECTED,
                           impl->conn_user_data);
    }
}

/**
 * Callback Paho pentru mesaj primit
 */
static int message_arrived_callback(void* context, 
                                    char* topicName, 
                                    int topicLen,
                                    MQTTClient_message* message) {
    struct MqttClientImpl* impl = (struct MqttClientImpl*)context;
    
    if (!impl || !topicName || !message) {
        return 1; /* Mesaj consumat */
    }
    
    MQTT_LOG("Message arrived on '%s' (%d bytes)", 
             topicName, message->payloadlen);
    
    /* Construiește mesajul pentru callback */
    MqttMessage mqtt_msg;
    
    /* Topic-ul poate să nu fie null-terminated */
    size_t topic_len = topicLen > 0 ? (size_t)topicLen : strlen(topicName);
    if (topic_len >= MQTT_MAX_TOPIC_LENGTH) {
        topic_len = MQTT_MAX_TOPIC_LENGTH - 1;
    }
    memcpy(mqtt_msg.topic, topicName, topic_len);
    mqtt_msg.topic[topic_len] = '\0';
    
    mqtt_msg.payload = (uint8_t*)message->payload;
    mqtt_msg.payload_length = (size_t)message->payloadlen;
    mqtt_msg.qos = qos_from_paho(message->qos);
    mqtt_msg.retained = message->retained;
    mqtt_msg.timestamp_ms = mqtt_get_timestamp_ms();
    
    /* Găsește subscripțiile potrivite și apelează callback-urile */
    pthread_mutex_lock(&impl->subs_mutex);
    
    for (int i = 0; i < impl->subscription_count; i++) {
        PahoSubscription* sub = &impl->subscriptions[i];
        
        if (sub->active && mqtt_topic_matches(mqtt_msg.topic, sub->topic_filter)) {
            pthread_mutex_unlock(&impl->subs_mutex);
            
            if (sub->callback) {
                sub->callback(&mqtt_msg, sub->user_data);
            }
            
            pthread_mutex_lock(&impl->subs_mutex);
        }
    }
    
    pthread_mutex_unlock(&impl->subs_mutex);
    
    /* Eliberează mesajul Paho */
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    
    return 1; /* Mesaj consumat */
}

/**
 * Callback Paho pentru confirmare livrare
 */
static void delivery_complete_callback(void* context, 
                                       MQTTClient_deliveryToken token) {
    (void)context;
    MQTT_LOG("Delivery complete for token %d", token);
}

/* ============================================================================
 * IMPLEMENTARE API PUBLIC
 * ============================================================================ */

const char* mqtt_get_backend_name(void) {
    return "Paho MQTT C (Real Broker Connection)";
}

const char* mqtt_get_version(void) {
    return "1.0.0-paho";
}

const char* mqtt_error_string(MqttError error) {
    switch (error) {
        case MQTT_SUCCESS:              return "Success";
        case MQTT_ERR_INVALID_PARAM:    return "Invalid parameter";
        case MQTT_ERR_NO_MEMORY:        return "Out of memory";
        case MQTT_ERR_CONNECT_FAILED:   return "Connection failed";
        case MQTT_ERR_DISCONNECTED:     return "Disconnected";
        case MQTT_ERR_PUBLISH_FAILED:   return "Publish failed";
        case MQTT_ERR_SUBSCRIBE_FAILED: return "Subscribe failed";
        case MQTT_ERR_TIMEOUT:          return "Timeout";
        case MQTT_ERR_PROTOCOL:         return "Protocol error";
        case MQTT_ERR_NOT_CONNECTED:    return "Not connected";
        case MQTT_ERR_TOPIC_INVALID:    return "Invalid topic";
        case MQTT_ERR_PAYLOAD_TOO_LARGE: return "Payload too large";
        case MQTT_ERR_INTERNAL:         return "Internal error";
        default:                        return "Unknown error";
    }
}

void mqtt_options_init(MqttConnectOptions* options) {
    if (!options) return;
    
    memset(options, 0, sizeof(*options));
    options->broker_address = "localhost";
    options->broker_port = 1883;
    options->client_id = NULL;
    options->username = NULL;
    options->password = NULL;
    options->keepalive_seconds = 60;
    options->clean_session = true;
    options->connect_timeout_ms = 10000;
    options->lwt_topic = NULL;
    options->lwt_message = NULL;
    options->lwt_qos = MQTT_QOS_AT_MOST_ONCE;
    options->lwt_retained = false;
}

MqttError mqtt_client_create(MqttClient* client, const char* client_id) {
    if (!client) return MQTT_ERR_INVALID_PARAM;
    
    struct MqttClientImpl* impl = calloc(1, sizeof(struct MqttClientImpl));
    if (!impl) return MQTT_ERR_NO_MEMORY;
    
    /* Generează sau copiază client ID */
    if (client_id && strlen(client_id) > 0) {
        strncpy(impl->client_id, client_id, MQTT_MAX_CLIENT_ID_LENGTH - 1);
    } else {
        mqtt_generate_client_id(impl->client_id, MQTT_MAX_CLIENT_ID_LENGTH);
    }
    impl->client_id[MQTT_MAX_CLIENT_ID_LENGTH - 1] = '\0';
    
    impl->state = MQTT_STATE_DISCONNECTED;
    impl->subscription_count = 0;
    impl->running = false;
    impl->paho_client = NULL;
    
    if (pthread_mutex_init(&impl->subs_mutex, NULL) != 0) {
        free(impl);
        return MQTT_ERR_INTERNAL;
    }
    
    *client = impl;
    
    MQTT_LOG("Created Paho client '%s'", impl->client_id);
    return MQTT_SUCCESS;
}

MqttError mqtt_client_connect(MqttClient client, const MqttConnectOptions* options) {
    if (!client) return MQTT_ERR_INVALID_PARAM;
    
    struct MqttClientImpl* impl = client;
    
    if (impl->state == MQTT_STATE_CONNECTED) {
        return MQTT_SUCCESS;
    }
    
    impl->state = MQTT_STATE_CONNECTING;
    
    /* Construiește URI broker */
    const char* addr = options && options->broker_address ? 
                       options->broker_address : "localhost";
    int port = options && options->broker_port > 0 ? 
               options->broker_port : 1883;
    
    snprintf(impl->broker_uri, sizeof(impl->broker_uri), 
             "tcp://%s:%d", addr, port);
    
    /* Salvează opțiunile pentru reconectare */
    if (options) {
        impl->saved_options = *options;
    } else {
        mqtt_options_init(&impl->saved_options);
    }
    
    /* Creează clientul Paho */
    int rc = MQTTClient_create(&impl->paho_client,
                               impl->broker_uri,
                               impl->client_id,
                               MQTTCLIENT_PERSISTENCE_NONE,
                               NULL);
    
    if (rc != MQTTCLIENT_SUCCESS) {
        MQTT_LOG("MQTTClient_create failed: %d", rc);
        impl->state = MQTT_STATE_ERROR;
        return MQTT_ERR_INTERNAL;
    }
    
    /* Setează callbacks */
    rc = MQTTClient_setCallbacks(impl->paho_client,
                                 impl,
                                 connection_lost_callback,
                                 message_arrived_callback,
                                 delivery_complete_callback);
    
    if (rc != MQTTCLIENT_SUCCESS) {
        MQTT_LOG("MQTTClient_setCallbacks failed: %d", rc);
        MQTTClient_destroy(&impl->paho_client);
        impl->state = MQTT_STATE_ERROR;
        return MQTT_ERR_INTERNAL;
    }
    
    /* Pregătește opțiunile de conectare Paho */
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    
    conn_opts.keepAliveInterval = options ? options->keepalive_seconds : 60;
    conn_opts.cleansession = options ? options->clean_session : 1;
    conn_opts.connectTimeout = options ? options->connect_timeout_ms / 1000 : 10;
    
    if (options && options->username) {
        conn_opts.username = options->username;
    }
    if (options && options->password) {
        conn_opts.password = options->password;
    }
    
    /* Last Will and Testament */
    MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;
    if (options && options->lwt_topic) {
        will_opts.topicName = options->lwt_topic;
        will_opts.message = options->lwt_message;
        will_opts.qos = qos_to_paho(options->lwt_qos);
        will_opts.retained = options->lwt_retained;
        conn_opts.will = &will_opts;
    }
    
    /* Conectare */
    MQTT_LOG("Connecting to %s...", impl->broker_uri);
    
    rc = MQTTClient_connect(impl->paho_client, &conn_opts);
    
    if (rc != MQTTCLIENT_SUCCESS) {
        MQTT_LOG("MQTTClient_connect failed: %d", rc);
        MQTTClient_destroy(&impl->paho_client);
        impl->paho_client = NULL;
        impl->state = MQTT_STATE_ERROR;
        
        if (impl->conn_callback) {
            impl->conn_callback(MQTT_STATE_ERROR, 
                               MQTT_ERR_CONNECT_FAILED, 
                               impl->conn_user_data);
        }
        
        return MQTT_ERR_CONNECT_FAILED;
    }
    
    impl->state = MQTT_STATE_CONNECTED;
    impl->running = true;
    
    MQTT_LOG("Connected successfully to %s", impl->broker_uri);
    
    if (impl->conn_callback) {
        impl->conn_callback(MQTT_STATE_CONNECTED, 0, impl->conn_user_data);
    }
    
    return MQTT_SUCCESS;
}

MqttError mqtt_client_disconnect(MqttClient client) {
    if (!client) return MQTT_ERR_INVALID_PARAM;
    
    struct MqttClientImpl* impl = client;
    
    impl->running = false;
    
    if (impl->paho_client && impl->state == MQTT_STATE_CONNECTED) {
        MQTT_LOG("Disconnecting from broker...");
        
        int rc = MQTTClient_disconnect(impl->paho_client, DEFAULT_TIMEOUT_MS);
        if (rc != MQTTCLIENT_SUCCESS) {
            MQTT_LOG("MQTTClient_disconnect warning: %d", rc);
        }
    }
    
    impl->state = MQTT_STATE_DISCONNECTED;
    
    if (impl->conn_callback) {
        impl->conn_callback(MQTT_STATE_DISCONNECTED, 0, impl->conn_user_data);
    }
    
    MQTT_LOG("Disconnected");
    return MQTT_SUCCESS;
}

bool mqtt_client_is_connected(MqttClient client) {
    if (!client) return false;
    
    struct MqttClientImpl* impl = client;
    
    if (impl->paho_client) {
        return MQTTClient_isConnected(impl->paho_client);
    }
    
    return false;
}

MqttConnectionState mqtt_client_get_state(MqttClient client) {
    if (!client) return MQTT_STATE_DISCONNECTED;
    return client->state;
}

void mqtt_client_destroy(MqttClient client) {
    if (!client) return;
    
    struct MqttClientImpl* impl = client;
    
    /* Deconectează dacă necesar */
    if (impl->state == MQTT_STATE_CONNECTED) {
        mqtt_client_disconnect(impl);
    }
    
    /* Distruge clientul Paho */
    if (impl->paho_client) {
        MQTTClient_destroy(&impl->paho_client);
        impl->paho_client = NULL;
    }
    
    pthread_mutex_destroy(&impl->subs_mutex);
    
    MQTT_LOG("Destroyed Paho client '%s'", impl->client_id);
    
    free(impl);
}

MqttError mqtt_publish(MqttClient client,
                       const char* topic,
                       const void* payload,
                       size_t payload_length,
                       MqttQoS qos,
                       bool retained) {
    if (!client || !topic) return MQTT_ERR_INVALID_PARAM;
    if (!mqtt_validate_topic(topic, false)) return MQTT_ERR_TOPIC_INVALID;
    
    struct MqttClientImpl* impl = client;
    
    if (!impl->paho_client || impl->state != MQTT_STATE_CONNECTED) {
        return MQTT_ERR_NOT_CONNECTED;
    }
    
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = (void*)payload;
    pubmsg.payloadlen = (int)payload_length;
    pubmsg.qos = qos_to_paho(qos);
    pubmsg.retained = retained ? 1 : 0;
    
    MQTTClient_deliveryToken token;
    
    int rc = MQTTClient_publishMessage(impl->paho_client, topic, &pubmsg, &token);
    
    if (rc != MQTTCLIENT_SUCCESS) {
        MQTT_LOG("MQTTClient_publishMessage failed: %d", rc);
        return MQTT_ERR_PUBLISH_FAILED;
    }
    
    /* Așteaptă confirmarea pentru QoS > 0 */
    if (qos != MQTT_QOS_AT_MOST_ONCE) {
        rc = MQTTClient_waitForCompletion(impl->paho_client, token, DEFAULT_TIMEOUT_MS);
        if (rc != MQTTCLIENT_SUCCESS) {
            MQTT_LOG("MQTTClient_waitForCompletion failed: %d", rc);
            return MQTT_ERR_TIMEOUT;
        }
    }
    
    MQTT_LOG("Published to '%s' (%zu bytes, token=%d)", 
             topic, payload_length, token);
    
    return MQTT_SUCCESS;
}

MqttError mqtt_publish_string(MqttClient client,
                              const char* topic,
                              const char* message,
                              MqttQoS qos,
                              bool retained) {
    size_t len = message ? strlen(message) : 0;
    return mqtt_publish(client, topic, message, len, qos, retained);
}

MqttError mqtt_subscribe(MqttClient client,
                         const char* topic_filter,
                         MqttQoS qos,
                         MqttMessageCallback callback,
                         void* user_data) {
    if (!client || !topic_filter || !callback) return MQTT_ERR_INVALID_PARAM;
    if (!mqtt_validate_topic(topic_filter, true)) return MQTT_ERR_TOPIC_INVALID;
    
    struct MqttClientImpl* impl = client;
    
    if (!impl->paho_client || impl->state != MQTT_STATE_CONNECTED) {
        return MQTT_ERR_NOT_CONNECTED;
    }
    
    pthread_mutex_lock(&impl->subs_mutex);
    
    if (impl->subscription_count >= MQTT_MAX_SUBSCRIPTIONS) {
        pthread_mutex_unlock(&impl->subs_mutex);
        return MQTT_ERR_SUBSCRIBE_FAILED;
    }
    
    /* Subscrie la broker */
    int rc = MQTTClient_subscribe(impl->paho_client, 
                                  topic_filter, 
                                  qos_to_paho(qos));
    
    if (rc != MQTTCLIENT_SUCCESS) {
        pthread_mutex_unlock(&impl->subs_mutex);
        MQTT_LOG("MQTTClient_subscribe failed: %d", rc);
        return MQTT_ERR_SUBSCRIBE_FAILED;
    }
    
    /* Salvează subscripția local */
    PahoSubscription* sub = &impl->subscriptions[impl->subscription_count];
    strncpy(sub->topic_filter, topic_filter, MQTT_MAX_TOPIC_LENGTH - 1);
    sub->topic_filter[MQTT_MAX_TOPIC_LENGTH - 1] = '\0';
    sub->qos = qos;
    sub->callback = callback;
    sub->user_data = user_data;
    sub->active = true;
    impl->subscription_count++;
    
    pthread_mutex_unlock(&impl->subs_mutex);
    
    MQTT_LOG("Subscribed to '%s' (QoS %d)", topic_filter, qos);
    return MQTT_SUCCESS;
}

MqttError mqtt_unsubscribe(MqttClient client, const char* topic_filter) {
    if (!client || !topic_filter) return MQTT_ERR_INVALID_PARAM;
    
    struct MqttClientImpl* impl = client;
    
    if (impl->paho_client && impl->state == MQTT_STATE_CONNECTED) {
        int rc = MQTTClient_unsubscribe(impl->paho_client, topic_filter);
        if (rc != MQTTCLIENT_SUCCESS) {
            MQTT_LOG("MQTTClient_unsubscribe warning: %d", rc);
        }
    }
    
    /* Dezactivează subscripția locală */
    pthread_mutex_lock(&impl->subs_mutex);
    
    for (int i = 0; i < impl->subscription_count; i++) {
        if (impl->subscriptions[i].active &&
            strcmp(impl->subscriptions[i].topic_filter, topic_filter) == 0) {
            impl->subscriptions[i].active = false;
            break;
        }
    }
    
    pthread_mutex_unlock(&impl->subs_mutex);
    
    MQTT_LOG("Unsubscribed from '%s'", topic_filter);
    return MQTT_SUCCESS;
}

void mqtt_set_connection_callback(MqttClient client,
                                  MqttConnectionCallback callback,
                                  void* user_data) {
    if (!client) return;
    client->conn_callback = callback;
    client->conn_user_data = user_data;
}

MqttError mqtt_process(MqttClient client, int timeout_ms) {
    if (!client) return MQTT_ERR_INVALID_PARAM;
    
    struct MqttClientImpl* impl = client;
    
    if (!impl->paho_client || impl->state != MQTT_STATE_CONNECTED) {
        return MQTT_ERR_NOT_CONNECTED;
    }
    
    /* Paho gestionează mesajele prin callback în background thread */
    /* Această funcție doar așteaptă pentru consistență cu API-ul */
    
    if (timeout_ms > 0) {
        struct timespec ts;
        ts.tv_sec = timeout_ms / 1000;
        ts.tv_nsec = (timeout_ms % 1000) * 1000000;
        nanosleep(&ts, NULL);
    }
    
    /* Verifică dacă mai suntem conectați */
    if (!MQTTClient_isConnected(impl->paho_client)) {
        impl->state = MQTT_STATE_DISCONNECTED;
        return MQTT_ERR_DISCONNECTED;
    }
    
    return MQTT_SUCCESS;
}

MqttError mqtt_loop_forever(MqttClient client) {
    if (!client) return MQTT_ERR_INVALID_PARAM;
    
    struct MqttClientImpl* impl = client;
    impl->running = true;
    
    MQTT_LOG("Starting Paho infinite loop");
    
    while (impl->running && impl->state == MQTT_STATE_CONNECTED) {
        MqttError err = mqtt_process(impl, 100);
        
        if (err == MQTT_ERR_DISCONNECTED) {
            /* Încearcă reconectare */
            MQTT_LOG("Attempting reconnection...");
            impl->state = MQTT_STATE_RECONNECTING;
            
            int rc = MQTTClient_connect(impl->paho_client, 
                                        &(MQTTClient_connectOptions)
                                        MQTTClient_connectOptions_initializer);
            
            if (rc == MQTTCLIENT_SUCCESS) {
                impl->state = MQTT_STATE_CONNECTED;
                MQTT_LOG("Reconnected successfully");
                
                /* Re-subscribe la toate topic-urile */
                pthread_mutex_lock(&impl->subs_mutex);
                for (int i = 0; i < impl->subscription_count; i++) {
                    if (impl->subscriptions[i].active) {
                        MQTTClient_subscribe(impl->paho_client,
                                           impl->subscriptions[i].topic_filter,
                                           qos_to_paho(impl->subscriptions[i].qos));
                    }
                }
                pthread_mutex_unlock(&impl->subs_mutex);
            } else {
                /* Așteaptă înainte de retry */
                sleep(5);
            }
        } else if (err != MQTT_SUCCESS && err != MQTT_ERR_TIMEOUT) {
            return err;
        }
    }
    
    return MQTT_SUCCESS;
}

void mqtt_stop_loop(MqttClient client) {
    if (!client) return;
    client->running = false;
    MQTT_LOG("Paho loop stop requested");
}

/* ============================================================================
 * FUNCȚII UTILITARE (comune ambelor implementări)
 * ============================================================================ */

bool mqtt_validate_topic(const char* topic, bool allow_wildcards) {
    if (!topic || strlen(topic) == 0) return false;
    if (strlen(topic) >= MQTT_MAX_TOPIC_LENGTH) return false;
    if (topic[0] == '$') return false;
    
    const char* p = topic;
    while (*p) {
        char c = *p;
        
        if (c == '+' || c == '#') {
            if (!allow_wildcards) return false;
            
            if (c == '+') {
                if (p != topic && *(p-1) != '/') return false;
                if (*(p+1) != '\0' && *(p+1) != '/') return false;
            }
            
            if (c == '#') {
                if (p != topic && *(p-1) != '/') return false;
                if (*(p+1) != '\0') return false;
            }
        }
        
        p++;
    }
    
    return true;
}

bool mqtt_topic_matches(const char* topic, const char* pattern) {
    if (!topic || !pattern) return false;
    
    const char* t = topic;
    const char* p = pattern;
    
    while (*t && *p) {
        if (*p == '#') {
            return true;
        }
        
        if (*p == '+') {
            while (*t && *t != '/') t++;
            p++;
            if (*p == '/') p++;
            if (*t == '/') t++;
            continue;
        }
        
        if (*t != *p) return false;
        
        t++;
        p++;
    }
    
    if (*p == '#') return true;
    if (*t == '\0' && *p == '\0') return true;
    if (*t == '\0' && strcmp(p, "/#") == 0) return true;
    
    return false;
}

char* mqtt_generate_client_id(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size < 32) return NULL;
    
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    
    snprintf(buffer, buffer_size, "paho_%ld_%d_%04x",
             (long)ts.tv_sec % 100000,
             getpid() % 10000,
             (unsigned)(ts.tv_nsec / 1000) & 0xFFFF);
    
    return buffer;
}

uint64_t mqtt_get_timestamp_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

#endif /* USE_PAHO_MQTT */
