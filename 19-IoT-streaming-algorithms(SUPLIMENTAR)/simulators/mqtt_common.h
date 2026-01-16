/**
 * ============================================================================
 * MQTT ABSTRACTION LAYER - Header Comun
 * ============================================================================
 * 
 * Săptămâna 19: Algoritmi pentru IoT și Stream Processing
 * Curs: Algoritmi și Tehnici de Programare, ASE București
 * 
 * DESCRIERE:
 *   Acest header definește o interfață abstractă pentru comunicația MQTT,
 *   permițând alegerea la compilare între două implementări:
 *   
 *   1. PAHO MQTT C - Conectare reală la broker Mosquitto
 *      Necesită: libpaho-mqtt3c-dev (apt install)
 *      Activare: -DUSE_PAHO_MQTT la compilare
 *   
 *   2. POSIX SIMULATION - Simulare in-memory cu pipes/shared memory
 *      Nu necesită biblioteci externe
 *      Activare: implicită (fără USE_PAHO_MQTT)
 * 
 * PATTERN ARHITECTURAL:
 *   Strategy Pattern implementat prin compilare condiționată.
 *   Toate funcțiile au aceeași semnătură indiferent de backend.
 * 
 * UTILIZARE:
 *   // Alegere la compilare:
 *   gcc -DUSE_PAHO_MQTT publisher.c mqtt_paho.c -lpaho-mqtt3c
 *   gcc publisher.c mqtt_posix.c   // fără dependențe externe
 * 
 * AUTOR: Generated for ASE-CSIE ATP Course
 * VERSIUNE: 1.0 (Ianuarie 2026)
 * LICENȚĂ: Educational Use
 * ============================================================================
 */

#ifndef MQTT_COMMON_H
#define MQTT_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ============================================================================
 * CONSTANTELE PROTOCOLULUI
 * ============================================================================ */

/** Versiunea protocolului MQTT simulat */
#define MQTT_PROTOCOL_VERSION       "3.1.1"

/** Dimensiuni maxime pentru buffere */
#define MQTT_MAX_TOPIC_LENGTH       256
#define MQTT_MAX_PAYLOAD_SIZE       65536
#define MQTT_MAX_CLIENT_ID_LENGTH   64
#define MQTT_MAX_SUBSCRIPTIONS      32

/** Timeout-uri implicite (milisecunde) */
#define MQTT_DEFAULT_KEEPALIVE_MS   60000
#define MQTT_DEFAULT_CONNECT_TIMEOUT_MS 10000
#define MQTT_DEFAULT_PUBLISH_TIMEOUT_MS 5000

/** Niveluri QoS (Quality of Service) */
typedef enum {
    MQTT_QOS_AT_MOST_ONCE  = 0,  /**< Fire and forget - fără confirmare */
    MQTT_QOS_AT_LEAST_ONCE = 1,  /**< Garantează livrarea, posibile duplicate */
    MQTT_QOS_EXACTLY_ONCE  = 2   /**< Garantează livrare unică (overhead mare) */
} MqttQoS;

/** Coduri de eroare unificate */
typedef enum {
    MQTT_SUCCESS                = 0,
    MQTT_ERR_INVALID_PARAM      = -1,
    MQTT_ERR_NO_MEMORY          = -2,
    MQTT_ERR_CONNECT_FAILED     = -3,
    MQTT_ERR_DISCONNECTED       = -4,
    MQTT_ERR_PUBLISH_FAILED     = -5,
    MQTT_ERR_SUBSCRIBE_FAILED   = -6,
    MQTT_ERR_TIMEOUT            = -7,
    MQTT_ERR_PROTOCOL           = -8,
    MQTT_ERR_NOT_CONNECTED      = -9,
    MQTT_ERR_TOPIC_INVALID      = -10,
    MQTT_ERR_PAYLOAD_TOO_LARGE  = -11,
    MQTT_ERR_INTERNAL           = -99
} MqttError;

/** Stări ale conexiunii */
typedef enum {
    MQTT_STATE_DISCONNECTED = 0,
    MQTT_STATE_CONNECTING   = 1,
    MQTT_STATE_CONNECTED    = 2,
    MQTT_STATE_RECONNECTING = 3,
    MQTT_STATE_ERROR        = 4
} MqttConnectionState;

/* ============================================================================
 * STRUCTURI DE DATE
 * ============================================================================ */

/**
 * Mesaj MQTT primit
 * 
 * Conține toate informațiile despre un mesaj recepționat:
 * topic-ul, payload-ul, QoS-ul și flag-ul retained.
 */
typedef struct {
    char topic[MQTT_MAX_TOPIC_LENGTH];      /**< Topic-ul mesajului */
    uint8_t* payload;                        /**< Pointer către payload */
    size_t payload_length;                   /**< Lungimea payload-ului în bytes */
    MqttQoS qos;                             /**< Nivelul QoS */
    bool retained;                           /**< Flag retained (broker păstrează) */
    uint64_t timestamp_ms;                   /**< Timestamp recepție (epoch ms) */
} MqttMessage;

/**
 * Opțiuni de conectare
 * 
 * Configurația completă pentru stabilirea conexiunii MQTT.
 */
typedef struct {
    const char* broker_address;              /**< Adresa broker-ului (hostname/IP) */
    int broker_port;                         /**< Portul broker-ului (default 1883) */
    const char* client_id;                   /**< Identificator unic client */
    const char* username;                    /**< Username (opțional, NULL dacă nu) */
    const char* password;                    /**< Password (opțional, NULL dacă nu) */
    int keepalive_seconds;                   /**< Interval keepalive (default 60) */
    bool clean_session;                      /**< Clean session flag */
    int connect_timeout_ms;                  /**< Timeout conectare (ms) */
    
    /* Last Will and Testament (LWT) - mesaj trimis automat la deconectare */
    const char* lwt_topic;                   /**< Topic LWT (NULL pentru dezactivare) */
    const char* lwt_message;                 /**< Mesaj LWT */
    MqttQoS lwt_qos;                         /**< QoS pentru LWT */
    bool lwt_retained;                       /**< Retained flag pentru LWT */
} MqttConnectOptions;

/**
 * Callback pentru mesaje recepționate
 * 
 * @param message Pointer către mesajul primit
 * @param user_data Date utilizator transmise la subscribe
 */
typedef void (*MqttMessageCallback)(const MqttMessage* message, void* user_data);

/**
 * Callback pentru schimbări de stare conexiune
 * 
 * @param new_state Noua stare a conexiunii
 * @param reason Cod eroare (0 dacă succes)
 * @param user_data Date utilizator
 */
typedef void (*MqttConnectionCallback)(MqttConnectionState new_state, 
                                        int reason, 
                                        void* user_data);

/**
 * Handle opac pentru client MQTT
 * 
 * Implementarea internă diferă între Paho și POSIX,
 * dar API-ul extern rămâne identic.
 */
typedef struct MqttClientImpl* MqttClient;

/* ============================================================================
 * FUNCȚII DE INIȚIALIZARE ȘI CONFIGURARE
 * ============================================================================ */

/**
 * Inițializează opțiunile de conectare cu valori implicite
 * 
 * Setează valori sensibile pentru toți parametrii:
 * - broker_address: "localhost"
 * - broker_port: 1883
 * - keepalive_seconds: 60
 * - clean_session: true
 * - connect_timeout_ms: 10000
 * 
 * @param options Pointer către structura de opțiuni
 */
void mqtt_options_init(MqttConnectOptions* options);

/**
 * Returnează numele implementării curente
 * 
 * @return "Paho MQTT C" sau "POSIX Simulation"
 */
const char* mqtt_get_backend_name(void);

/**
 * Returnează versiunea bibliotecii
 * 
 * @return String cu versiunea (ex: "1.0.0")
 */
const char* mqtt_get_version(void);

/**
 * Convertește cod eroare în mesaj text
 * 
 * @param error Codul erorii
 * @return Descriere text a erorii
 */
const char* mqtt_error_string(MqttError error);

/* ============================================================================
 * FUNCȚII DE CONECTARE ȘI DECONECTARE
 * ============================================================================ */

/**
 * Creează un nou client MQTT
 * 
 * Alocă și inițializează structurile interne pentru client.
 * Clientul nu este încă conectat după această operație.
 * 
 * @param client Pointer către handle-ul clientului (output)
 * @param client_id Identificator unic (NULL pentru generare automată)
 * @return MQTT_SUCCESS sau cod eroare
 * 
 * @note Clientul trebuie eliberat cu mqtt_client_destroy()
 */
MqttError mqtt_client_create(MqttClient* client, const char* client_id);

/**
 * Conectează clientul la broker
 * 
 * Stabilește conexiunea TCP și efectuează handshake-ul MQTT.
 * Operația este blocantă până la succes sau timeout.
 * 
 * @param client Handle-ul clientului
 * @param options Opțiuni de conectare
 * @return MQTT_SUCCESS sau cod eroare
 */
MqttError mqtt_client_connect(MqttClient client, const MqttConnectOptions* options);

/**
 * Deconectează clientul de la broker
 * 
 * Trimite pachet DISCONNECT și închide conexiunea grațios.
 * 
 * @param client Handle-ul clientului
 * @return MQTT_SUCCESS sau cod eroare
 */
MqttError mqtt_client_disconnect(MqttClient client);

/**
 * Verifică dacă clientul este conectat
 * 
 * @param client Handle-ul clientului
 * @return true dacă conectat, false altfel
 */
bool mqtt_client_is_connected(MqttClient client);

/**
 * Obține starea curentă a conexiunii
 * 
 * @param client Handle-ul clientului
 * @return Starea conexiunii
 */
MqttConnectionState mqtt_client_get_state(MqttClient client);

/**
 * Eliberează resursele clientului
 * 
 * Deconectează (dacă necesar) și eliberează toată memoria alocată.
 * 
 * @param client Handle-ul clientului
 */
void mqtt_client_destroy(MqttClient client);

/* ============================================================================
 * FUNCȚII DE PUBLICARE
 * ============================================================================ */

/**
 * Publică un mesaj pe un topic
 * 
 * Trimite mesajul către broker pentru distribuție către subscriberi.
 * 
 * @param client Handle-ul clientului
 * @param topic Topic-ul de publicare
 * @param payload Datele mesajului
 * @param payload_length Lungimea payload-ului
 * @param qos Nivelul QoS dorit
 * @param retained Flag retained (broker păstrează ultima valoare)
 * @return MQTT_SUCCESS sau cod eroare
 * 
 * @note Pentru QoS > 0, funcția așteaptă confirmarea de la broker
 */
MqttError mqtt_publish(MqttClient client,
                       const char* topic,
                       const void* payload,
                       size_t payload_length,
                       MqttQoS qos,
                       bool retained);

/**
 * Publică un string pe un topic (convenință)
 * 
 * Echivalent cu mqtt_publish() cu payload_length = strlen(message)
 * 
 * @param client Handle-ul clientului
 * @param topic Topic-ul de publicare
 * @param message Stringul de publicat (null-terminated)
 * @param qos Nivelul QoS
 * @param retained Flag retained
 * @return MQTT_SUCCESS sau cod eroare
 */
MqttError mqtt_publish_string(MqttClient client,
                              const char* topic,
                              const char* message,
                              MqttQoS qos,
                              bool retained);

/* ============================================================================
 * FUNCȚII DE SUBSCRIBERE
 * ============================================================================ */

/**
 * Subscrie la un topic cu callback
 * 
 * Înregistrează interesul pentru mesaje pe topic-ul specificat.
 * Callback-ul va fi apelat pentru fiecare mesaj primit.
 * 
 * Wildcards suportate:
 * - '+' : Un singur nivel (ex: "sensors/+/temperature")
 * - '#' : Orice nivele (ex: "sensors/#")
 * 
 * @param client Handle-ul clientului
 * @param topic_filter Topic sau pattern cu wildcards
 * @param qos QoS maxim dorit (broker poate reduce)
 * @param callback Funcția apelată la recepție
 * @param user_data Date transmise callback-ului
 * @return MQTT_SUCCESS sau cod eroare
 */
MqttError mqtt_subscribe(MqttClient client,
                         const char* topic_filter,
                         MqttQoS qos,
                         MqttMessageCallback callback,
                         void* user_data);

/**
 * Anulează subscriberea la un topic
 * 
 * @param client Handle-ul clientului
 * @param topic_filter Topic-ul de la care se renunță
 * @return MQTT_SUCCESS sau cod eroare
 */
MqttError mqtt_unsubscribe(MqttClient client, const char* topic_filter);

/**
 * Setează callback pentru schimbări stare conexiune
 * 
 * @param client Handle-ul clientului
 * @param callback Funcția apelată la schimbare stare
 * @param user_data Date transmise callback-ului
 */
void mqtt_set_connection_callback(MqttClient client,
                                  MqttConnectionCallback callback,
                                  void* user_data);

/* ============================================================================
 * FUNCȚII DE PROCESARE EVENIMENTE
 * ============================================================================ */

/**
 * Procesează evenimente în mod non-blocant
 * 
 * Verifică mesaje primite și invocă callback-urile corespunzătoare.
 * Trebuie apelată periodic într-un loop.
 * 
 * @param client Handle-ul clientului
 * @param timeout_ms Timp maxim de așteptare (0 pentru non-blocant)
 * @return MQTT_SUCCESS, MQTT_ERR_TIMEOUT, sau cod eroare
 */
MqttError mqtt_process(MqttClient client, int timeout_ms);

/**
 * Rulează loop-ul de procesare continuu
 * 
 * Funcție blocantă care procesează evenimente până la deconectare
 * sau apelul mqtt_stop_loop().
 * 
 * @param client Handle-ul clientului
 * @return Codul erorii care a cauzat ieșirea
 */
MqttError mqtt_loop_forever(MqttClient client);

/**
 * Oprește loop-ul de procesare
 * 
 * Semnalizează mqtt_loop_forever() să se oprească grațios.
 * 
 * @param client Handle-ul clientului
 */
void mqtt_stop_loop(MqttClient client);

/* ============================================================================
 * FUNCȚII UTILITARE
 * ============================================================================ */

/**
 * Validează un topic MQTT
 * 
 * Verifică că topic-ul respectă regulile protocolului:
 * - Nu este gol
 * - Nu începe cu $
 * - Nu conține caractere invalide
 * 
 * @param topic Topic-ul de validat
 * @param allow_wildcards Permite wildcards (+, #)
 * @return true dacă valid, false altfel
 */
bool mqtt_validate_topic(const char* topic, bool allow_wildcards);

/**
 * Verifică dacă un topic se potrivește unui pattern
 * 
 * Evaluează wildcards pentru matching:
 * - '+' se potrivește cu exact un nivel
 * - '#' se potrivește cu zero sau mai multe nivele (doar la final)
 * 
 * @param topic Topic-ul de verificat
 * @param pattern Pattern-ul cu wildcards
 * @return true dacă se potrivește, false altfel
 */
bool mqtt_topic_matches(const char* topic, const char* pattern);

/**
 * Generează un client ID unic
 * 
 * Format: "client_<timestamp>_<random>"
 * 
 * @param buffer Buffer pentru rezultat
 * @param buffer_size Dimensiunea bufferului
 * @return Pointer către buffer sau NULL la eroare
 */
char* mqtt_generate_client_id(char* buffer, size_t buffer_size);

/**
 * Obține timestamp curent în milisecunde
 * 
 * @return Milisecunde de la epoch (1970-01-01)
 */
uint64_t mqtt_get_timestamp_ms(void);

/* ============================================================================
 * MACROS ȘI CONSTANTE CONDIȚIONALE
 * ============================================================================ */

#ifdef USE_PAHO_MQTT
    /* Compilare cu Paho MQTT C */
    #define MQTT_BACKEND_PAHO 1
    #define MQTT_BACKEND_POSIX 0
#else
    /* Compilare cu simulare POSIX */
    #define MQTT_BACKEND_PAHO 0
    #define MQTT_BACKEND_POSIX 1
#endif

/**
 * Macro pentru logging condiționat
 * 
 * Activat cu -DMQTT_DEBUG la compilare
 */
#ifdef MQTT_DEBUG
    #include <stdio.h>
    #define MQTT_LOG(fmt, ...) \
        fprintf(stderr, "[MQTT %s:%d] " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#else
    #define MQTT_LOG(fmt, ...) ((void)0)
#endif

#endif /* MQTT_COMMON_H */
