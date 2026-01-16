# 📝 Tema 1: Sistem de Monitorizare Temperatură Industrial

## Săptămâna 19: Algoritmi pentru IoT și Stream Processing

---

## 📋 Descriere

O fabrică de componente electronice necesită monitorizarea temperaturii în **camera curată** 
(clean room) unde se produc cipuri. Temperatura trebuie menținută strict între **20°C și 24°C**.

Trebuie să implementați un sistem de monitorizare care:
1. Citește date de la senzori (simulat din fișier CSV)
2. Filtrează zgomotul folosind EMA
3. Detectează anomalii cu Z-score
4. Generează alerte cu rate limiting
5. Produce un raport final

---

## 🎯 Cerințe Funcționale

### Partea A: Structuri de Date (20 puncte)

Implementați următoarele structuri:

```c
// Filtru EMA
typedef struct {
    double alpha;
    double value;
    bool initialized;
} EMAFilter;

// Detector Z-score cu Welford
typedef struct {
    double mean;
    double M2;
    uint64_t count;
    double threshold;
} ZScoreDetector;

// Rate limiter sliding window
typedef struct {
    uint64_t* timestamps;  // Buffer circular
    int capacity;
    int head;
    int count;
    uint64_t window_ms;
    uint64_t cooldown_until;
} RateLimiter;

// Context per-senzor
typedef struct {
    char sensor_id[32];
    EMAFilter filter;
    ZScoreDetector detector;
    RateLimiter limiter;
    // Statistici
    int total_readings;
    int anomalies_detected;
    int alerts_sent;
    int alerts_suppressed;
    double min_value;
    double max_value;
} SensorContext;
```

### Partea B: Funcții Core (40 puncte)

1. **`ema_init(EMAFilter* f, double alpha)`** - Inițializare filtru
2. **`ema_update(EMAFilter* f, double value)`** - Returnează valoarea filtrată
3. **`zscore_init(ZScoreDetector* d, double threshold)`** - Inițializare detector
4. **`zscore_check(ZScoreDetector* d, double value)`** - Returnează (is_anomaly, zscore)
5. **`rate_limiter_init(RateLimiter* rl, int capacity, uint64_t window_ms)`**
6. **`rate_limiter_allow(RateLimiter* rl, uint64_t timestamp)`** - Returnează true dacă alertă permisă
7. **`process_reading(SensorContext* ctx, double value, uint64_t timestamp)`** - Procesare completă

### Partea C: Logica de Business (20 puncte)

Implementați verificări suplimentare:
- Alertă **CRITICAL** dacă temperatura filtrată > 26°C sau < 18°C
- Alertă **WARNING** dacă temperatura filtrată e în afara [20°C, 24°C]
- Alertă **ANOMALY** dacă Z-score > threshold

```c
typedef enum {
    ALERT_NONE = 0,
    ALERT_WARNING,
    ALERT_CRITICAL,
    ALERT_ANOMALY
} AlertLevel;

AlertLevel check_thresholds(double filtered_value, bool is_anomaly);
```

### Partea D: Raportare (20 puncte)

La finalul procesării, generați un raport text cu:
- Statistici per senzor (min, max, media, deviație standard)
- Numărul de anomalii detectate
- Numărul de alerte trimise vs. suprimate
- Eficiența rate limiter-ului

---

## 📁 Format Fișier Input

Fișierul `temperature_data.csv`:

```csv
timestamp,sensor_id,temperature
1704067200000,sensor_001,22.5
1704067201000,sensor_001,22.6
1704067202000,sensor_001,22.4
1704067203000,sensor_001,35.2
1704067204000,sensor_001,22.5
...
```

---

## 🔧 Parametri Recomandați

| Parametru | Valoare | Justificare |
|-----------|---------|-------------|
| EMA α | 0.2 | Netezire moderată |
| Z-score threshold | 2.5 | Standard pentru outliers |
| Rate limit window | 60 sec | 1 minut |
| Max alerte/window | 3 | Previne spam |
| Cooldown after burst | 30 sec | Permite reset |

---

## 📊 Exemplu Output

```
=== RAPORT MONITORIZARE TEMPERATURĂ ===
Perioada: 2024-01-01 08:00:00 - 2024-01-01 18:00:00
Senzori procesați: 5

SENZOR: sensor_001
  Citiri procesate: 36000
  Temperatură: min=19.8°C, max=24.3°C, avg=22.1°C, σ=0.8°C
  Anomalii detectate: 12
  Alerte trimise: 8
  Alerte suprimate: 4 (33.3% rate limiting)

SENZOR: sensor_002
  ...

SUMAR GLOBAL:
  Total anomalii: 47
  Total alerte: 31
  Eficiență rate limiter: 34.0%
```

---

## ✅ Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Compilare fără warnings (-Wall -Wextra) | 10 |
| Corectitudinea EMA | 15 |
| Corectitudinea Z-score Welford | 15 |
| Corectitudinea Rate Limiter | 15 |
| Logica threshold-uri | 15 |
| Raport corect și complet | 15 |
| Stil cod, comentarii | 10 |
| Bonus: Valgrind clean | +5 |
| **TOTAL** | **100** |

---

## 🚀 Testare

```bash
# Compilare
gcc -Wall -Wextra -std=c11 -o tema1 tema1.c -lm

# Rulare cu date test
./tema1 temperature_data.csv

# Verificare memorie
valgrind --leak-check=full ./tema1 temperature_data.csv
```

---

## 📚 Resurse

- Slide-urile săptămânii 19
- `example1.c` - implementare de referință
- [Welford's Algorithm](https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm)

---

## 📅 Termen Limită

**Data**: Vezi platforma de cursuri  
**Predare**: Arhivă ZIP cu:
- `tema1.c` - codul sursă
- `README.txt` - instrucțiuni compilare, observații
- `output.txt` - exemplu de output

---

*Algoritmi și Tehnici de Programare - ASE București, CSIE*
