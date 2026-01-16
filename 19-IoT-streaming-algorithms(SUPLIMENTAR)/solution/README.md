# 📁 Soluții - Săptămâna 19

## Conținut

| Fișier | Descriere |
|--------|-----------|
| `exercise1_sol.c` | Smart Building Temperature Monitor - soluție completă |
| `exercise2_sol.c` | Multi-Method Anomaly Detector - soluție completă |

---

## Compilare

```bash
# Exercise 1 - Temperature Monitor
gcc -Wall -Wextra -std=c11 -O2 -o exercise1_sol exercise1_sol.c -lm

# Exercise 2 - Anomaly Detector  
gcc -Wall -Wextra -std=c11 -O2 -o exercise2_sol exercise2_sol.c -lm
```

---

## Rulare

### Exercise 1: Temperature Monitor

```bash
# Mod demo cu date generate
./exercise1_sol

# Cu fișier CSV propriu
./exercise1_sol ../data/sensor_temperature.csv
```

**Format input CSV:**
```csv
timestamp,sensor_id,temperature
1704067200000,room_101,22.5
1704067201000,room_101,22.6
...
```

### Exercise 2: Anomaly Detector

```bash
# Mod demo
./exercise2_sol

# Cu fișier CSV
./exercise2_sol data.csv
```

**Format input CSV:**
```csv
timestamp,value
1704067200000,25.3
1704067201000,25.1
...
```

---

## Output Exemplu

### Exercise 1

```
[WINDOW] room_101: avg=22.15°C, min=21.80, max=22.50, σ=0.180, n=300
[WARNING] room_101: raw=26.50°C, filtered=26.20°C, zscore=1.85
[CRITICAL] server_room: raw=31.20°C, filtered=30.80°C, zscore=2.10
...

╔══════════════════════════════════════════════════════════════╗
║           RAPORT MONITORIZARE TEMPERATURĂ                    ║
╠══════════════════════════════════════════════════════════════╣
...
```

### Exercise 2

```
[ANOMALY] ts=1704067500000 value=38.50 severity=3 types=ZSCORE IQR THRESHOLD
          zscore=3.25 iqr_dist=5.20 spike_delta=12.30
...

╔══════════════════════════════════════════════════════════════╗
║            RAPORT DETECTOR ANOMALII                         ║
╠══════════════════════════════════════════════════════════════╣
║ Total samples procesate: 2000                               ║
║ Anomalii detectate:      47       (2.35%)                   ║
...
```

---

## Algoritmi Implementați

### Exercise 1
- **EMA Filter** - Exponential Moving Average pentru netezire
- **Tumbling Window** - Agregare pe ferestre de 5 minute
- **Threshold Checking** - Verificare praguri temperatură
- **Welford Statistics** - Medie și deviație standard incrementale

### Exercise 2
- **Z-Score Detection** - Detecție bazată pe deviații standard
- **IQR Detection** - Interquartile Range pentru outlieri
- **Threshold Detection** - Praguri absolute
- **Spike Detection** - Schimbări bruște
- **Rate Limiting** - Sliding window pentru prevenire alert fatigue

---

## Complexitate

| Algoritm | Timp | Spațiu |
|----------|------|--------|
| EMA | O(1) | O(1) |
| Z-Score (Welford) | O(1) | O(1) |
| Tumbling Window | O(1) | O(1) |
| IQR | O(n log n) | O(n) |
| Rate Limiter | O(k) | O(k) |

---

## Verificare Memorie

```bash
valgrind --leak-check=full --show-leak-kinds=all ./exercise1_sol
valgrind --leak-check=full --show-leak-kinds=all ./exercise2_sol
```

Ambele soluții ar trebui să fie "memory clean" (no leaks).

---

## Note pentru Studenți

1. **Studiați ordinea operațiilor** - Observați cum fiecare citire trece prin:
   - Filtrare (EMA)
   - Detecție anomalii
   - Agregare window
   - Verificare threshold

2. **Welford vs calcul naiv** - Comparați stabilitatea numerică:
   ```c
   // Naiv (instabil pentru valori mari)
   variance = sum_of_squares/n - mean*mean;
   
   // Welford (stabil)
   variance = M2 / n;
   ```

3. **Rate limiting** - Crucial în producție pentru a preveni:
   - Supraîncărcarea sistemelor de alertare
   - Alert fatigue la operatori
   - Costuri excesive (SMS, email, etc.)

4. **Consensul detectorilor** - În exercise2, observați cum multiple metode
   de detecție oferă mai multă încredere decât una singură.

---

*ATP Week 19 - IoT Stream Processing - ASE București CSIE*
