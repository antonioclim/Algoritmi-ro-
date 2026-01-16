# 📋 Tema 1: Sistem IoT pentru Agricultură Inteligentă

## Săptămâna 19 - Algoritmi pentru IoT și Stream Processing

**Termen limită:** 2 săptămâni de la predare  
**Punctaj maxim:** 100 puncte  
**Mod de predare:** Arhivă ZIP pe platforma e-learning

---

## 🎯 Obiectiv

Implementați un sistem complet de monitorizare pentru o seră inteligentă care:
1. Colectează date de la senzori multipli
2. Aplică filtrare pentru reducerea zgomotului
3. Detectează condiții anormale
4. Generează alerte cu rate limiting
5. Produce rapoarte agregate

---

## 📝 Cerințe Funcționale

### Partea A: Structuri de Date (20 puncte)

Definiți următoarele structuri:

```c
// Tipuri de senzori disponibili
typedef enum {
    SENSOR_SOIL_MOISTURE,    // Umiditate sol (%)
    SENSOR_SOIL_TEMPERATURE, // Temperatură sol (°C)
    SENSOR_AIR_TEMPERATURE,  // Temperatură aer (°C)
    SENSOR_AIR_HUMIDITY,     // Umiditate aer (%)
    SENSOR_LIGHT_INTENSITY,  // Intensitate luminoasă (lux)
    SENSOR_CO2_LEVEL,        // Nivel CO2 (ppm)
    SENSOR_TYPE_COUNT
} SensorType;

// Citire de la senzor
typedef struct {
    char sensor_id[32];
    SensorType type;
    double value;
    uint64_t timestamp_ms;
    uint8_t quality;  // 0-100, calitatea citirii
} SensorReading;

// Context per senzor (de completat)
typedef struct {
    // ... filtre, detectoare, etc.
} SensorContext;

// Configurație sistem
typedef struct {
    // ... praguri, intervale, etc.
} SystemConfig;
```

### Partea B: Filtrare Date (25 puncte)

Implementați **două** filtre din următoarele trei:

1. **Moving Average cu buffer circular** (O(1) update)
   ```c
   typedef struct MovingAverage MovingAverage;
   MovingAverage* ma_create(int window_size);
   double ma_update(MovingAverage* ma, double value);
   void ma_destroy(MovingAverage* ma);
   ```

2. **Exponential Moving Average**
   ```c
   typedef struct EMAFilter EMAFilter;
   void ema_init(EMAFilter* f, double alpha);
   double ema_update(EMAFilter* f, double value);
   ```

3. **Filtru Kalman 1D simplificat**
   ```c
   typedef struct Kalman1D Kalman1D;
   void kalman_init(Kalman1D* k, double process_noise, double measurement_noise);
   double kalman_update(Kalman1D* k, double measurement);
   ```

### Partea C: Detecție Anomalii (25 puncte)

Implementați detector de anomalii cu **Z-score incremental** (Welford):

```c
typedef struct {
    double mean;
    double M2;
    uint64_t count;
    double threshold;
} ZScoreDetector;

void zscore_init(ZScoreDetector* d, double threshold);
bool zscore_is_anomaly(ZScoreDetector* d, double value, double* out_zscore);
```

**Criterii de anomalie per tip senzor:**

| Senzor | Valori Normale | Threshold Z-Score |
|--------|----------------|-------------------|
| Umiditate sol | 30-70% | 2.5 |
| Temp. sol | 15-30°C | 2.5 |
| Temp. aer | 18-35°C | 2.5 |
| Umiditate aer | 40-80% | 2.5 |
| Lumină | 0-100000 lux | 3.0 |
| CO2 | 300-2000 ppm | 2.5 |

### Partea D: Rate Limiting Alerte (15 puncte)

Implementați un rate limiter pentru a preveni alert fatigue:

```c
typedef struct {
    uint64_t timestamps[MAX_ALERTS];
    int head;
    int count;
    uint64_t window_ms;
    uint64_t cooldown_until;
} RateLimiter;

void rate_limiter_init(RateLimiter* rl, int max_alerts, uint64_t window_ms);
bool rate_limiter_allow(RateLimiter* rl, uint64_t current_time);
```

**Cerințe:**
- Maximum 5 alerte per senzor în 60 secunde
- Cooldown de 30 secunde după depășirea limitei
- Logging al alertelor suprimate

### Partea E: Agregare și Raportare (15 puncte)

Implementați fereastră tumbling pentru agregare:

```c
typedef struct {
    double sum, min, max;
    int count;
    uint64_t window_start;
    uint64_t window_size_ms;
} TumblingWindow;

typedef struct {
    double mean, min, max;
    int count;
    uint64_t window_start;
    uint64_t window_end;
} WindowStats;

void window_init(TumblingWindow* w, uint64_t window_size_ms);
bool window_add(TumblingWindow* w, double value, uint64_t timestamp, WindowStats* out);
```

**Raport generat la fiecare 5 minute:**
```
=== RAPORT SERĂ 14:30 - 14:35 ===
Senzor              Media    Min      Max      Citiri   Anomalii
----------------------------------------------------------------
soil_moisture_01    45.2%    42.1%    48.7%    300      2
soil_temp_01        22.3°C   21.8°C   23.1°C   300      0
air_temp_01         28.5°C   26.2°C   31.2°C   300      1
...
Alerte generate: 3
Alerte suprimate: 7
================================================================
```

---

## 📊 Date de Test

Folosiți fișierele CSV furnizate în `data/`:
- `greenhouse_sensors_normal.csv` - date normale
- `greenhouse_sensors_anomalies.csv` - date cu anomalii injectate

**Format CSV:**
```csv
timestamp_ms,sensor_id,type,value,quality
1704067200000,soil_moisture_01,SENSOR_SOIL_MOISTURE,45.2,95
1704067200000,soil_temp_01,SENSOR_SOIL_TEMPERATURE,22.3,98
...
```

---

## 🔧 Cerințe Tehnice

1. **Limbaj:** C11 standard
2. **Compilare:** `gcc -std=c11 -Wall -Wextra -O2`
3. **Fără biblioteci externe** (doar C standard + POSIX)
4. **Memorie:** Toate structurile trebuie să aibă complexitate spațială O(1) sau O(k) pentru ferestre
5. **Fără memory leaks** (verificabil cu Valgrind)

---

## 📁 Structura Proiectului

```
tema1_numestudent/
├── Makefile
├── README.md
├── include/
│   ├── filters.h
│   ├── anomaly.h
│   ├── rate_limiter.h
│   └── aggregation.h
├── src/
│   ├── main.c
│   ├── filters.c
│   ├── anomaly.c
│   ├── rate_limiter.c
│   └── aggregation.c
├── tests/
│   └── test_all.c
└── data/
    └── (fișiere CSV test)
```

---

## ✅ Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Structuri de date corecte | 20 |
| Filtre implementate corect | 25 |
| Detecție anomalii funcțională | 25 |
| Rate limiting corect | 15 |
| Agregare și raportare | 15 |
| **Bonus:** Documentație excelentă | +5 |
| **Bonus:** Teste unitare comprehensive | +5 |
| **Penalizare:** Memory leaks | -10 |
| **Penalizare:** Nu compilează | -50 |

---

## 💡 Sugestii

1. Începeți cu structurile de date și testați individual fiecare componentă
2. Folosiți `assert()` pentru validare intrări
3. Implementați mai întâi versiuni simple, apoi optimizați
4. Testați cu date generate aleator înainte de CSV-urile furnizate
5. Verificați cu Valgrind: `valgrind --leak-check=full ./program`

---

## 📚 Resurse

- Algoritmul Welford: [Wikipedia](https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm)
- Filtrul Kalman: Slide-urile cursului
- Circular Buffer: `man 3 memmove`

---

## ❓ Întrebări Frecvente

**Q: Pot folosi `malloc`?**  
A: Da, dar trebuie să eliberați toată memoria alocată.

**Q: Ce fac dacă timestamp-urile nu sunt ordonate?**  
A: Presupuneți că sunt ordonate crescător. Nu trebuie să sortați.

**Q: Cum testez fără date CSV?**  
A: Generați date programatic cu `rand()` și seed fix pentru reproducibilitate.

---

*Succes! 🌱*
