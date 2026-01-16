/**
 * ============================================================================
 * MQTT POSIX SIMULATION - Implementare Fără Dependențe Externe
 * ============================================================================
 * 
 * Săptămâna 19: Algoritmi pentru IoT și Stream Processing
 * Curs: Algoritmi și Tehnici de Programare, ASE București
 * 
 * DESCRIERE:
 *   Implementare completă a abstractizării MQTT folosind exclusiv
 *   primitive POSIX: shared memory, semafoare, semnale, pipes.
 *   
 *   NU necesită broker extern - simulează comunicația între procese
 *   pe aceeași mașină folosind IPC (Inter-Process Communication).
 * 
 * ARHITECTURĂ:
 *   +------------------+     +------------------+
 *   |   Publisher 1    |     |   Subscriber 1   |
 *   +--------+---------+     +--------+---------+
 *            |                        |
 *            v                        v
 *   +------------------------------------------+
 *   |         SHARED MEMORY BROKER             |
 *   |  +------------------------------------+  |
 *   |  |  Topic Registry (array topics)     |  |
 *   |  |  Message Queue (ring buffer)       |  |
 *   |  |  Subscription Table               |  |
 *   |  +------------------------------------+  |
 *   +------------------------------------------+
 *            |                        |
 *            v                        v
 *   +--------+---------+     +--------+---------+
 *   |   Publisher 2    |     |   Subscriber 2   |
 *   +------------------+     +------------------+
 * 
 * MECANISME POSIX UTILIZATE:
 *   1. shm_open() / mmap()     - Memorie partajată pentru mesaje
 *   2. sem_open() / sem_wait() - Sincronizare acces
 *   3. Named pipes (FIFO)      - Notificare evenimente (alternativă)
 *   4. Signals (SIGUSR1)       - Notificare mesaje noi
 * 
 * LIMITĂRI:
 *   - Funcționează doar local (aceeași mașină)
 *   - Nu suportă QoS real (simulat)
 *   - Maximum 16 clienți simultani
 *   - Maximum 64 topic-uri active
 * 
 * COMPILARE:
 *   gcc -o program program.c mqtt_posix.c -lpthread -lrt
 * 
 * AUTOR: Generated for ASE-CSIE ATP Course
 * VERSIUNE: 1.0 (Ianuarie 2026)
 * ============================================================================
 */

#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include "mqtt_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>

/* ============================================================================
 * CONSTANTE ȘI CONFIGURARE
 * ============================================================================ */

/** Nume shared memory și semafoare */
#define SHM_NAME            "/mqtt_sim_broker"
#define SEM_BROKER_NAME     "/mqtt_sim_broker_sem"
#define SEM_MSG_NAME        "/mqtt_sim_msg_sem"

/** Limite structuri */
#define MAX_CLIENTS         16
#define MAX_TOPICS          64
#define MAX_MESSAGES        256
#define MAX_SUBSCRIPTIONS_PER_CLIENT 32

/** Dimensiune maximă partajată */
#define SHARED_SIZE (sizeof(SharedBroker))

/* ============================================================================
 * STRUCTURI INTERNE
 * ============================================================================ */

/**
 * Mesaj în coada partajată
 */
typedef struct {
    bool valid;                              /**< Slot ocupat */
    char topic[MQTT_MAX_TOPIC_LENGTH];       /**< Topic-ul mesajului */
    uint8_t payload[4096];                   /**< Payload (limitat pentru shm) */
    size_t payload_length;                   /**< Lungime payload */
    MqttQoS qos;                             /**< QoS solicitat */
    bool retained;                           /**< Flag retained */
    uint64_t timestamp_ms;                   /**< Timestamp publicare */
    uint64_t sequence;                       /**< Număr secvență global */
} SharedMessage;

/**
 * Înregistrare topic retained
 */
typedef struct {
    bool valid;
    char topic[MQTT_MAX_TOPIC_LENGTH];
    uint8_t payload[4096];
    size_t payload_length;
    MqttQoS qos;
    uint64_t timestamp_ms;
} RetainedMessage;

/**
 * Informații client în broker
 */
typedef struct {
    bool connected;
    char client_id[MQTT_MAX_CLIENT_ID_LENGTH];
    pid_t pid;                               /**< PID pentru semnale */
    uint64_t last_seen;                      /**< Timestamp ultima activitate */
    char subscriptions[MAX_SUBSCRIPTIONS_PER_CLIENT][MQTT_MAX_TOPIC_LENGTH];
    int subscription_count;
    uint64_t last_processed_seq;             /**< Ultima secvență procesată */
} ClientInfo;

/**
 * Broker partajat în memorie
 */
typedef struct {
    /* Metadata */
    uint32_t magic;                          /**< Magic number pentru validare */
    uint64_t sequence_counter;               /**< Contor secvență mesaje */
    
    /* Clienți */
    ClientInfo clients[MAX_CLIENTS];
    int client_count;
    
    /* Mesaje (ring buffer) */
    SharedMessage messages[MAX_MESSAGES];
    int msg_head;                            /**< Index scriere */
    int msg_tail;                            /**< Index citire (nu se folosește) */
    
    /* Retained messages */
    RetainedMessage retained[MAX_TOPICS];
} SharedBroker;

/**
 * Înregistrare subscripție locală
 */
typedef struct {
    char topic_filter[MQTT_MAX_TOPIC_LENGTH];
    MqttQoS qos;
    MqttMessageCallback callback;
    void* user_data;
} LocalSubscription;

/**
 * Implementarea clientului MQTT
 */
struct MqttClientImpl {
    /* Identificare */
    char client_id[MQTT_MAX_CLIENT_ID_LENGTH];
    int client_index;                        /**< Index în array clienți broker */
    
    /* Stare conexiune */
    MqttConnectionState state;
    bool running;                            /**< Flag pentru loop */
    
    /* Shared memory */
    int shm_fd;
    SharedBroker* broker;
    sem_t* sem_broker;
    sem_t* sem_msg;
    
    /* Subscripții locale */
    LocalSubscription subscriptions[MQTT_MAX_SUBSCRIPTIONS];
    int subscription_count;
    
    /* Callbacks */
    MqttConnectionCallback conn_callback;
    void* conn_user_data;
    
    /* Tracking procesare */
    uint64_t last_processed_seq;
    
    /* Thread pentru polling (opțional) */
    pthread_t poll_thread;
    bool poll_thread_active;
};

/* ============================================================================
 * VARIABILE GLOBALE
 * ============================================================================ */

static volatile sig_atomic_t g_signal_received = 0;
static const uint32_t BROKER_MAGIC = 0x4D515454; /* "MQTT" */

/* ============================================================================
 * FUNCȚII HELPER INTERNE
 * ============================================================================ */

/**
 * Handler pentru SIGUSR1 (notificare mesaj nou)
 */
static void signal_handler(int sig) {
    if (sig == SIGUSR1) {
        g_signal_received = 1;
    }
}

/**
 * Configurează handler-ul de semnal
 */
static void setup_signal_handler(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);
}

/**
 * Inițializează sau atașează la broker-ul partajat
 */
static MqttError init_shared_broker(struct MqttClientImpl* client) {
    bool created = false;
    
    /* Încearcă să deschidă shared memory existentă */
    client->shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    
    if (client->shm_fd == -1) {
        /* Nu există, creăm una nouă */
        client->shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
        if (client->shm_fd == -1) {
            MQTT_LOG("shm_open failed: %s", strerror(errno));
            return MQTT_ERR_INTERNAL;
        }
        
        /* Setează dimensiunea */
        if (ftruncate(client->shm_fd, SHARED_SIZE) == -1) {
            MQTT_LOG("ftruncate failed: %s", strerror(errno));
            close(client->shm_fd);
            shm_unlink(SHM_NAME);
            return MQTT_ERR_INTERNAL;
        }
        
        created = true;
    }
    
    /* Mapează în spațiul de adrese */
    client->broker = mmap(NULL, SHARED_SIZE, PROT_READ | PROT_WRITE,
                          MAP_SHARED, client->shm_fd, 0);
    
    if (client->broker == MAP_FAILED) {
        MQTT_LOG("mmap failed: %s", strerror(errno));
        close(client->shm_fd);
        if (created) shm_unlink(SHM_NAME);
        return MQTT_ERR_INTERNAL;
    }
    
    /* Deschide sau creează semafor broker */
    client->sem_broker = sem_open(SEM_BROKER_NAME, O_CREAT, 0666, 1);
    if (client->sem_broker == SEM_FAILED) {
        MQTT_LOG("sem_open broker failed: %s", strerror(errno));
        munmap(client->broker, SHARED_SIZE);
        close(client->shm_fd);
        if (created) shm_unlink(SHM_NAME);
        return MQTT_ERR_INTERNAL;
    }
    
    /* Deschide sau creează semafor mesaje */
    client->sem_msg = sem_open(SEM_MSG_NAME, O_CREAT, 0666, 0);
    if (client->sem_msg == SEM_FAILED) {
        MQTT_LOG("sem_open msg failed: %s", strerror(errno));
        sem_close(client->sem_broker);
        munmap(client->broker, SHARED_SIZE);
        close(client->shm_fd);
        if (created) shm_unlink(SHM_NAME);
        return MQTT_ERR_INTERNAL;
    }
    
    /* Inițializează broker-ul dacă tocmai l-am creat */
    if (created) {
        sem_wait(client->sem_broker);
        
        memset(client->broker, 0, SHARED_SIZE);
        client->broker->magic = BROKER_MAGIC;
        client->broker->sequence_counter = 1;
        client->broker->client_count = 0;
        client->broker->msg_head = 0;
        
        sem_post(client->sem_broker);
        
        MQTT_LOG("Created new shared broker");
    } else {
        /* Verifică magic number */
        if (client->broker->magic != BROKER_MAGIC) {
            MQTT_LOG("Invalid broker magic: 0x%X", client->broker->magic);
            sem_close(client->sem_msg);
            sem_close(client->sem_broker);
            munmap(client->broker, SHARED_SIZE);
            close(client->shm_fd);
            return MQTT_ERR_INTERNAL;
        }
        MQTT_LOG("Attached to existing broker");
    }
    
    return MQTT_SUCCESS;
}

/**
 * Înregistrează clientul în broker
 */
static MqttError register_client(struct MqttClientImpl* client) {
    sem_wait(client->sem_broker);
    
    /* Caută slot liber sau client existent */
    int slot = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!client->broker->clients[i].connected) {
            if (slot == -1) slot = i;
        } else if (strcmp(client->broker->clients[i].client_id, 
                          client->client_id) == 0) {
            /* Client existent - refolosește slot-ul */
            slot = i;
            break;
        }
    }
    
    if (slot == -1) {
        sem_post(client->sem_broker);
        MQTT_LOG("No available client slots");
        return MQTT_ERR_CONNECT_FAILED;
    }
    
    /* Înregistrează clientul */
    ClientInfo* info = &client->broker->clients[slot];
    info->connected = true;
    strncpy(info->client_id, client->client_id, MQTT_MAX_CLIENT_ID_LENGTH - 1);
    info->client_id[MQTT_MAX_CLIENT_ID_LENGTH - 1] = '\0';
    info->pid = getpid();
    info->last_seen = mqtt_get_timestamp_ms();
    info->subscription_count = 0;
    info->last_processed_seq = client->broker->sequence_counter;
    
    client->client_index = slot;
    client->last_processed_seq = info->last_processed_seq;
    client->broker->client_count++;
    
    sem_post(client->sem_broker);
    
    MQTT_LOG("Registered client '%s' in slot %d", client->client_id, slot);
    return MQTT_SUCCESS;
}

/**
 * Deînregistrează clientul din broker
 */
static void unregister_client(struct MqttClientImpl* client) {
    if (!client || !client->broker || client->client_index < 0) return;
    
    sem_wait(client->sem_broker);
    
    if (client->client_index < MAX_CLIENTS) {
        ClientInfo* info = &client->broker->clients[client->client_index];
        if (info->connected && 
            strcmp(info->client_id, client->client_id) == 0) {
            info->connected = false;
            info->subscription_count = 0;
            client->broker->client_count--;
        }
    }
    
    sem_post(client->sem_broker);
    
    MQTT_LOG("Unregistered client '%s'", client->client_id);
}

/**
 * Notifică toți subscriberii unui topic
 */
static void notify_subscribers(SharedBroker* broker, const char* topic) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!broker->clients[i].connected) continue;
        
        ClientInfo* info = &broker->clients[i];
        
        /* Verifică subscripțiile */
        for (int j = 0; j < info->subscription_count; j++) {
            if (mqtt_topic_matches(topic, info->subscriptions[j])) {
                /* Trimite semnal SIGUSR1 */
                if (info->pid > 0) {
                    kill(info->pid, SIGUSR1);
                }
                break;
            }
        }
    }
}

/**
 * Procesează mesajele noi pentru acest client
 */
static void process_new_messages(struct MqttClientImpl* client) {
    if (!client || !client->broker) return;
    
    sem_wait(client->sem_broker);
    
    uint64_t current_seq = client->broker->sequence_counter;
    
    /* Parcurge mesajele noi */
    for (int i = 0; i < MAX_MESSAGES; i++) {
        SharedMessage* msg = &client->broker->messages[i];
        
        if (!msg->valid) continue;
        if (msg->sequence <= client->last_processed_seq) continue;
        if (msg->sequence > current_seq) continue;
        
        /* Verifică dacă suntem subscriși la acest topic */
        for (int j = 0; j < client->subscription_count; j++) {
            LocalSubscription* sub = &client->subscriptions[j];
            
            if (mqtt_topic_matches(msg->topic, sub->topic_filter)) {
                /* Construiește mesajul pentru callback */
                MqttMessage mqtt_msg;
                strncpy(mqtt_msg.topic, msg->topic, MQTT_MAX_TOPIC_LENGTH - 1);
                mqtt_msg.topic[MQTT_MAX_TOPIC_LENGTH - 1] = '\0';
                mqtt_msg.payload = msg->payload;
                mqtt_msg.payload_length = msg->payload_length;
                mqtt_msg.qos = msg->qos;
                mqtt_msg.retained = msg->retained;
                mqtt_msg.timestamp_ms = msg->timestamp_ms;
                
                sem_post(client->sem_broker);
                
                /* Apelează callback-ul (în afara lock-ului!) */
                if (sub->callback) {
                    sub->callback(&mqtt_msg, sub->user_data);
                }
                
                sem_wait(client->sem_broker);
                break; /* Un mesaj - un callback per topic match */
            }
        }
    }
    
    client->last_processed_seq = current_seq;
    
    /* Actualizează și în broker */
    if (client->client_index >= 0 && client->client_index < MAX_CLIENTS) {
        client->broker->clients[client->client_index].last_processed_seq = 
            current_seq;
        client->broker->clients[client->client_index].last_seen = 
            mqtt_get_timestamp_ms();
    }
    
    sem_post(client->sem_broker);
}

/* ============================================================================
 * IMPLEMENTARE API PUBLIC
 * ============================================================================ */

const char* mqtt_get_backend_name(void) {
    return "POSIX Simulation (Shared Memory + Signals)";
}

const char* mqtt_get_version(void) {
    return "1.0.0-posix";
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
    options->client_id = NULL;  /* Auto-generate */
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
    impl->client_index = -1;
    impl->shm_fd = -1;
    impl->broker = NULL;
    impl->sem_broker = SEM_FAILED;
    impl->sem_msg = SEM_FAILED;
    impl->subscription_count = 0;
    impl->running = false;
    impl->poll_thread_active = false;
    
    /* Configurează signal handler */
    setup_signal_handler();
    
    *client = impl;
    
    MQTT_LOG("Created client '%s'", impl->client_id);
    return MQTT_SUCCESS;
}

MqttError mqtt_client_connect(MqttClient client, const MqttConnectOptions* options) {
    if (!client) return MQTT_ERR_INVALID_PARAM;
    
    struct MqttClientImpl* impl = client;
    
    if (impl->state == MQTT_STATE_CONNECTED) {
        return MQTT_SUCCESS; /* Deja conectat */
    }
    
    impl->state = MQTT_STATE_CONNECTING;
    
    /* Inițializează shared memory */
    MqttError err = init_shared_broker(impl);
    if (err != MQTT_SUCCESS) {
        impl->state = MQTT_STATE_ERROR;
        if (impl->conn_callback) {
            impl->conn_callback(MQTT_STATE_ERROR, err, impl->conn_user_data);
        }
        return err;
    }
    
    /* Înregistrează clientul */
    err = register_client(impl);
    if (err != MQTT_SUCCESS) {
        munmap(impl->broker, SHARED_SIZE);
        close(impl->shm_fd);
        impl->state = MQTT_STATE_ERROR;
        if (impl->conn_callback) {
            impl->conn_callback(MQTT_STATE_ERROR, err, impl->conn_user_data);
        }
        return err;
    }
    
    impl->state = MQTT_STATE_CONNECTED;
    impl->running = true;
    
    if (impl->conn_callback) {
        impl->conn_callback(MQTT_STATE_CONNECTED, 0, impl->conn_user_data);
    }
    
    MQTT_LOG("Connected to simulated broker");
    return MQTT_SUCCESS;
}

MqttError mqtt_client_disconnect(MqttClient client) {
    if (!client) return MQTT_ERR_INVALID_PARAM;
    
    struct MqttClientImpl* impl = client;
    
    impl->running = false;
    
    /* Deînregistrează din broker */
    unregister_client(impl);
    
    /* Eliberează resursele */
    if (impl->sem_msg != SEM_FAILED) {
        sem_close(impl->sem_msg);
        impl->sem_msg = SEM_FAILED;
    }
    
    if (impl->sem_broker != SEM_FAILED) {
        sem_close(impl->sem_broker);
        impl->sem_broker = SEM_FAILED;
    }
    
    if (impl->broker) {
        munmap(impl->broker, SHARED_SIZE);
        impl->broker = NULL;
    }
    
    if (impl->shm_fd >= 0) {
        close(impl->shm_fd);
        impl->shm_fd = -1;
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
    return client->state == MQTT_STATE_CONNECTED;
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
    
    /* Curăță subscripțiile */
    impl->subscription_count = 0;
    
    MQTT_LOG("Destroyed client '%s'", impl->client_id);
    
    free(impl);
}

MqttError mqtt_publish(MqttClient client,
                       const char* topic,
                       const void* payload,
                       size_t payload_length,
                       MqttQoS qos,
                       bool retained) {
    if (!client || !topic) return MQTT_ERR_INVALID_PARAM;
    if (payload_length > 4096) return MQTT_ERR_PAYLOAD_TOO_LARGE;
    if (!mqtt_validate_topic(topic, false)) return MQTT_ERR_TOPIC_INVALID;
    
    struct MqttClientImpl* impl = client;
    
    if (impl->state != MQTT_STATE_CONNECTED) {
        return MQTT_ERR_NOT_CONNECTED;
    }
    
    sem_wait(impl->sem_broker);
    
    /* Găsește slot în ring buffer */
    int slot = impl->broker->msg_head;
    impl->broker->msg_head = (slot + 1) % MAX_MESSAGES;
    
    /* Scrie mesajul */
    SharedMessage* msg = &impl->broker->messages[slot];
    msg->valid = true;
    strncpy(msg->topic, topic, MQTT_MAX_TOPIC_LENGTH - 1);
    msg->topic[MQTT_MAX_TOPIC_LENGTH - 1] = '\0';
    
    if (payload && payload_length > 0) {
        memcpy(msg->payload, payload, payload_length);
        msg->payload_length = payload_length;
    } else {
        msg->payload_length = 0;
    }
    
    msg->qos = qos;
    msg->retained = retained;
    msg->timestamp_ms = mqtt_get_timestamp_ms();
    msg->sequence = impl->broker->sequence_counter++;
    
    /* Salvează ca retained dacă solicitat */
    if (retained) {
        /* Caută sau creează slot pentru retained */
        int ret_slot = -1;
        for (int i = 0; i < MAX_TOPICS; i++) {
            if (!impl->broker->retained[i].valid) {
                if (ret_slot == -1) ret_slot = i;
            } else if (strcmp(impl->broker->retained[i].topic, topic) == 0) {
                ret_slot = i;
                break;
            }
        }
        
        if (ret_slot >= 0) {
            RetainedMessage* ret = &impl->broker->retained[ret_slot];
            ret->valid = true;
            strncpy(ret->topic, topic, MQTT_MAX_TOPIC_LENGTH - 1);
            if (payload && payload_length > 0) {
                memcpy(ret->payload, payload, payload_length);
                ret->payload_length = payload_length;
            } else {
                ret->payload_length = 0;
            }
            ret->qos = qos;
            ret->timestamp_ms = msg->timestamp_ms;
        }
    }
    
    /* Notifică subscriberii */
    notify_subscribers(impl->broker, topic);
    
    sem_post(impl->sem_broker);
    
    MQTT_LOG("Published to '%s' (%zu bytes, seq=%lu)", 
             topic, payload_length, (unsigned long)msg->sequence);
    
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
    
    if (impl->state != MQTT_STATE_CONNECTED) {
        return MQTT_ERR_NOT_CONNECTED;
    }
    
    if (impl->subscription_count >= MQTT_MAX_SUBSCRIPTIONS) {
        return MQTT_ERR_SUBSCRIBE_FAILED;
    }
    
    /* Adaugă subscripție locală */
    LocalSubscription* sub = &impl->subscriptions[impl->subscription_count];
    strncpy(sub->topic_filter, topic_filter, MQTT_MAX_TOPIC_LENGTH - 1);
    sub->topic_filter[MQTT_MAX_TOPIC_LENGTH - 1] = '\0';
    sub->qos = qos;
    sub->callback = callback;
    sub->user_data = user_data;
    impl->subscription_count++;
    
    /* Înregistrează în broker pentru notificări */
    sem_wait(impl->sem_broker);
    
    if (impl->client_index >= 0 && impl->client_index < MAX_CLIENTS) {
        ClientInfo* info = &impl->broker->clients[impl->client_index];
        if (info->subscription_count < MAX_SUBSCRIPTIONS_PER_CLIENT) {
            strncpy(info->subscriptions[info->subscription_count],
                    topic_filter, MQTT_MAX_TOPIC_LENGTH - 1);
            info->subscription_count++;
        }
    }
    
    /* Livrează mesaje retained care se potrivesc */
    for (int i = 0; i < MAX_TOPICS; i++) {
        RetainedMessage* ret = &impl->broker->retained[i];
        if (ret->valid && mqtt_topic_matches(ret->topic, topic_filter)) {
            MqttMessage mqtt_msg;
            strncpy(mqtt_msg.topic, ret->topic, MQTT_MAX_TOPIC_LENGTH - 1);
            mqtt_msg.topic[MQTT_MAX_TOPIC_LENGTH - 1] = '\0';
            mqtt_msg.payload = ret->payload;
            mqtt_msg.payload_length = ret->payload_length;
            mqtt_msg.qos = ret->qos;
            mqtt_msg.retained = true;
            mqtt_msg.timestamp_ms = ret->timestamp_ms;
            
            sem_post(impl->sem_broker);
            callback(&mqtt_msg, user_data);
            sem_wait(impl->sem_broker);
        }
    }
    
    sem_post(impl->sem_broker);
    
    MQTT_LOG("Subscribed to '%s' (QoS %d)", topic_filter, qos);
    return MQTT_SUCCESS;
}

MqttError mqtt_unsubscribe(MqttClient client, const char* topic_filter) {
    if (!client || !topic_filter) return MQTT_ERR_INVALID_PARAM;
    
    struct MqttClientImpl* impl = client;
    
    /* Șterge subscripția locală */
    for (int i = 0; i < impl->subscription_count; i++) {
        if (strcmp(impl->subscriptions[i].topic_filter, topic_filter) == 0) {
            /* Mută ultimul element pe poziția curentă */
            if (i < impl->subscription_count - 1) {
                impl->subscriptions[i] = 
                    impl->subscriptions[impl->subscription_count - 1];
            }
            impl->subscription_count--;
            break;
        }
    }
    
    /* Șterge din broker */
    sem_wait(impl->sem_broker);
    
    if (impl->client_index >= 0 && impl->client_index < MAX_CLIENTS) {
        ClientInfo* info = &impl->broker->clients[impl->client_index];
        for (int i = 0; i < info->subscription_count; i++) {
            if (strcmp(info->subscriptions[i], topic_filter) == 0) {
                if (i < info->subscription_count - 1) {
                    strcpy(info->subscriptions[i],
                           info->subscriptions[info->subscription_count - 1]);
                }
                info->subscription_count--;
                break;
            }
        }
    }
    
    sem_post(impl->sem_broker);
    
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
    
    if (impl->state != MQTT_STATE_CONNECTED) {
        return MQTT_ERR_NOT_CONNECTED;
    }
    
    /* Resetează flag-ul de semnal */
    g_signal_received = 0;
    
    if (timeout_ms > 0) {
        /* Așteaptă semnal sau timeout */
        struct timespec ts;
        ts.tv_sec = timeout_ms / 1000;
        ts.tv_nsec = (timeout_ms % 1000) * 1000000;
        nanosleep(&ts, NULL);
    }
    
    /* Procesează mesaje noi */
    process_new_messages(impl);
    
    return MQTT_SUCCESS;
}

MqttError mqtt_loop_forever(MqttClient client) {
    if (!client) return MQTT_ERR_INVALID_PARAM;
    
    struct MqttClientImpl* impl = client;
    impl->running = true;
    
    MQTT_LOG("Starting infinite loop");
    
    while (impl->running && impl->state == MQTT_STATE_CONNECTED) {
        MqttError err = mqtt_process(impl, 100);
        if (err != MQTT_SUCCESS && err != MQTT_ERR_TIMEOUT) {
            return err;
        }
    }
    
    return MQTT_SUCCESS;
}

void mqtt_stop_loop(MqttClient client) {
    if (!client) return;
    client->running = false;
    MQTT_LOG("Loop stop requested");
}

/* ============================================================================
 * FUNCȚII UTILITARE
 * ============================================================================ */

bool mqtt_validate_topic(const char* topic, bool allow_wildcards) {
    if (!topic || strlen(topic) == 0) return false;
    if (strlen(topic) >= MQTT_MAX_TOPIC_LENGTH) return false;
    if (topic[0] == '$') return false; /* Topics $ sunt rezervate */
    
    const char* p = topic;
    while (*p) {
        char c = *p;
        
        /* Caractere invalide */
        if (c == '\0') break;
        
        /* Wildcards */
        if (c == '+' || c == '#') {
            if (!allow_wildcards) return false;
            
            /* + trebuie să fie singur între / */
            if (c == '+') {
                if (p != topic && *(p-1) != '/') return false;
                if (*(p+1) != '\0' && *(p+1) != '/') return false;
            }
            
            /* # trebuie să fie la final */
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
            return true; /* # potrivește tot restul */
        }
        
        if (*p == '+') {
            /* + potrivește un nivel */
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
    
    /* Verifică terminarea */
    if (*p == '#') return true;
    if (*t == '\0' && *p == '\0') return true;
    if (*t == '\0' && strcmp(p, "/#") == 0) return true;
    
    return false;
}

char* mqtt_generate_client_id(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size < 32) return NULL;
    
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    
    snprintf(buffer, buffer_size, "posix_%ld_%d_%04x",
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

/* ============================================================================
 * FUNCȚII DE CLEANUP BROKER
 * ============================================================================ */

/**
 * Șterge broker-ul partajat (pentru cleanup/reset)
 * 
 * Apelați doar când nu mai există clienți activi!
 */
void mqtt_posix_cleanup_broker(void) {
    sem_unlink(SEM_MSG_NAME);
    sem_unlink(SEM_BROKER_NAME);
    shm_unlink(SHM_NAME);
    printf("[MQTT] Cleaned up shared broker resources\n");
}

/**
 * Afișează statistici broker (pentru debugging)
 */
void mqtt_posix_print_stats(MqttClient client) {
    if (!client || !client->broker) {
        printf("[MQTT] No broker connected\n");
        return;
    }
    
    struct MqttClientImpl* impl = client;
    
    sem_wait(impl->sem_broker);
    
    printf("\n=== MQTT POSIX Broker Statistics ===\n");
    printf("Sequence counter: %lu\n", 
           (unsigned long)impl->broker->sequence_counter);
    printf("Connected clients: %d\n", impl->broker->client_count);
    printf("Message head: %d\n", impl->broker->msg_head);
    
    printf("\nClients:\n");
    for (int i = 0; i < MAX_CLIENTS; i++) {
        ClientInfo* info = &impl->broker->clients[i];
        if (info->connected) {
            printf("  [%d] '%s' (PID %d, subs=%d, last_seq=%lu)\n",
                   i, info->client_id, info->pid,
                   info->subscription_count,
                   (unsigned long)info->last_processed_seq);
        }
    }
    
    int retained_count = 0;
    for (int i = 0; i < MAX_TOPICS; i++) {
        if (impl->broker->retained[i].valid) retained_count++;
    }
    printf("\nRetained messages: %d\n", retained_count);
    
    sem_post(impl->sem_broker);
    
    printf("====================================\n\n");
}
