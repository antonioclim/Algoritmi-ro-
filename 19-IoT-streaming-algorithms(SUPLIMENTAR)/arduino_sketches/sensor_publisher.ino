/**
 * ============================================================================
 * ESP32 SENSOR PUBLISHER - IoT Stream Processing Demo
 * ============================================================================
 * 
 * Săptămâna 19: Algoritmi pentru IoT și Stream Processing
 * Curs: Algoritmi și Tehnici de Programare, ASE București
 * 
 * DESCRIERE:
 *   Citește date de la senzorul DHT22 (temperatură + umiditate),
 *   aplică filtru EMA pentru netezire, detectează anomalii cu Z-score,
 *   și publică rezultatele prin MQTT.
 * 
 * HARDWARE:
 *   - ESP32 DevKit sau Arduino cu WiFi Shield
 *   - Senzor DHT22 pe GPIO 4
 *   - LED indicator pe GPIO 2 (built-in pe majoritatea board-urilor)
 * 
 * BIBLIOTECI:
 *   - WiFi.h (ESP32 core)
 *   - PubSubClient (MQTT)
 *   - DHT sensor library (Adafruit)
 *   - ArduinoJson
 * 
 * AUTOR: Generated for ASE-CSIE ATP Course
 * VERSIUNE: 1.0 (Ianuarie 2026)
 * ============================================================================
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <math.h>

/* ============================================================================
 * CONFIGURAȚIE - EDITAȚI AICI
 * ============================================================================ */

// WiFi
#define WIFI_SSID     "WIFI_NETWORK_NAME"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// MQTT Broker
#define MQTT_BROKER   "192.168.1.100"  // sau "broker.hivemq.com" pentru test
#define MQTT_PORT     1883
#define MQTT_USER     ""               // Gol dacă nu e nevoie
#define MQTT_PASS     ""

// Device
#define DEVICE_ID     "esp32_001"
#define TOPIC_BASE    "sensors/" DEVICE_ID

// Hardware
#define DHT_PIN       4
#define DHT_TYPE      DHT22
#define LED_PIN       2                // LED built-in ESP32

// Timing
#define READ_INTERVAL_MS    2000       // Citire la 2 secunde
#define PUBLISH_INTERVAL_MS 5000       // Publicare la 5 secunde
#define RECONNECT_DELAY_MS  5000

// Algoritmi
#define EMA_ALPHA           0.2        // Factor netezire EMA
#define ZSCORE_THRESHOLD    2.5        // Prag detecție anomalii
#define WARMUP_SAMPLES      10         // Samples înainte de detecție
#define ALERT_COOLDOWN_MS   30000      // 30 sec între alerte

/* ============================================================================
 * CLASE ALGORITMI
 * ============================================================================ */

/**
 * Filtru Exponential Moving Average
 * Complexitate: O(1) timp, O(1) spațiu
 */
class EMAFilter {
private:
    float alpha;
    float value;
    bool initialized;
    
public:
    EMAFilter(float smoothing = 0.2) : 
        alpha(smoothing), value(0), initialized(false) {}
    
    float update(float measurement) {
        if (!initialized) {
            value = measurement;
            initialized = true;
        } else {
            // EMA(t) = α × x(t) + (1-α) × EMA(t-1)
            value = alpha * measurement + (1.0f - alpha) * value;
        }
        return value;
    }
    
    float getValue() const { return value; }
    
    void reset() {
        value = 0;
        initialized = false;
    }
};

/**
 * Detector de anomalii bazat pe Z-score
 * Folosește algoritmul Welford pentru calcul incremental
 * Complexitate: O(1) timp, O(1) spațiu
 */
class ZScoreDetector {
private:
    float mean;
    float M2;           // Suma pătratelor diferențelor
    unsigned long count;
    float threshold;
    
public:
    ZScoreDetector(float thresh = 2.5) : 
        mean(0), M2(0), count(0), threshold(thresh) {}
    
    /**
     * Verifică dacă valoarea este anomalie și actualizează statisticile
     * @return true dacă valoarea este anomalie
     */
    bool checkAndUpdate(float x) {
        count++;
        
        // Algoritmul Welford - actualizare incrementală
        float delta = x - mean;
        mean += delta / count;
        float delta2 = x - mean;
        M2 += delta * delta2;
        
        // Nu detectăm anomalii în perioada de warmup
        if (count < WARMUP_SAMPLES) {
            return false;
        }
        
        // Calculăm deviația standard
        float variance = M2 / count;
        float stddev = sqrt(variance);
        
        // Evităm împărțirea la zero
        if (stddev < 0.001f) {
            return false;
        }
        
        // Z-score = |x - μ| / σ
        float zscore = fabs(delta) / stddev;
        
        return zscore > threshold;
    }
    
    float getMean() const { return mean; }
    float getStdDev() const { 
        return (count > 1) ? sqrt(M2 / count) : 0; 
    }
    unsigned long getCount() const { return count; }
    
    void reset() {
        mean = 0;
        M2 = 0;
        count = 0;
    }
};

/**
 * Rate Limiter pentru prevenirea alert fatigue
 */
class RateLimiter {
private:
    unsigned long lastTrigger;
    unsigned long cooldownMs;
    unsigned long suppressedCount;
    
public:
    RateLimiter(unsigned long cooldown = 30000) : 
        lastTrigger(0), cooldownMs(cooldown), suppressedCount(0) {}
    
    bool allow() {
        unsigned long now = millis();
        
        // Gestionăm overflow-ul millis()
        if (now < lastTrigger) {
            lastTrigger = 0;  // Reset la overflow
        }
        
        if (now - lastTrigger >= cooldownMs) {
            lastTrigger = now;
            return true;
        }
        
        suppressedCount++;
        return false;
    }
    
    unsigned long getSuppressedCount() const { return suppressedCount; }
    
    void reset() {
        lastTrigger = 0;
        suppressedCount = 0;
    }
};

/* ============================================================================
 * VARIABILE GLOBALE
 * ============================================================================ */

// Conexiuni
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
DHT dht(DHT_PIN, DHT_TYPE);

// Filtre și detectoare
EMAFilter tempFilter(EMA_ALPHA);
EMAFilter humidFilter(EMA_ALPHA);
ZScoreDetector tempDetector(ZSCORE_THRESHOLD);
ZScoreDetector humidDetector(ZSCORE_THRESHOLD);
RateLimiter alertLimiter(ALERT_COOLDOWN_MS);

// Timere
unsigned long lastReadTime = 0;
unsigned long lastPublishTime = 0;
unsigned long messageCount = 0;

// Ultima citire
float lastTemp = 0;
float lastHumid = 0;
float filteredTemp = 0;
float filteredHumid = 0;
bool tempAnomaly = false;
bool humidAnomaly = false;

/* ============================================================================
 * FUNCȚII AUXILIARE
 * ============================================================================ */

/**
 * Conectare la WiFi
 */
void connectWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.print("Connected! IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println();
        Serial.println("WiFi connection failed!");
    }
}

/**
 * Conectare la broker MQTT
 */
void connectMQTT() {
    if (mqttClient.connected()) return;
    
    Serial.print("Connecting to MQTT broker...");
    
    // Client ID unic
    String clientId = "esp32_" + String(random(0xffff), HEX);
    
    // Last Will and Testament
    String lwtTopic = String(TOPIC_BASE) + "/status";
    String lwtMessage = "{\"status\":\"offline\",\"device\":\"" DEVICE_ID "\"}";
    
    bool connected;
    if (strlen(MQTT_USER) > 0) {
        connected = mqttClient.connect(
            clientId.c_str(),
            MQTT_USER, MQTT_PASS,
            lwtTopic.c_str(), 0, true, lwtMessage.c_str()
        );
    } else {
        connected = mqttClient.connect(
            clientId.c_str(),
            lwtTopic.c_str(), 0, true, lwtMessage.c_str()
        );
    }
    
    if (connected) {
        Serial.println(" Connected!");
        
        // Publicare status online
        String onlineMsg = "{\"status\":\"online\",\"device\":\"" DEVICE_ID "\"}";
        mqttClient.publish(lwtTopic.c_str(), onlineMsg.c_str(), true);
        
        // Subscribe la comenzi (opțional)
        String cmdTopic = String(TOPIC_BASE) + "/cmd";
        mqttClient.subscribe(cmdTopic.c_str());
    } else {
        Serial.print(" Failed (rc=");
        Serial.print(mqttClient.state());
        Serial.println(")");
    }
}

/**
 * Callback pentru mesaje MQTT primite
 */
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message received on ");
    Serial.print(topic);
    Serial.print(": ");
    
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);
    
    // Procesare comenzi
    if (message == "reset") {
        tempFilter.reset();
        humidFilter.reset();
        tempDetector.reset();
        humidDetector.reset();
        alertLimiter.reset();
        Serial.println("Filters reset!");
    } else if (message == "status") {
        publishStatus();
    }
}

/**
 * Citire senzori
 */
void readSensors() {
    float temp = dht.readTemperature();
    float humid = dht.readHumidity();
    
    // Verificare citiri valide
    if (isnan(temp) || isnan(humid)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }
    
    lastTemp = temp;
    lastHumid = humid;
    
    // Aplicare filtre EMA
    filteredTemp = tempFilter.update(temp);
    filteredHumid = humidFilter.update(humid);
    
    // Detectare anomalii
    tempAnomaly = tempDetector.checkAndUpdate(temp);
    humidAnomaly = humidDetector.checkAndUpdate(humid);
    
    // Indicare LED la anomalie
    if (tempAnomaly || humidAnomaly) {
        digitalWrite(LED_PIN, HIGH);
    } else {
        digitalWrite(LED_PIN, LOW);
    }
    
    // Log la serial
    Serial.printf("Temp: %.1f°C (filtered: %.1f, %s) | Humid: %.1f%% (filtered: %.1f, %s)\n",
                  temp, filteredTemp, tempAnomaly ? "ANOMALY!" : "OK",
                  humid, filteredHumid, humidAnomaly ? "ANOMALY!" : "OK");
}

/**
 * Publicare date prin MQTT
 */
void publishData() {
    if (!mqttClient.connected()) return;
    
    // Creăm JSON
    StaticJsonDocument<512> doc;
    
    doc["device_id"] = DEVICE_ID;
    doc["timestamp"] = millis() / 1000;
    doc["message_id"] = messageCount++;
    
    // Date brute
    JsonObject raw = doc.createNestedObject("raw");
    raw["temperature"] = round(lastTemp * 10) / 10.0;
    raw["humidity"] = round(lastHumid * 10) / 10.0;
    
    // Date filtrate
    JsonObject filtered = doc.createNestedObject("filtered");
    filtered["temperature"] = round(filteredTemp * 10) / 10.0;
    filtered["humidity"] = round(filteredHumid * 10) / 10.0;
    
    // Statistici detector
    JsonObject stats = doc.createNestedObject("stats");
    stats["temp_mean"] = round(tempDetector.getMean() * 10) / 10.0;
    stats["temp_stddev"] = round(tempDetector.getStdDev() * 100) / 100.0;
    stats["samples"] = tempDetector.getCount();
    
    // Stare anomalii
    JsonObject anomalies = doc.createNestedObject("anomalies");
    anomalies["temperature"] = tempAnomaly;
    anomalies["humidity"] = humidAnomaly;
    
    // Serializare și publicare
    char buffer[512];
    serializeJson(doc, buffer);
    
    String topic = String(TOPIC_BASE) + "/data";
    mqttClient.publish(topic.c_str(), buffer);
    
    Serial.println("Published data to MQTT");
}

/**
 * Publicare alertă anomalie
 */
void publishAlert(const char* sensorType, float value, float mean, float stddev) {
    if (!mqttClient.connected()) return;
    
    // Verificare rate limit
    if (!alertLimiter.allow()) {
        Serial.println("Alert suppressed by rate limiter");
        return;
    }
    
    StaticJsonDocument<256> doc;
    
    doc["device_id"] = DEVICE_ID;
    doc["alert_type"] = "anomaly";
    doc["sensor"] = sensorType;
    doc["value"] = round(value * 10) / 10.0;
    doc["expected_mean"] = round(mean * 10) / 10.0;
    doc["stddev"] = round(stddev * 100) / 100.0;
    doc["zscore"] = round(fabs(value - mean) / stddev * 100) / 100.0;
    doc["timestamp"] = millis() / 1000;
    doc["suppressed_count"] = alertLimiter.getSuppressedCount();
    
    char buffer[256];
    serializeJson(doc, buffer);
    
    String topic = String(TOPIC_BASE) + "/alerts";
    mqttClient.publish(topic.c_str(), buffer);
    
    Serial.println("⚠️ ALERT published!");
}

/**
 * Publicare status dispozitiv
 */
void publishStatus() {
    if (!mqttClient.connected()) return;
    
    StaticJsonDocument<256> doc;
    
    doc["device_id"] = DEVICE_ID;
    doc["status"] = "online";
    doc["uptime_sec"] = millis() / 1000;
    doc["wifi_rssi"] = WiFi.RSSI();
    doc["free_heap"] = ESP.getFreeHeap();
    doc["messages_sent"] = messageCount;
    doc["suppressed_alerts"] = alertLimiter.getSuppressedCount();
    
    char buffer[256];
    serializeJson(doc, buffer);
    
    String topic = String(TOPIC_BASE) + "/status";
    mqttClient.publish(topic.c_str(), buffer, true);
}

/* ============================================================================
 * SETUP ȘI LOOP
 * ============================================================================ */

void setup() {
    // Inițializare serial
    Serial.begin(115200);
    delay(1000);
    
    Serial.println();
    Serial.println("========================================");
    Serial.println("  ESP32 IoT Sensor Publisher");
    Serial.println("  ATP Week 19 - Stream Processing");
    Serial.println("========================================");
    Serial.println();
    
    // Inițializare LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    // Inițializare DHT
    dht.begin();
    Serial.println("DHT sensor initialized");
    
    // Conectare WiFi
    connectWiFi();
    
    // Configurare MQTT
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
    mqttClient.setBufferSize(512);
    
    // Conectare MQTT
    connectMQTT();
    
    Serial.println();
    Serial.println("Setup complete! Starting measurements...");
    Serial.println();
}

void loop() {
    unsigned long currentTime = millis();
    
    // Menține conexiunile
    if (WiFi.status() != WL_CONNECTED) {
        connectWiFi();
    }
    
    if (!mqttClient.connected()) {
        connectMQTT();
    }
    
    mqttClient.loop();
    
    // Citire senzori la interval
    if (currentTime - lastReadTime >= READ_INTERVAL_MS) {
        lastReadTime = currentTime;
        readSensors();
        
        // Publicare alerte imediat la anomalii
        if (tempAnomaly) {
            publishAlert("temperature", lastTemp, 
                        tempDetector.getMean(), 
                        tempDetector.getStdDev());
        }
        if (humidAnomaly) {
            publishAlert("humidity", lastHumid,
                        humidDetector.getMean(),
                        humidDetector.getStdDev());
        }
    }
    
    // Publicare date periodică
    if (currentTime - lastPublishTime >= PUBLISH_INTERVAL_MS) {
        lastPublishTime = currentTime;
        publishData();
        
        // Publicare status la fiecare 12 mesaje (~1 minut)
        if (messageCount % 12 == 0) {
            publishStatus();
        }
    }
}
