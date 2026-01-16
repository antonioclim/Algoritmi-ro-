/**
 * =============================================================================
 * EXERCIȚIUL 1: SMART BUILDING MONITOR
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unui sistem de monitorizare pentru o clădire inteligentă
 *   cu 10 senzori de temperatură simulați, filtrare EMA, agregare în
 *   ferestre de 5 minute și detectare camere cu temperatură anormală.
 *
 * CERINȚE:
 *   1. Simularea a 10 senzori de temperatură cu zgomot gaussian
 *   2. Implementarea filtrului EMA cu α ajustabil (α = 0.2)
 *   3. Agregare în ferestre tumbling de 5 minute (mean, min, max)
 *   4. Detectarea camerelor neîncălzite (temp < 18°C) sau supraîncălzite (> 26°C)
 *   5. Raport periodic cu statistici per cameră
 *
 * EXEMPLU OUTPUT:
 *   [T=300s] Camera 3: temp medie 17.2°C - ALERTĂ FRIG!
 *   [T=600s] Raport: 8 camere OK, 1 rece, 1 caldă
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <float.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define NUM_SENSORS 10
#define SIMULATION_DURATION_SEC 3600    /* 1 oră de simulare */
#define READING_INTERVAL_SEC 10         /* O citire la 10 secunde */
#define WINDOW_SIZE_SEC 300             /* Fereastră de 5 minute */

#define TEMP_COLD_THRESHOLD 18.0
#define TEMP_HOT_THRESHOLD 26.0
#define EMA_ALPHA 0.2

/* Culori pentru output */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_CYAN    "\033[0;36m"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * TODO 1: Definește structura pentru filtrul EMA
 *
 * Hint: Ai nevoie de:
 *   - alpha (factor de netezire)
 *   - ema (valoarea curentă)
 *   - initialized (bool pentru prima valoare)
 */
typedef struct {
    /* YOUR CODE HERE */
    double alpha;
    double ema;
    bool initialized;
} EMAFilter;

/**
 * TODO 2: Definește structura pentru agregare în fereastră
 *
 * Hint: Stochează valorile într-un buffer circular
 */
typedef struct {
    /* YOUR CODE HERE */
    double *values;
    size_t capacity;
    size_t count;
    uint64_t window_start;
} TumblingWindow;

/**
 * Structura pentru un senzor de cameră
 */
typedef struct {
    int room_id;
    char room_name[32];
    double base_temp;           /* Temperatura de bază a camerei */
    double noise_stddev;        /* Zgomotul senzorului */
    EMAFilter filter;           /* Filtrul pentru netezire */
    TumblingWindow window;      /* Fereastra de agregare */
    size_t alert_count;         /* Numărul de alerte generate */
} RoomSensor;

/**
 * Rezultatul agregării
 */
typedef struct {
    double mean;
    double min;
    double max;
    size_t count;
} AggregateResult;

/* =============================================================================
 * FUNCȚII UTILITARE (FURNIZATE)
 * =============================================================================
 */

/**
 * Generează un număr aleator cu distribuție normală
 */
double random_gaussian(double mean, double stddev) {
    static bool has_spare = false;
    static double spare;
    
    if (has_spare) {
        has_spare = false;
        return spare * stddev + mean;
    }
    
    double u, v, s;
    do {
        u = (rand() / (double)RAND_MAX) * 2.0 - 1.0;
        v = (rand() / (double)RAND_MAX) * 2.0 - 1.0;
        s = u * u + v * v;
    } while (s >= 1.0 || s == 0.0);
    
    s = sqrt(-2.0 * log(s) / s);
    spare = v * s;
    has_spare = true;
    
    return mean + stddev * u * s;
}

/**
 * Generează o citire de temperatură pentru o cameră
 */
double generate_temperature(RoomSensor *sensor) {
    return sensor->base_temp + random_gaussian(0, sensor->noise_stddev);
}

/**
 * Formatează timpul în format MM:SS
 */
void format_time(uint64_t seconds, char *buffer, size_t size) {
    snprintf(buffer, size, "%02lu:%02lu", 
             (unsigned long)(seconds / 60), 
             (unsigned long)(seconds % 60));
}

/* =============================================================================
 * FUNCȚII DE IMPLEMENTAT
 * =============================================================================
 */

/**
 * TODO 3: Inițializează filtrul EMA
 *
 * @param filter Pointer la structura EMA
 * @param alpha Factor de netezire (0 < alpha < 1)
 */
void ema_init(EMAFilter *filter, double alpha) {
    /* YOUR CODE HERE */
    filter->alpha = alpha;
    filter->ema = 0.0;
    filter->initialized = false;
}

/**
 * TODO 4: Actualizează filtrul EMA cu o nouă valoare
 *
 * Formula: EMA_t = α * x_t + (1-α) * EMA_{t-1}
 *
 * @param filter Pointer la structura EMA
 * @param value Noua valoare
 * @return Valoarea EMA actualizată
 */
double ema_update(EMAFilter *filter, double value) {
    /* YOUR CODE HERE */
    if (!filter->initialized) {
        filter->ema = value;
        filter->initialized = true;
    } else {
        filter->ema = filter->alpha * value + (1.0 - filter->alpha) * filter->ema;
    }
    return filter->ema;
}

/**
 * TODO 5: Inițializează fereastra tumbling
 *
 * @param window Pointer la structura fereastră
 * @param capacity Numărul maxim de valori în fereastră
 */
void window_init(TumblingWindow *window, size_t capacity) {
    /* YOUR CODE HERE */
    window->values = (double*)malloc(capacity * sizeof(double));
    window->capacity = capacity;
    window->count = 0;
    window->window_start = 0;
}

/**
 * TODO 6: Adaugă o valoare în fereastră
 *
 * @param window Pointer la structura fereastră
 * @param value Valoarea de adăugat
 * @param timestamp Timestamp-ul curent
 * @param window_size Dimensiunea ferestrei în unități de timp
 * @param result Pointer pentru rezultatul agregării (dacă fereastra se închide)
 * @return true dacă fereastra s-a închis și result conține date valide
 */
bool window_add(TumblingWindow *window, double value, uint64_t timestamp,
                uint64_t window_size, AggregateResult *result) {
    /* YOUR CODE HERE */
    
    /* Pași:
     * 1. Dacă e prima valoare, setează window_start
     * 2. Verifică dacă fereastra s-a închis (timestamp >= window_start + window_size)
     * 3. Dacă da: calculează agregarea, resetează fereastra, returnează true
     * 4. Dacă nu: adaugă valoarea, returnează false
     */
    
    if (window->count == 0) {
        window->window_start = timestamp;
    }
    
    if (timestamp >= window->window_start + window_size) {
        /* Calculează agregarea */
        if (window->count > 0) {
            double sum = 0.0, min = DBL_MAX, max = -DBL_MAX;
            for (size_t i = 0; i < window->count; i++) {
                sum += window->values[i];
                if (window->values[i] < min) min = window->values[i];
                if (window->values[i] > max) max = window->values[i];
            }
            result->mean = sum / window->count;
            result->min = min;
            result->max = max;
            result->count = window->count;
        }
        
        /* Resetează fereastra */
        window->count = 0;
        window->window_start = timestamp;
        
        /* Adaugă valoarea curentă */
        if (window->count < window->capacity) {
            window->values[window->count++] = value;
        }
        
        return true;
    }
    
    /* Adaugă în fereastră */
    if (window->count < window->capacity) {
        window->values[window->count++] = value;
    }
    
    return false;
}

/**
 * TODO 7: Eliberează memoria ferestrei
 */
void window_free(TumblingWindow *window) {
    /* YOUR CODE HERE */
    free(window->values);
    window->values = NULL;
    window->count = 0;
}

/**
 * TODO 8: Verifică starea temperaturii și generează alertă dacă e cazul
 *
 * @param sensor Pointer la senzorul camerei
 * @param temp_mean Temperatura medie din ultima fereastră
 * @param timestamp Timestamp-ul curent
 */
void check_temperature_alert(RoomSensor *sensor, double temp_mean, uint64_t timestamp) {
    /* YOUR CODE HERE */
    
    /* Verifică dacă temperatura e prea scăzută sau prea ridicată
     * Afișează alertă colorată dacă e cazul
     * Incrementează alert_count pentru senzor
     */
    
    char time_str[16];
    format_time(timestamp, time_str, sizeof(time_str));
    
    if (temp_mean < TEMP_COLD_THRESHOLD) {
        printf("%s[%s] Camera %s: temp medie %.1f°C - ALERTĂ FRIG!%s\n",
               COLOR_BLUE, time_str, sensor->room_name, temp_mean, COLOR_RESET);
        sensor->alert_count++;
    } else if (temp_mean > TEMP_HOT_THRESHOLD) {
        printf("%s[%s] Camera %s: temp medie %.1f°C - ALERTĂ CALD!%s\n",
               COLOR_RED, time_str, sensor->room_name, temp_mean, COLOR_RESET);
        sensor->alert_count++;
    }
}

/**
 * TODO 9: Inițializează un senzor de cameră
 */
void sensor_init(RoomSensor *sensor, int room_id, const char *name, 
                 double base_temp, double noise) {
    /* YOUR CODE HERE */
    sensor->room_id = room_id;
    strncpy(sensor->room_name, name, sizeof(sensor->room_name) - 1);
    sensor->base_temp = base_temp;
    sensor->noise_stddev = noise;
    sensor->alert_count = 0;
    
    ema_init(&sensor->filter, EMA_ALPHA);
    window_init(&sensor->window, WINDOW_SIZE_SEC / READING_INTERVAL_SEC + 10);
}

/* =============================================================================
 * FUNCȚIE PRINCIPALĂ
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 1: SMART BUILDING MONITOR                                      ║\n");
    printf("║     Monitorizare 10 camere cu detecție anomalii temperatură                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Inițializare random */
    srand((unsigned int)time(NULL));
    
    /* Creează senzorii pentru 10 camere */
    RoomSensor sensors[NUM_SENSORS];
    const char *room_names[] = {
        "Living", "Dormitor1", "Dormitor2", "Bucătărie", "Baie1",
        "Baie2", "Birou", "Hol", "Cameră copii", "Mansardă"
    };
    
    /* Temperaturi de bază diferite pentru fiecare cameră */
    double base_temps[] = {22.0, 21.0, 21.0, 23.0, 24.0, 24.0, 22.0, 19.0, 21.5, 17.0};
    
    for (int i = 0; i < NUM_SENSORS; i++) {
        sensor_init(&sensors[i], i, room_names[i], base_temps[i], 0.5);
    }
    
    printf("Simulare: %d secunde, %d senzori, citire la %d secunde\n",
           SIMULATION_DURATION_SEC, NUM_SENSORS, READING_INTERVAL_SEC);
    printf("Praguri: FRIG < %.1f°C, CALD > %.1f°C\n\n",
           TEMP_COLD_THRESHOLD, TEMP_HOT_THRESHOLD);
    
    /* Bucla principală de simulare */
    int windows_completed = 0;
    
    for (uint64_t t = 0; t < SIMULATION_DURATION_SEC; t += READING_INTERVAL_SEC) {
        for (int i = 0; i < NUM_SENSORS; i++) {
            /* Generează citire */
            double raw_temp = generate_temperature(&sensors[i]);
            
            /* Filtrează cu EMA */
            double filtered_temp = ema_update(&sensors[i].filter, raw_temp);
            
            /* Adaugă în fereastră și verifică dacă s-a închis */
            AggregateResult agg;
            if (window_add(&sensors[i].window, filtered_temp, t, 
                          WINDOW_SIZE_SEC, &agg)) {
                /* Fereastră completă - verifică alertă */
                check_temperature_alert(&sensors[i], agg.mean, t);
                
                if (i == 0) windows_completed++;
            }
        }
        
        /* Raport periodic la fiecare 10 minute */
        if (t > 0 && t % 600 == 0) {
            char time_str[16];
            format_time(t, time_str, sizeof(time_str));
            
            int rooms_ok = 0, rooms_cold = 0, rooms_hot = 0;
            for (int i = 0; i < NUM_SENSORS; i++) {
                double current_temp = sensors[i].filter.ema;
                if (current_temp < TEMP_COLD_THRESHOLD) rooms_cold++;
                else if (current_temp > TEMP_HOT_THRESHOLD) rooms_hot++;
                else rooms_ok++;
            }
            
            printf("\n%s[%s] === RAPORT PERIODIC ===%s\n", COLOR_GREEN, time_str, COLOR_RESET);
            printf("  Camere OK: %d | Reci: %d | Calde: %d\n", rooms_ok, rooms_cold, rooms_hot);
            printf("\n");
        }
    }
    
    /* Raport final */
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    RAPORT FINAL                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("%-15s %-12s %-12s %-10s\n", "Cameră", "Temp curentă", "Temp bază", "Alerte");
    printf("%-15s %-12s %-12s %-10s\n", "------", "------------", "---------", "------");
    
    size_t total_alerts = 0;
    for (int i = 0; i < NUM_SENSORS; i++) {
        printf("%-15s %-12.1f %-12.1f %-10zu\n",
               sensors[i].room_name,
               sensors[i].filter.ema,
               sensors[i].base_temp,
               sensors[i].alert_count);
        total_alerts += sensors[i].alert_count;
    }
    
    printf("\nTotal alerte generate: %zu\n", total_alerts);
    printf("Ferestre procesate per senzor: %d\n", windows_completed);
    
    /* Curățare */
    for (int i = 0; i < NUM_SENSORS; i++) {
        window_free(&sensors[i].window);
    }
    
    printf("\n%s✓ Simulare completă!%s\n\n", COLOR_GREEN, COLOR_RESET);
    
    return 0;
}
