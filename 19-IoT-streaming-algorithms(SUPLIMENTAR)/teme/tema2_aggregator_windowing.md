# 📝 Tema 2: Aggregator de Date IoT cu Windowing

## Săptămâna 19: Algoritmi pentru IoT și Stream Processing

---

## 📋 Descriere

O fermă inteligentă (smart farm) monitorizează condițiile de mediu în **sere** (greenhouses).
Fiecare seră are senzori pentru: temperatură, umiditate, lumină, CO₂.

Trebuie să implementați un **aggregator** care:
1. Primește date de la multiple senzori
2. Agregă datele în ferestre de timp (tumbling windows)
3. Calculează statistici per fereastră
4. Detectează condiții anormale
5. Generează comenzi pentru actuatoare

---

## 🎯 Cerințe Funcționale

### Partea A: Tumbling Window (30 puncte)

Implementați o fereastră tumbling care:
- Acumulează date pe o perioadă fixă (ex: 5 minute)
- La închiderea ferestrei, emite statistici agregate
- Resetează pentru următoarea fereastră

```c
typedef struct {
    // Configurare
    uint64_t window_size_ms;
    
    // Stare fereastră curentă
    uint64_t window_start;
    
    // Statistici running (Welford)
    double sum;
    double min;
    double max;
    double mean;
    double M2;
    int count;
} TumblingWindow;

typedef struct {
    uint64_t window_start;
    uint64_t window_end;
    double avg;
    double min;
    double max;
    double stddev;
    int sample_count;
} WindowStats;

// Funcții
void window_init(TumblingWindow* w, uint64_t size_ms);
bool window_add(TumblingWindow* w, double value, uint64_t timestamp, WindowStats* out);
void window_reset(TumblingWindow* w, uint64_t new_start);
```

### Partea B: Multi-Sensor Aggregator (30 puncte)

Gestionați multiple senzori și tipuri de măsurători:

```c
typedef enum {
    SENSOR_TEMPERATURE,
    SENSOR_HUMIDITY,
    SENSOR_LIGHT,
    SENSOR_CO2
} SensorType;

typedef struct {
    char sensor_id[32];
    SensorType type;
    TumblingWindow window;
    EMAFilter filter;
    
    // Praguri pentru alertare
    double threshold_low;
    double threshold_high;
} SensorAggregator;

typedef struct {
    char greenhouse_id[32];
    SensorAggregator sensors[4];  // Unul pentru fiecare tip
    
    // Statistici globale
    int windows_completed;
    int alerts_generated;
} GreenhouseMonitor;
```

### Partea C: Logica de Control (25 puncte)

Implementați reguli de control pentru actuatoare:

| Condiție | Acțiune |
|----------|---------|
| Temp > 30°C | Activează ventilație |
| Temp < 15°C | Activează încălzire |
| Humidity > 80% | Activează dehumidifier |
| Humidity < 40% | Activează umidificator |
| Light < 500 lux (ziua) | Activează lumini artificiale |
| CO₂ > 1000 ppm | Activează ventilație |

```c
typedef enum {
    CMD_NONE = 0,
    CMD_HEATING_ON,
    CMD_HEATING_OFF,
    CMD_COOLING_ON,
    CMD_COOLING_OFF,
    CMD_HUMIDIFIER_ON,
    CMD_HUMIDIFIER_OFF,
    CMD_DEHUMIDIFIER_ON,
    CMD_DEHUMIDIFIER_OFF,
    CMD_LIGHTS_ON,
    CMD_LIGHTS_OFF,
    CMD_VENTILATION_ON,
    CMD_VENTILATION_OFF
} ActuatorCommand;

typedef struct {
    ActuatorCommand command;
    char greenhouse_id[32];
    uint64_t timestamp;
    char reason[128];
} ControlAction;

ControlAction* evaluate_conditions(GreenhouseMonitor* monitor, WindowStats stats[4], int* n_actions);
```

### Partea D: Raport și Logging (15 puncte)

Generați output în format structurat:

```
[2024-01-01 10:00:00] WINDOW_CLOSE greenhouse_001 temperature avg=24.5 min=23.1 max=25.8 σ=0.6
[2024-01-01 10:00:00] WINDOW_CLOSE greenhouse_001 humidity avg=65.2 min=60.0 max=72.1 σ=3.2
[2024-01-01 10:00:00] CONTROL greenhouse_001 CMD_DEHUMIDIFIER_ON reason="humidity 72.1% > 70%"
[2024-01-01 10:05:00] WINDOW_CLOSE greenhouse_001 temperature avg=24.3 min=23.0 max=25.5 σ=0.5
...
```

---

## 📁 Format Input

Fișierul `greenhouse_data.csv`:

```csv
timestamp,greenhouse_id,sensor_type,value
1704067200000,greenhouse_001,temperature,24.5
1704067200000,greenhouse_001,humidity,65.0
1704067200000,greenhouse_001,light,850
1704067200000,greenhouse_001,co2,450
1704067201000,greenhouse_001,temperature,24.6
...
```

---

## 🔧 Parametri

| Parametru | Valoare |
|-----------|---------|
| Window size | 5 minute (300000 ms) |
| EMA α | 0.15 |
| Temp range | 15°C - 30°C |
| Humidity range | 40% - 80% |
| Light minimum (ziua) | 500 lux |
| CO₂ maximum | 1000 ppm |

---

## 📊 Exemplu Output Raport Final

```
=== RAPORT AGREGARE SERĂ INTELIGENTĂ ===
Perioada: 2024-01-01 06:00 - 2024-01-01 18:00
Ferestre procesate: 144 (12 ore × 12 ferestre/oră)

SERĂ: greenhouse_001
  Temperature:
    - Media generală: 23.8°C
    - Range: [18.5°C, 28.2°C]
    - Ferestre în afara limitelor: 3
  
  Humidity:
    - Media generală: 62.4%
    - Range: [45.1%, 78.9%]
    - Ferestre în afara limitelor: 5
  
  Comenzi emise:
    - HEATING_ON: 2
    - COOLING_ON: 1
    - DEHUMIDIFIER_ON: 4
    - HUMIDIFIER_ON: 1
    - VENTILATION_ON: 2

STATISTICI GLOBALE:
  Total ferestre: 576 (4 sere × 144)
  Total comenzi: 47
  Eficiență energetică estimată: 89%
```

---

## ✅ Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Tumbling Window corect | 25 |
| Multi-sensor aggregation | 20 |
| Statistici Welford | 15 |
| Logica de control | 20 |
| Raport și logging | 10 |
| Stil cod | 10 |
| **TOTAL** | **100** |

---

## 🚀 Compilare și Testare

```bash
# Compilare
gcc -Wall -Wextra -std=c11 -o tema2 tema2.c -lm

# Rulare
./tema2 greenhouse_data.csv

# Generare date test (script Python inclus)
python3 generate_greenhouse_data.py > greenhouse_data.csv
```

---

## 💡 Hints

1. Folosiți `uint64_t` pentru timestamps (milisecunde Unix)
2. La `window_add()`, verificați dacă timestamp-ul e în fereastra curentă
3. Gândiți-vă la cazuri limită: ce se întâmplă dacă un senzor nu trimite date?
4. Hysteresis pentru comenzi: nu porniți/opriți continuu un actuator

---

## 📅 Termen Limită

**Data**: Vezi platforma de cursuri  
**Predare**: Arhivă ZIP cu `tema2.c`, `README.txt`, `output_sample.txt`

---

*Algoritmi și Tehnici de Programare - ASE București, CSIE*
