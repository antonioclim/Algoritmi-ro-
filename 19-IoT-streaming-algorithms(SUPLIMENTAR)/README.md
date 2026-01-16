# Săptămâna 19: Algoritmi pentru IoT și Stream Processing

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, studentul va fi capabil să:

1. **Recunoască** caracteristicile fundamentale ale procesării în flux (single-pass, memorie limitată, latență), tipurile de ferestre temporale (tumbling, sliding, session) și formulele pentru filtrele digitale Moving Average și Exponential Moving Average
2. **Înțeleagă** compromisurile arhitecturale edge computing vs. cloud processing, rațiunea pentru care filtrarea la sursă reduce costurile de bandă și latență, precum și principiile de funcționare ale Kalman Filter 1D
3. **Aplice** implementări complete în C pentru: Moving Average, Exponential Moving Average, Kalman Filter 1D simplificat, detectoare de anomalii (Z-score, IQR), agregare pe ferestre temporale și simulare de protocoale IoT
4. **Analizeze** latența end-to-end în pipeline-uri de procesare senzori, impactul dimensiunii ferestrei asupra latenței și acurateții, precum și trade-off-urile între diferite strategii de filtrare
5. **Evalueze** scenariile optime pentru edge vs. cloud processing în funcție de cerințele de latență, volum de date și resurse computaționale disponibile
6. **Creeze** un pipeline complet de procesare senzori cu simulare Docker, integrând filtrare, agregare și detecție de anomalii într-un sistem funcțional

---

## 📜 Context Istoric

### Originile Internet of Things și Stream Processing

Conceptul de „Internet of Things" a fost articulat pentru prima dată de Kevin Ashton în 1999, în contextul optimizării lanțurilor de aprovizionare folosind etichete RFID. Însă rădăcinile teoretice ale procesării în flux se întind mult mai devreme, în domeniul teoriei controlului și al procesării semnalelor digitale.

Filtrarea digitală – fundamentul procesării senzorilor – își are originile în lucrările lui Claude Shannon privind teoria informației (1948) și în dezvoltarea sistemelor de control în timp real pentru aplicații aerospațiale. Necesitatea de a procesa date în mod continuu, fără a le stoca integral, a condus la dezvoltarea algoritmilor de streaming care operează în regim single-pass, folosind memorie constantă sau sublineară.

Revoluția IoT din anii 2010 a adus provocări fără precedent: miliarde de dispozitive generând terabyți de date pe secundă, necesitând procesare în timp real la edge pentru a reduce latența și costurile de transmisie. Această paradigmă a revitalizat interesul pentru algoritmii clasici de procesare a semnalelor, adaptați pentru hardware cu resurse limitate.

### Figură Cheie: Rudolf E. Kálmán (1930-2016)

```
    ┌─────────────────────────────────────────────────────────────────┐
    │  RUDOLF EMIL KÁLMÁN                                             │
    │  Matematician și inginer electrotehnist maghiaro-american       │
    │                                                                 │
    │  • Născut în Budapesta, emigrat în SUA în 1943                  │
    │  • PhD Stanford University, 1957                                │
    │  • Filtrul Kalman publicat în 1960                              │
    │  • Contribuții fundamentale la teoria controlului modern        │
    │  • National Medal of Science (2008), Kyoto Prize (1985)        │
    │                                                                 │
    │  Impactul filtrului Kalman:                                     │
    │  • Navigația programului Apollo (aterizare pe Lună)             │
    │  • Sisteme GPS moderne                                          │
    │  • Vehicule autonome și dronelor                                 │
    │  • Procesare financiară și economică                            │
    └─────────────────────────────────────────────────────────────────┘
```

> *„A good research problem should have at least three solutions, two of which should be wrong."*
> — Rudolf E. Kálmán

### Kevin Ashton (1968-)

Kevin Ashton, cercetător britanic, a inventat termenul „Internet of Things" în 1999 în timp ce lucra la Procter & Gamble, unde folosea tehnologia RFID pentru gestionarea lanțului de aprovizionare. Ulterior a co-fondat Auto-ID Center la MIT, organizație care a dezvoltat standardele pentru identificarea obiectelor prin Internet.

### Andy Stanford-Clark și Arlen Nipper (MQTT, 1999)

Protocolul MQTT (Message Queuing Telemetry Transport) a fost dezvoltat în 1999 de Andy Stanford-Clark (IBM) și Arlen Nipper (Eurotech) pentru monitorizarea conductelor de petrol prin conexiuni satelitare cu bandă limitată. Astăzi, MQTT este standardul de facto pentru comunicarea în sistemele IoT.

---

## 📚 Fundamente Teoretice

### 1. Caracteristicile Stream Processing

Procesarea în flux diferă fundamental de procesarea batch tradițională:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      BATCH vs. STREAM PROCESSING                            │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   BATCH PROCESSING                    STREAM PROCESSING                     │
│   ┌─────────────┐                     ───────────────────►                  │
│   │   ████████  │                     ○ → ○ → ○ → ○ → ○ → ...               │
│   │   ████████  │                                                           │
│   │   ████████  │  ← date stocate     ↑ procesare imediată                  │
│   └─────────────┘                                                           │
│                                                                             │
│   • Date disponibile complet          • Date sosesc continuu                │
│   • Acces aleator la elemente         • Single-pass (o singură trecere)     │
│   • Memorie: O(n)                     • Memorie: O(1) sau O(√n)             │
│   • Latență: minute/ore               • Latență: milisecunde/secunde        │
│   • Rezultat exact                    • Rezultat aproximativ (acceptabil)   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**Constrângeri fundamentale în stream processing:**
- **Single-pass**: fiecare element poate fi procesat o singură dată
- **Memorie limitată**: nu putem stoca întregul flux
- **Timp real**: rezultatele trebuie produse cu latență minimă
- **Ordine imperfectă**: elementele pot sosi out-of-order

### 2. Ferestre Temporale (Windowing)

Pentru a produce rezultate semnificative din fluxuri infinite, grupăm datele în ferestre:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         TIPURI DE FERESTRE                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   TUMBLING WINDOW (fereastră fixă, non-overlapping)                         │
│   ┌──────┐┌──────┐┌──────┐┌──────┐                                         │
│   │ W1   ││ W2   ││ W3   ││ W4   │                                         │
│   └──────┘└──────┘└──────┘└──────┘                                         │
│   ────●──●──●────●──●──●──●────●──●────●──●──●──●──►  timp                  │
│                                                                             │
│   SLIDING WINDOW (fereastră glisantă, overlapping)                          │
│   ┌──────────────┐                                                          │
│      ┌──────────────┐                                                       │
│         ┌──────────────┐                                                    │
│   ───●──●──●──●──●──●──●──●──●──►  timp                                    │
│   slide = distanța între începuturi                                         │
│                                                                             │
│   SESSION WINDOW (bazată pe gap de activitate)                              │
│   ┌─────────┐    ┌───────────────┐  ┌────┐                                 │
│   │ Sesiune │    │   Sesiune     │  │Ses.│                                 │
│   └─────────┘    └───────────────┘  └────┘                                 │
│   ───●─●─●───────●──●──●──●──●──────●─●──►  timp                           │
│        ↑ gap       ↑ gap                                                    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**Structura în C pentru fereastră tumbling:**

```c
typedef struct {
    double *buffer;         /* Buffer circular pentru valori */
    uint64_t *timestamps;   /* Timestamps pentru fiecare valoare */
    size_t capacity;        /* Dimensiunea maximă */
    size_t count;           /* Numărul curent de elemente */
    size_t head;            /* Index pentru scriere */
    uint64_t window_start;  /* Timestamp început fereastră */
    uint64_t window_size_ms;/* Dimensiunea ferestrei în ms */
} TumblingWindow;
```

### 3. Filtre Digitale

**Moving Average (MA):**
```
MA_t = (x_t + x_{t-1} + ... + x_{t-n+1}) / n

Avantaje:                       Dezavantaje:
• Simplu de implementat         • Memorie O(n) pentru buffer
• Netezire eficientă           • Latență de n/2 samples
• Fără dependență de parametri  • Ponderi egale (neoptim)
```

**Exponential Moving Average (EMA):**
```
EMA_t = α × x_t + (1-α) × EMA_{t-1}

unde α ∈ (0, 1) controlează „memoria" filtrului:
  α mare → urmărire rapidă, zgomot mai mult
  α mic  → urmărire lentă, netezire puternică

Avantaje:                       Dezavantaje:
• Memorie O(1)                  • Necesită tuning al lui α
• Ponderi exponențiale          • Dificil de interpretat
• Actualizare incrementală      • Sensibil la outliers
```

**Kalman Filter 1D (simplificat):**
```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       KALMAN FILTER - INTUIȚIE                              │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Predicție     Măsurătură      Estimare finală                             │
│   (a priori)    (observație)    (a posteriori)                              │
│       ↓              ↓               ↓                                      │
│       ●──────────────●───────────────●                                      │
│       │              │               │                                      │
│   Ne așteptăm    Vedem în       Combinăm ponderat                          │
│   să fie aici    realitate      în funcție de încredere                    │
│                                                                             │
│   Ecuații simplificate (1D):                                                │
│   ┌────────────────────────────────────────────────────────────┐           │
│   │  Kalman Gain: K = P / (P + R)                              │           │
│   │  Update estimare: x = x + K × (z - x)                      │           │
│   │  Update eroare: P = (1 - K) × P + Q                        │           │
│   │                                                            │           │
│   │  unde: x = estimare, z = măsurătură, P = eroare estimare   │           │
│   │        R = zgomot măsurătură, Q = zgomot proces            │           │
│   └────────────────────────────────────────────────────────────┘           │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 4. Detecția Anomaliilor

**Z-Score Detector:**
```
Z = (x - μ) / σ

Anomalie dacă |Z| > threshold (tipic 2 sau 3)

┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│          ▄▄▄▄▄▄                                                │
│        ▄████████▄              Normal: |Z| ≤ 2                 │
│       ██████████████                                           │
│      ████████████████          Suspect: 2 < |Z| ≤ 3            │
│  ───────────────────────────                                   │
│   -3σ   -2σ   -1σ   μ   1σ   2σ   3σ   Anomalie: |Z| > 3      │
│     ↑           ↑       ↑           ↑                          │
│  0.1%        2.3%     2.3%       0.1%                          │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

**IQR (Interquartile Range) Detector:**
```
IQR = Q3 - Q1

Limite:
  Lower = Q1 - k × IQR
  Upper = Q3 + k × IQR
  (k tipic = 1.5)

Anomalie dacă x < Lower sau x > Upper
```

### 5. Arhitectura Edge vs. Cloud

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    EDGE vs. CLOUD PROCESSING                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   EDGE COMPUTING                                                            │
│   ┌─────────┐      ┌─────────┐                                             │
│   │ Senzor  │─────►│  Edge   │──── date agregate ────► Cloud               │
│   │         │      │ Gateway │                                              │
│   └─────────┘      └─────────┘                                             │
│                    ↓ procesare locală                                       │
│                    • Filtrare                                               │
│                    • Agregare                                               │
│                    • Detecție anomalii                                      │
│                                                                             │
│   Avantaje:                        Dezavantaje:                             │
│   ✓ Latență foarte mică           ✗ Resurse limitate                       │
│   ✓ Funcționare offline           ✗ Management distribuit                  │
│   ✓ Reducere bandă/costuri        ✗ Update-uri mai complexe                │
│   ✓ Confidențialitate date        ✗ Hardware la distanță                   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 🏭 Aplicații Industriale

### 1. Smart Building Management

```c
/* Pseudocod pentru monitorizare clădire inteligentă */
typedef struct {
    int sensor_id;
    double temperature;
    double humidity;
    bool motion_detected;
    uint64_t timestamp;
} RoomReading;

/* Pipeline: Sensor → Filter → Aggregate → Anomaly → Action */
void smart_building_pipeline(RoomReading reading) {
    /* 1. Filtrare zgomot */
    double filtered_temp = ema_update(&temp_filter, reading.temperature);
    
    /* 2. Detectare anomalie */
    if (zscore_is_anomaly(&temp_stats, filtered_temp)) {
        trigger_alert(ALERT_TEMPERATURE, reading.sensor_id);
    }
    
    /* 3. Agregare pentru raportare */
    window_add(&hourly_window, filtered_temp, reading.timestamp);
    
    /* 4. Acțiune automată */
    if (filtered_temp < 18.0 && !hvac_is_on(reading.sensor_id)) {
        hvac_start_heating(reading.sensor_id);
    }
}
```

### 2. Industrial IoT - Predictive Maintenance

Detectarea din timp a defecțiunilor în echipamente industriale prin analiza vibrațiilor și temperaturii.

### 3. Agricultura de Precizie

Monitorizarea umidității solului, temperaturii și luminozității pentru irigare optimă și protecția culturilor.

---

## 💻 Exerciții de Laborator

### Exercițiul 1: Smart Building Monitor

**Obiectiv:** Implementarea unui sistem de monitorizare pentru o clădire inteligentă cu 10 senzori simulați.

**Cerințe:**
1. Simularea a 10 senzori de temperatură cu zgomot gaussian
2. Implementarea filtrului EMA cu α ajustabil
3. Agregare în ferestre de 5 minute (mean, min, max)
4. Detectarea camerelor cu temperatură anormală
5. Raport periodic cu statistici

**Fișier:** `src/exercise1.c`

### Exercițiul 2: Anomaly Detector cu Rate Limiting

**Obiectiv:** Implementarea unui detector de anomalii în flux cu protecție împotriva alertelor excesive.

**Cerințe:**
1. Z-score detector cu fereastră adaptivă pentru μ și σ
2. Rate limiting: maxim 3 alerte per senzor pe minut
3. Cooldown de 30 secunde între alerte consecutive
4. Log cu context (valoare, timestamp, z-score)
5. Statistici finale: total alerte, false positive rate estimat

**Fișier:** `src/exercise2.c`

---

## 🔧 Compilare și Execuție

```bash
# Compilare completă
make all

# Rulare exemplu demonstrativ
make run

# Compilare și rulare exercițiu 1 (Smart Building)
make exercise1
./exercise1

# Compilare și rulare exercițiu 2 (Anomaly Detector)
make exercise2
./exercise2 < data/sensor_temperature.csv

# Rulare simulatoare
make simulators
./virtual_sensor

# Verificare cu teste automate
make test

# Verificare memory leaks cu Valgrind
make valgrind

# Curățare fișiere generate
make clean

# Ajutor comenzi disponibile
make help
```

---

## 🐳 Rulare în Docker

```bash
# Construiește imaginea
docker build -t atp-week19 .

# Rulează containerul interactiv
docker run -it --rm -v $(pwd):/workspace atp-week19

# În container:
make all && make run

# Sau folosind docker-compose pentru setup complet cu Mosquitto:
docker-compose up -d
docker-compose exec app make run
docker-compose down
```

---

## 📁 Structura Directorului

```
19-iot-streaming-algorithms/
├── README.md                           # Acest fișier
├── Makefile                            # Automatizare build și execuție
├── Dockerfile                          # Container standalone
├── docker-compose.yml                  # Setup cu Mosquitto MQTT broker
│
├── slides/
│   ├── presentation-week19.html        # Prezentare principală (35+ slides)
│   └── presentation-comparativ.html    # Comparație Pseudocod/C/Python
│
├── src/
│   ├── example1.c                      # Exemplu complet pipeline IoT
│   ├── exercise1.c                     # Exercițiu: Smart Building Monitor
│   └── exercise2.c                     # Exercițiu: Anomaly Detector
│
├── simulators/
│   ├── virtual_sensor.c                # Simulator senzori
│   ├── mqtt_publisher.c                # Publisher MQTT (simulat)
│   └── mqtt_subscriber.c               # Subscriber MQTT (simulat)
│
├── arduino_sketches/
│   ├── README_ARDUINO.md               # Instrucțiuni hardware opțional
│   ├── temperature_sensor.ino          # Cod Arduino pentru DHT22
│   └── mqtt_esp32.ino                  # ESP32 cu MQTT
│
├── data/
│   ├── sensor_temperature.csv          # Date temperatură simulate
│   ├── sensor_humidity.csv             # Date umiditate simulate
│   ├── sensor_motion.csv               # Date mișcare simulate
│   └── anomalies_labeled.csv           # Date cu anomalii etichetate
│
├── tests/
│   ├── test1_input.txt                 # Input test exercițiu 1
│   ├── test1_expected.txt              # Output așteptat exercițiu 1
│   ├── test2_input.txt                 # Input test exercițiu 2
│   └── test2_expected.txt              # Output așteptat exercițiu 2
│
├── python_comparison/
│   ├── pandas_streaming.py             # Echivalent pandas pentru streaming
│   └── anomaly_detection.py            # Detecție anomalii în Python
│
├── teme/
│   ├── homework-requirements.md        # Cerințe teme (2 × 50p)
│   └── homework-extended.md            # Provocări bonus (5 × 10p)
│
└── solution/
    ├── exercise1_sol.c                 # Soluție exercițiu 1
    ├── exercise2_sol.c                 # Soluție exercițiu 2
    ├── homework1_sol.c                 # Soluție temă 1
    └── homework2_sol.c                 # Soluție temă 2
```

---

## 📖 Bibliografie Recomandată

### Resurse Esențiale
- **Kleppmann, M.** - *Designing Data-Intensive Applications*, Cap. 11: Stream Processing
- **Marz, N. & Warren, J.** - *Big Data: Principles and Best Practices*, Lambda Architecture
- **Welch, G. & Bishop, G.** - *An Introduction to the Kalman Filter* (Tutorial UNC)

### Resurse Avansate
- **Kálmán, R.E.** - *A New Approach to Linear Filtering and Prediction Problems* (1960)
- **Flajolet, P. et al.** - *HyperLogLog in Practice* (referință pentru structuri probabilistice)
- **Cormode, G.** - *Synopses for Massive Data* (survey tehnic)

### Resurse Online
- Apache Kafka Streams Documentation: https://kafka.apache.org/documentation/streams/
- MQTT.org Protocol Specification: https://mqtt.org/
- Kalman Filter Tutorial (Bilgin's Blog): https://www.kalmanfilter.net/
- ESP32 Arduino Core: https://github.com/espressif/arduino-esp32

---

## ✅ Lista de Auto-Evaluare

Înainte de a considera această săptămână completă, verifică:

- [ ] Pot explica diferența între procesare batch și stream processing
- [ ] Înțeleg cele trei tipuri de ferestre temporale și când să le folosesc
- [ ] Pot implementa Moving Average și EMA în C
- [ ] Înțeleg principiul Kalman Filter 1D și când este util
- [ ] Pot implementa Z-score și IQR pentru detecția anomaliilor
- [ ] Înțeleg trade-off-urile edge vs. cloud computing
- [ ] Pot proiecta un pipeline complet de procesare senzori
- [ ] Pot simula senzori cu zgomot gaussian realist
- [ ] Înțeleg protocolul MQTT și modelul publish-subscribe
- [ ] Pot adapta algoritmii pentru cerințe specifice de latență/acuratețe

---

## 💼 Pregătire pentru Interviuri

### Întrebări Frecvente

1. **„Explică diferența între Moving Average și Exponential Moving Average."**
   - MA: fereastră fixă de n valori, ponderi egale, memorie O(n)
   - EMA: toată istoria, ponderi exponențial descrescătoare, memorie O(1)
   - EMA reacționează mai rapid la schimbări, MA este mai stabil

2. **„Cum ai detecta anomalii într-un stream de date în timp real?"**
   - Z-score cu statistici running (Welford's algorithm)
   - Threshold adaptiv bazat pe variabilitatea recentă
   - Exponential smoothing pentru baseline
   - Rate limiting pentru evitarea alert storms

3. **„Ce este Kalman Filter și când l-ai folosi?"**
   - Estimator optim pentru sisteme liniare cu zgomot gaussian
   - Combină predicții cu măsurători, ponderate după incertitudine
   - Folosit în GPS, navigație, tracking, robotică
   - Pentru IoT: fuziune senzori, reducere zgomot

4. **„Edge computing vs. cloud processing - când folosești fiecare?"**
   - Edge: latență critică, bandă limitată, confidențialitate, operare offline
   - Cloud: analiză complexă, ML training, agregare cross-device, storage

5. **„Cum ai proiecta un sistem IoT scalabil pentru 1 milion de senzori?"**
   - Edge aggregation pentru reducere volum
   - Message broker distribuit (Kafka, MQTT)
   - Time-series database optimizată
   - Sampling adaptiv bazat pe variabilitate

---

## 🔗 Previzualizare Săptămâna 20

**Tema:** Programare Paralelă și Concurentă în C

Săptămâna viitoare vom:
- Explora pthread-uri și sincronizare cu mutex/semafoare
- Implementa thread pools și pattern-ul producer-consumer
- Paraleliza algoritmi de sortare (Parallel Merge Sort)
- Descoperi C11 atomics și structuri lock-free
- Analiza speedup și legea lui Amdahl

Legătura cu IoT: procesarea paralelă a datelor de la multiple surse de senzori.

---

## 🔧 Hardware Real (Opțional)

```
╔═══════════════════════════════════════════════════════════════════════════════╗
║  HARDWARE FIZIC - DISPONIBIL LA BIBLIOTECA FACULTĂȚII                        ║
╠═══════════════════════════════════════════════════════════════════════════════╣
║                                                                               ║
║  Kit-uri disponibile pentru împrumut (max. 2 săptămâni):                      ║
║                                                                               ║
║  1. ESP32 DevKit V1                                                          ║
║     • WiFi 802.11 b/g/n + Bluetooth 4.2                                      ║
║     • Procesor dual-core 240 MHz                                             ║
║     • 520 KB SRAM                                                            ║
║     • Ideal pentru proiecte IoT cu conectivitate                             ║
║                                                                               ║
║  2. Arduino Due                                                              ║
║     • ARM Cortex-M3 32-bit @ 84 MHz                                          ║
║     • 512 KB Flash, 96 KB SRAM                                               ║
║     • Mai multă memorie pentru algoritmi complecși                           ║
║                                                                               ║
║  3. Senzori disponibili:                                                     ║
║     • DHT22 - temperatură și umiditate                                       ║
║     • PIR - detecție mișcare                                                 ║
║     • LDR - luminozitate                                                     ║
║     • MPU6050 - accelerometru + giroscop                                     ║
║                                                                               ║
║  Avantaje față de simulare:                                                  ║
║  ✓ Zgomot real de senzor (diferit de gaussian ideal)                        ║
║  ✓ Delay-uri reale de comunicație                                           ║
║  ✓ Experiență practică cu constraintes hardware                              ║
║  ✓ Proiect demonstrabil pentru CV/portofoliu                                ║
║                                                                               ║
║  Vezi directorul arduino_sketches/ pentru cod gata de upload.                 ║
║                                                                               ║
╚═══════════════════════════════════════════════════════════════════════════════╝
```

---

*Acest material face parte din cursul "Algoritmi și Tehnici de Programare" (ATP)*
*Academia de Studii Economice din București - CSIE*
*Actualizat: Ianuarie 2026*
