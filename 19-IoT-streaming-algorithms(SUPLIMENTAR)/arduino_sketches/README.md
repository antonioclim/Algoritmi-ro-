# 🔌 Arduino/ESP32 Sketches pentru IoT

## Săptămâna 19: Algoritmi pentru IoT și Stream Processing

Acest director conține exemple de cod pentru microcontrolere Arduino și ESP32,
demonstrând implementarea algoritmilor de stream processing pe dispozitive embedded.

---

## ⚠️ Notă Importantă

**Acest material este OPȚIONAL** - nu necesită hardware fizic pentru completarea
laboratorului. Studenții interesați pot împrumuta kit-uri ESP32 de la biblioteca
facultății (disponibilitate limitată).

Alternativ, se poate folosi simulatorul [Wokwi](https://wokwi.com/) pentru
testare fără hardware.

---

## 📁 Conținut

| Fișier | Descriere |
|--------|-----------|
| `sensor_publisher.ino` | Citire senzori DHT22 + publicare MQTT |
| `stream_filter.ino` | Implementare EMA pe ESP32 |
| `anomaly_alert.ino` | Detecție anomalii cu LED indicator |

---

## 🛠️ Hardware Necesar

### Opțiunea 1: ESP32 DevKit (Recomandat)
- **ESP32-WROOM-32** sau **ESP32-S3** development board
- Senzor **DHT22** (temperatură + umiditate)
- LED + rezistor 220Ω
- Breadboard + fire jumper
- Cablu micro-USB

### Opțiunea 2: Arduino Uno + Shield WiFi
- **Arduino Uno R3**
- **ESP8266 WiFi Shield** sau **Ethernet Shield**
- Senzor **DHT22**
- LED + rezistor

### Pinout ESP32 + DHT22
```
ESP32           DHT22
------          -----
3.3V    ───────  VCC
GND     ───────  GND
GPIO 4  ───────  DATA (cu rezistor 10K pull-up)
```

---

## 📦 Biblioteci Necesare

Instalare din Arduino IDE → Sketch → Include Library → Manage Libraries:

1. **PubSubClient** by Nick O'Leary (MQTT)
2. **DHT sensor library** by Adafruit
3. **ArduinoJson** by Benoit Blanchon
4. **WiFi** (inclusă în ESP32 core)

### Instalare ESP32 Board Support
1. File → Preferences → Additional Board Manager URLs:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
2. Tools → Board → Board Manager → Caută "ESP32" → Install

---

## 🚀 Quick Start

### 1. Configurare WiFi și MQTT
Editează `config.h`:
```cpp
// WiFi
#define WIFI_SSID     "NumeleRetelei"
#define WIFI_PASSWORD "ParolaWiFi"

// MQTT Broker (Mosquitto local sau HiveMQ cloud)
#define MQTT_BROKER   "192.168.1.100"  // sau "broker.hivemq.com"
#define MQTT_PORT     1883
#define MQTT_TOPIC    "sensors/esp32/temperature"
```

### 2. Upload și Monitor
1. Selectează Board: Tools → Board → ESP32 Dev Module
2. Selectează Port: Tools → Port → COMx (sau /dev/ttyUSB0)
3. Upload: Sketch → Upload
4. Monitor: Tools → Serial Monitor (115200 baud)

### 3. Verificare
Rulează subscriber pe PC:
```bash
mosquitto_sub -h localhost -t "sensors/#" -v
```

---

## 📊 Structura Mesajelor

### Format JSON publicat:
```json
{
  "device_id": "esp32_001",
  "sensor": "dht22",
  "temperature": 23.5,
  "humidity": 45.2,
  "filtered_temp": 23.4,
  "timestamp": 1704067200,
  "anomaly": false
}
```

### Topic-uri:
```
sensors/esp32_001/raw          # Date brute
sensors/esp32_001/filtered     # Date filtrate EMA
sensors/esp32_001/alerts       # Alerte anomalii
sensors/esp32_001/status       # Heartbeat/status
```

---

## 🔧 Algoritmi Implementați

### 1. Exponential Moving Average (EMA)
```cpp
class EMAFilter {
private:
    float alpha;
    float value;
    bool initialized;
    
public:
    EMAFilter(float smoothing = 0.2) : alpha(smoothing), 
                                        value(0), 
                                        initialized(false) {}
    
    float update(float measurement) {
        if (!initialized) {
            value = measurement;
            initialized = true;
        } else {
            value = alpha * measurement + (1 - alpha) * value;
        }
        return value;
    }
};

// Utilizare:
EMAFilter tempFilter(0.2);  // α = 0.2
float filtered = tempFilter.update(rawTemp);
```

### 2. Z-Score Anomaly Detection
```cpp
class AnomalyDetector {
private:
    float mean;
    float M2;
    unsigned long count;
    float threshold;
    
public:
    AnomalyDetector(float thresh = 2.5) : mean(0), M2(0), 
                                           count(0), 
                                           threshold(thresh) {}
    
    bool isAnomaly(float x) {
        count++;
        float delta = x - mean;
        mean += delta / count;
        float delta2 = x - mean;
        M2 += delta * delta2;
        
        if (count < 10) return false;  // Warm-up
        
        float variance = M2 / count;
        float stddev = sqrt(variance);
        
        if (stddev < 0.001) return false;
        
        float zscore = abs(delta) / stddev;
        return zscore > threshold;
    }
};
```

### 3. Rate Limiter pentru Alerte
```cpp
class RateLimiter {
private:
    unsigned long lastAlert;
    unsigned long cooldownMs;
    
public:
    RateLimiter(unsigned long cooldown = 30000) : 
        lastAlert(0), cooldownMs(cooldown) {}
    
    bool canAlert() {
        unsigned long now = millis();
        if (now - lastAlert >= cooldownMs) {
            lastAlert = now;
            return true;
        }
        return false;
    }
};
```

---

## 🔋 Considerații Power Management

Pentru aplicații pe baterie:

```cpp
// Deep sleep între citiri
#define SLEEP_DURATION_US  60000000  // 60 secunde

void goToSleep() {
    esp_sleep_enable_timer_wakeup(SLEEP_DURATION_US);
    esp_deep_sleep_start();
}

// Duty cycling WiFi
WiFi.setSleep(true);  // Modul light sleep
```

---

## 🐛 Troubleshooting

### ESP32 nu se conectează la WiFi
- Verifică SSID și parola (case-sensitive)
- Asigură-te că routerul suportă 2.4GHz (ESP32 nu suportă 5GHz)
- Încearcă să resetezi ESP32 (EN button)

### MQTT connection refused
- Verifică dacă broker-ul rulează: `sudo systemctl status mosquitto`
- Verifică firewall: `sudo ufw allow 1883`
- Testează cu mosquitto_pub local

### Citiri senzor invalide (NaN)
- Verifică conexiunile fizice
- Adaugă delay între citiri: `delay(2000);`
- Verifică rezistorul pull-up pe linia DATA

### Upload failed
- Ține apăsat BOOT button în timpul upload-ului
- Verifică driverul CP2102/CH340

---

## 📚 Resurse Adiționale

- [ESP32 Arduino Core Documentation](https://docs.espressif.com/projects/arduino-esp32/)
- [PubSubClient MQTT Library](https://pubsubclient.knolleary.net/)
- [Wokwi ESP32 Simulator](https://wokwi.com/projects/new/esp32)
- [HiveMQ Free MQTT Broker](https://www.hivemq.com/mqtt/public-mqtt-broker/)

---

## 📄 Licență

Cod educațional pentru cursul ATP, ASE București - CSIE.
Utilizare liberă în scopuri didactice.

---

*Generat pentru Săptămâna 19 - IoT și Stream Processing*
