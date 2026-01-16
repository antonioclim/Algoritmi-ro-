# 📡 Simulatoare MQTT pentru IoT

## Arhitectură Dual-Mode

Acest director conține un sistem complet de simulare MQTT cu **două implementări interschimbabile**:

```
┌─────────────────────────────────────────────────────────────┐
│                    APLICAȚIE (publisher/subscriber)          │
├─────────────────────────────────────────────────────────────┤
│                      mqtt_common.h                          │
│                    (API Uniform)                            │
├────────────────────────┬────────────────────────────────────┤
│   mqtt_posix.c         │         mqtt_paho.c               │
│   (Simulare locală)    │    (Broker real Mosquitto)        │
│   Shared Memory + IPC  │       libpaho-mqtt3c              │
│   FĂRĂ dependențe      │    NECESITĂ instalare             │
├────────────────────────┴────────────────────────────────────┤
│     Local IPC                    TCP/IP Network             │
│  (aceeași mașină)             (local sau remote)            │
└─────────────────────────────────────────────────────────────┘
```

## Alegerea Implementării

### Opțiunea 1: Simulare POSIX (Fără Dependențe)

Ideală pentru:
- Testare rapidă fără infrastructură
- Medii fără acces root/sudo
- Dezvoltare offline
- Laboratoare cu restricții de instalare

```bash
# Compilare
gcc -o virtual_sensor virtual_sensor.c mqtt_posix.c -lpthread -lrt -lm
gcc -o mqtt_publisher mqtt_publisher.c mqtt_posix.c -lpthread -lrt
gcc -o mqtt_subscriber mqtt_subscriber.c mqtt_posix.c -lpthread -lrt

# Sau cu Makefile
make mqtt-sim
```

### Opțiunea 2: Paho MQTT C (Broker Real)

Ideală pentru:
- Testare cu broker real Mosquitto
- Comunicație între mașini diferite
- Producție/demo-uri realiste
- Integrare cu alte sisteme MQTT

```bash
# Instalare bibliotecă (Ubuntu/Debian)
sudo apt install libpaho-mqtt-dev

# Sau din sursă
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c && mkdir build && cd build
cmake .. && make && sudo make install
sudo ldconfig

# Compilare cu flag USE_PAHO_MQTT
gcc -DUSE_PAHO_MQTT -o virtual_sensor virtual_sensor.c mqtt_paho.c \
    -lpaho-mqtt3c -lpthread -lm
gcc -DUSE_PAHO_MQTT -o mqtt_publisher mqtt_publisher.c mqtt_paho.c \
    -lpaho-mqtt3c -lpthread
gcc -DUSE_PAHO_MQTT -o mqtt_subscriber mqtt_subscriber.c mqtt_paho.c \
    -lpaho-mqtt3c -lpthread

# Sau cu Makefile
make mqtt-real
```

## Fișiere

| Fișier | Descriere |
|--------|-----------|
| `mqtt_common.h` | Header cu API uniform pentru ambele implementări |
| `mqtt_posix.c` | Implementare folosind shared memory + semafoare |
| `mqtt_paho.c` | Implementare folosind biblioteca Paho MQTT C |
| `virtual_sensor.c` | Simulator senzori IoT (temperatură, umiditate, etc.) |
| `mqtt_publisher.c` | Exemplu publicare mesaje |
| `mqtt_subscriber.c` | Exemplu subscribere cu wildcards |

## API Comun

Toate funcțiile au aceeași semnătură indiferent de backend:

```c
// Creare și conectare
MqttError mqtt_client_create(MqttClient* client, const char* client_id);
MqttError mqtt_client_connect(MqttClient client, const MqttConnectOptions* opts);
MqttError mqtt_client_disconnect(MqttClient client);
void mqtt_client_destroy(MqttClient client);

// Publicare
MqttError mqtt_publish(MqttClient client, const char* topic,
                       const void* payload, size_t len,
                       MqttQoS qos, bool retained);

// Subscribere
MqttError mqtt_subscribe(MqttClient client, const char* topic_filter,
                         MqttQoS qos, MqttMessageCallback callback,
                         void* user_data);

// Procesare evenimente
MqttError mqtt_process(MqttClient client, int timeout_ms);
MqttError mqtt_loop_forever(MqttClient client);
```

## Exemple de Utilizare

### Demo Complet (POSIX - fără broker)

```bash
# Terminal 1: Subscriber
./mqtt_subscriber "sensors/#" -v -t

# Terminal 2: Simulator senzori
./virtual_sensor -v -i 500

# Terminal 3: Publisher manual
./mqtt_publisher sensors/custom "Manual message test"
```

### Demo cu Mosquitto (Paho)

```bash
# 1. Pornește broker-ul
docker-compose up -d mosquitto
# sau: sudo apt install mosquitto && sudo systemctl start mosquitto

# 2. Subscriber într-un terminal
./mqtt_subscriber_paho "sensors/#" -v

# 3. Simulator în alt terminal
./virtual_sensor_paho -b localhost -p 1883 -v
```

## Diferențe între Implementări

| Caracteristică | POSIX | Paho |
|---------------|-------|------|
| Dependențe | Niciuna | libpaho-mqtt3c |
| Comunicație | Local (IPC) | Rețea TCP/IP |
| QoS real | Simulat | Complet suportat |
| TLS/SSL | Nu | Da |
| Multi-mașină | Nu | Da |
| Performanță | Foarte rapidă | Dependent de rețea |
| Max clienți | 16 | Nelimitat (broker) |

## Cleanup Resurse POSIX

Simularea POSIX folosește shared memory și semafoare care persistă. Pentru cleanup:

```bash
# Vizualizare resurse
ls -la /dev/shm/mqtt_*
ls -la /dev/sem/mqtt_*

# Cleanup manual
rm /dev/shm/mqtt_sim_broker
rm /dev/sem/mqtt_sim_*

# Sau din cod (apelat când nu mai sunt clienți)
mqtt_posix_cleanup_broker();
```

## Debugging

Compilare cu logging activat:

```bash
# POSIX cu debug
gcc -DMQTT_DEBUG -o virtual_sensor virtual_sensor.c mqtt_posix.c \
    -lpthread -lrt -lm

# Paho cu debug
gcc -DUSE_PAHO_MQTT -DMQTT_DEBUG -o virtual_sensor virtual_sensor.c \
    mqtt_paho.c -lpaho-mqtt3c -lpthread -lm
```

## Structuri de Date Principale

```c
// Niveluri QoS
typedef enum {
    MQTT_QOS_AT_MOST_ONCE  = 0,  // Fire and forget
    MQTT_QOS_AT_LEAST_ONCE = 1,  // Garantează livrare
    MQTT_QOS_EXACTLY_ONCE  = 2   // Livrare unică
} MqttQoS;

// Mesaj recepționat
typedef struct {
    char topic[256];
    uint8_t* payload;
    size_t payload_length;
    MqttQoS qos;
    bool retained;
    uint64_t timestamp_ms;
} MqttMessage;

// Opțiuni conectare
typedef struct {
    const char* broker_address;
    int broker_port;
    const char* client_id;
    const char* username;
    const char* password;
    int keepalive_seconds;
    bool clean_session;
    // Last Will and Testament
    const char* lwt_topic;
    const char* lwt_message;
} MqttConnectOptions;
```

## Wildcards MQTT

Topic patterns suportate:

```
sensors/#              → toate sub sensors/
sensors/+/temperature  → orice senzor, topic temperature
home/+/+/status        → ex: home/floor1/room1/status
```

- `+` potrivește exact **un** nivel
- `#` potrivește **zero sau mai multe** nivele (doar la final)

## Referințe

- [MQTT Specification 3.1.1](http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html)
- [Eclipse Paho MQTT C](https://www.eclipse.org/paho/index.php?page=clients/c/index.php)
- [Mosquitto Broker](https://mosquitto.org/)
