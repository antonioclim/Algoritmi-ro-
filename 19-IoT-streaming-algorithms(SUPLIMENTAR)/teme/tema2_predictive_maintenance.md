# 📋 Tema 2: Sistem de Mentenanță Predictivă Industrială

## Săptămâna 19 - Algoritmi pentru IoT și Stream Processing

**Termen limită:** 3 săptămâni de la predare  
**Punctaj maxim:** 100 puncte + 20 bonus  
**Nivel:** Avansat  
**Mod de predare:** Arhivă ZIP + demo video (opțional pentru bonus)

---

## 🎯 Obiectiv

Implementați un sistem de monitorizare pentru **mentenanță predictivă** a unei linii de producție industriale. Sistemul trebuie să:

1. Proceseze date de la senzori de vibrație și temperatură
2. Detecteze degradarea echipamentelor înainte de defectare
3. Clasifice tipul de defect probabil
4. Estimeze timpul rămas până la defecțiune (RUL - Remaining Useful Life)
5. Genereze alerte prioritizate cu acționare automată

---

## 🏭 Contextul Industrial

Monitorizați o **pompă centrifugală** cu următorii senzori:

| Senzor | Tip | Frecvență | Unitate | Range Normal |
|--------|-----|-----------|---------|--------------|
| vibration_x | Accelerometru | 1000 Hz | g | 0-2 |
| vibration_y | Accelerometru | 1000 Hz | g | 0-2 |
| vibration_z | Accelerometru | 1000 Hz | g | 0-2 |
| temperature_bearing | Termocuplu | 10 Hz | °C | 40-70 |
| temperature_motor | Termocuplu | 10 Hz | °C | 50-80 |
| current_motor | Senzor curent | 10 Hz | A | 8-12 |
| pressure_inlet | Presiune | 10 Hz | bar | 2-4 |
| pressure_outlet | Presiune | 10 Hz | bar | 6-10 |

---

## 📝 Cerințe Funcționale

### Partea A: Procesare Semnal Vibrație (25 puncte)

Implementați analiza semnalului de vibrație în domeniul frecvență folosind FFT simplificat (DFT pentru ferestre mici):

```c
typedef struct {
    double* buffer;
    int size;
    int head;
    int count;
} SignalBuffer;

typedef struct {
    double frequency;
    double magnitude;
} FrequencyComponent;

typedef struct {
    FrequencyComponent dominant[5];  // Top 5 componente
    double rms;                       // Root Mean Square
    double peak;                      // Valoare maximă
    double crest_factor;             // Peak / RMS
} VibrationAnalysis;

// Funcții de implementat
void signal_buffer_init(SignalBuffer* sb, int size);
void signal_buffer_add(SignalBuffer* sb, double value);
VibrationAnalysis analyze_vibration(SignalBuffer* sb, double sample_rate);
```

**Indicatori de defect din spectru:**
- **Dezechilibru rotor:** Frecvență dominantă = RPM
- **Dezaliniere:** Frecvență 2× RPM
- **Rulmenți defecți:** Frecvențe înalte (> 1kHz) + armonice
- **Cavitație:** Zgomot broadband + frecvențe aleatorii

### Partea B: Detecție Multi-Variată (25 puncte)

Implementați detector de anomalii care consideră **corelații** între senzori:

```c
typedef struct {
    double means[MAX_SENSORS];
    double covariance[MAX_SENSORS][MAX_SENSORS];  // Matricea de covarianță
    uint64_t count;
    int n_sensors;
} MultivariateDetector;

typedef struct {
    bool is_anomaly;
    double mahalanobis_distance;
    int* contributing_sensors;  // Senzorii care contribuie la anomalie
    int n_contributing;
} AnomalyResult;

void mv_detector_init(MultivariateDetector* d, int n_sensors);
void mv_detector_update(MultivariateDetector* d, double* values);
AnomalyResult mv_detector_check(MultivariateDetector* d, double* values, 
                                 double threshold);
```

**Formula distanței Mahalanobis (simplificată pentru 2D):**
```
D² = (x - μ)ᵀ Σ⁻¹ (x - μ)
```

### Partea C: Clasificare Tip Defect (20 puncte)

Implementați un clasificator simplu bazat pe reguli pentru tipul de defect:

```c
typedef enum {
    FAULT_NONE,
    FAULT_IMBALANCE,        // Dezechilibru
    FAULT_MISALIGNMENT,     // Dezaliniere
    FAULT_BEARING_WEAR,     // Uzură rulmenți
    FAULT_CAVITATION,       // Cavitație pompă
    FAULT_OVERHEATING,      // Supraîncălzire
    FAULT_ELECTRICAL,       // Problemă electrică
    FAULT_UNKNOWN
} FaultType;

typedef struct {
    FaultType type;
    double confidence;      // 0-1
    const char* description;
    const char* recommended_action;
} FaultClassification;

FaultClassification classify_fault(
    VibrationAnalysis* vib,
    double temp_bearing,
    double temp_motor,
    double current,
    double pressure_diff
);
```

**Reguli de clasificare:**

| Condiție | Diagnostic | Acțiune |
|----------|------------|---------|
| Vib. 1×RPM > 2g | Dezechilibru | Echilibrare rotor |
| Vib. 2×RPM > 1.5g | Dezaliniere | Realiniere cuplaj |
| Temp bearing > 80°C + current normal | Rulmenți | Înlocuire rulmenți |
| Crest factor > 5 | Defect rulmenți | Inspecție urgentă |
| Temp motor > 90°C + current > 14A | Suprasarcină | Reducere sarcină |
| Presiune outlet scade + zgomot broadband | Cavitație | Verificare NPSH |

### Partea D: Estimare RUL (15 puncte)

Implementați estimare simplificată a Remaining Useful Life bazată pe trend:

```c
typedef struct {
    double values[HISTORY_SIZE];
    uint64_t timestamps[HISTORY_SIZE];
    int head;
    int count;
    double failure_threshold;
} RULEstimator;

typedef struct {
    double estimated_hours;
    double confidence;
    double current_degradation;  // 0-100%
    double trend_rate;           // Degradare per oră
} RULEstimate;

void rul_init(RULEstimator* e, double failure_threshold);
void rul_update(RULEstimator* e, double health_indicator, uint64_t timestamp);
RULEstimate rul_estimate(RULEstimator* e);
```

**Health Indicator:**
```c
// Combinație ponderată a indicatorilor
health = 100 - (
    0.3 * normalize(vibration_rms, 0, 5) +
    0.3 * normalize(temp_bearing, 40, 100) +
    0.2 * normalize(crest_factor, 2, 10) +
    0.2 * normalize(current_deviation, 0, 5)
) * 100;
```

### Partea E: Alertare Prioritizată (15 puncte)

Implementați sistem de alerte cu priorități și escaladare:

```c
typedef enum {
    PRIORITY_INFO,      // Logging doar
    PRIORITY_LOW,       // Email la shift end
    PRIORITY_MEDIUM,    // Email imediat
    PRIORITY_HIGH,      // SMS + email
    PRIORITY_CRITICAL   // Oprire automată + apel
} AlertPriority;

typedef struct {
    AlertPriority priority;
    FaultType fault;
    double rul_hours;
    const char* message;
    uint64_t timestamp;
    bool acknowledged;
} Alert;

typedef struct {
    Alert active_alerts[MAX_ACTIVE_ALERTS];
    int count;
    // Escaladare
    uint64_t escalation_times[5];  // Timp până la escaladare per nivel
} AlertManager;

void alert_manager_init(AlertManager* am);
void alert_generate(AlertManager* am, FaultType fault, double confidence, 
                    RULEstimate* rul);
void alert_acknowledge(AlertManager* am, int alert_id);
void alert_check_escalation(AlertManager* am, uint64_t current_time);
```

**Logica prioritizare:**

| RUL | Confidence | Prioritate |
|-----|------------|------------|
| < 24h | > 80% | CRITICAL |
| < 72h | > 80% | HIGH |
| < 168h | > 60% | MEDIUM |
| < 336h | > 40% | LOW |
| orice | < 40% | INFO |

---

## 📊 Date de Test

Fișierele din `data/industrial/`:
- `pump_normal_operation.csv` - 1 oră funcționare normală
- `pump_bearing_degradation.csv` - Degradare graduală rulmenți (72h comprimate)
- `pump_sudden_failure.csv` - Defect brusc (cavitație)
- `pump_multiple_faults.csv` - Scenarii multiple

**Format:**
```csv
timestamp_ms,vibration_x,vibration_y,vibration_z,temp_bearing,temp_motor,current,pressure_in,pressure_out
1704067200000,0.45,0.52,0.38,52.3,65.1,10.2,3.1,8.2
...
```

---

## 🔧 Cerințe Tehnice

1. **Performanță:** Procesare în timp real (< 10ms per batch de 100 samples)
2. **Memorie:** Maximum 1MB alocare dinamică
3. **Thread-safety:** Opțional, dar bonus dacă implementat
4. **Logging:** Fișier log cu toate evenimentele importante

---

## 📁 Structura Proiectului

```
tema2_numestudent/
├── Makefile
├── README.md
├── docs/
│   └── architecture.md
├── include/
│   ├── signal_analysis.h
│   ├── multivariate_detector.h
│   ├── fault_classifier.h
│   ├── rul_estimator.h
│   └── alert_manager.h
├── src/
│   ├── main.c
│   ├── signal_analysis.c
│   ├── multivariate_detector.c
│   ├── fault_classifier.c
│   ├── rul_estimator.c
│   └── alert_manager.c
├── tests/
│   ├── test_signal.c
│   ├── test_detector.c
│   └── test_integration.c
└── data/
    └── industrial/
```

---

## ✅ Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Analiza vibrație (FFT/DFT) | 25 |
| Detecție multivariată | 25 |
| Clasificare defecte | 20 |
| Estimare RUL | 15 |
| Alertare prioritizată | 15 |
| **Bonus:** Demo video funcțional | +10 |
| **Bonus:** Vizualizare grafică (ncurses/SDL) | +10 |
| **Penalizare:** Performanță slabă (>100ms/batch) | -15 |
| **Penalizare:** Memory leaks | -20 |

---

## 💡 Sugestii Avansate

1. **FFT:** Pentru simplitate, implementați DFT direct pentru ferestre mici (64-256 samples). FFT-ul Cooley-Tukey este opțional.

2. **Matricea de covarianță:** Actualizați incremental:
   ```c
   // Update incremental (simplificat)
   cov[i][j] += (x[i] - mean[i]) * (x[j] - mean[j]) / count;
   ```

3. **RUL cu regresie liniară:**
   ```c
   // Găsește t când health = threshold
   // health(t) = a*t + b
   // t_failure = (threshold - b) / a
   ```

4. **Testare:** Generați date sintetice cu defecte cunoscute pentru validare.

---

## 📚 Resurse

- ISO 10816: Vibration severity standards
- ISO 13381: Condition monitoring and diagnostics
- Bearing frequencies: SKF Calculator
- [Predictive Maintenance Overview](https://www.nist.gov/el/intelligent-systems-division-73500/condition-monitoring)

---

## 🏆 Competiție (Opțional)

Studenții cu cele mai bune sisteme (evaluare pe set de date secret) vor primi:
- Locul 1: +20 puncte bonus + mențiune
- Locul 2-3: +10 puncte bonus
- Locul 4-10: +5 puncte bonus

Criterii competiție:
- Acuratețe detecție (F1 score)
- Timp de avertizare înainte de defect
- Rata false pozitive

---

*Succes la implementare! ⚙️*
