/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 2: Parser de Configurație INI
 * =============================================================================
 *
 * ATENȚIE: Acest fișier conține soluția completă!
 * Destinat DOAR instructorilor pentru verificare și evaluare.
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c
 *
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_SECTIUNI 20
#define MAX_PERECHI_TOTAL 200
#define MAX_LINE 512
#define MAX_KEY 64
#define MAX_VALUE 256
#define MAX_SECTION_NAME 64

typedef enum {
    TYPE_STRING,
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_BOOLEAN,
    TYPE_UNKNOWN
} ValueType;

/* =============================================================================
 * TIPURI DE DATE
 * =============================================================================
 */

/* SOLUȚIE TODO 1: Structura ConfigEntry completă */
typedef struct {
    char section[MAX_SECTION_NAME];
    char key[MAX_KEY];
    char value[MAX_VALUE];
    ValueType type;
} ConfigEntry;

typedef struct {
    ConfigEntry entries[MAX_PERECHI_TOTAL];
    int count;
    char sections[MAX_SECTIUNI][MAX_SECTION_NAME];
    int section_count;
} Config;

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

/* SOLUȚIE TODO 2: Funcția trim */
char *trim(char *str) {
    if (str == NULL) return NULL;
    
    /* Eliminăm spațiile de la început */
    while (isspace((unsigned char)*str)) {
        str++;
    }
    
    /* Dacă string-ul e gol, returnăm */
    if (*str == '\0') {
        return str;
    }
    
    /* Eliminăm spațiile de la final */
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    
    /* Punem terminatorul */
    end[1] = '\0';
    
    return str;
}

/* SOLUȚIE TODO 3: Detectarea tipului valorii */
ValueType detecteaza_tip(const char *value) {
    if (value == NULL || *value == '\0') {
        return TYPE_STRING;
    }
    
    /* Verificare boolean */
    if (strcasecmp(value, "true") == 0 || strcasecmp(value, "false") == 0 ||
        strcasecmp(value, "yes") == 0 || strcasecmp(value, "no") == 0 ||
        strcasecmp(value, "on") == 0 || strcasecmp(value, "off") == 0) {
        return TYPE_BOOLEAN;
    }
    
    /* Verificare dacă e "1" sau "0" (pot fi și boolean) */
    if (strcmp(value, "1") == 0 || strcmp(value, "0") == 0) {
        return TYPE_BOOLEAN;
    }
    
    /* Verificare întreg */
    char *endptr;
    long l = strtol(value, &endptr, 10);
    (void)l;  /* Evităm warning pentru variabilă neutilizată */
    if (*endptr == '\0') {
        return TYPE_INTEGER;
    }
    
    /* Verificare float */
    double d = strtod(value, &endptr);
    (void)d;
    if (*endptr == '\0') {
        return TYPE_FLOAT;
    }
    
    return TYPE_STRING;
}

/* SOLUȚIE TODO 4: Verificare comentariu */
int este_comentariu(const char *line) {
    /* Sărim peste spațiile de la început */
    while (isspace((unsigned char)*line)) {
        line++;
    }
    
    /* Verificăm dacă e linie goală sau comentariu */
    return (*line == '\0' || *line == '#' || *line == ';');
}

/* SOLUȚIE TODO 5: Verificare secțiune */
int este_sectiune(const char *line, char *section_name, int max_len) {
    /* Sărim peste spațiile de la început */
    while (isspace((unsigned char)*line)) {
        line++;
    }
    
    /* Verificăm dacă începe cu '[' */
    if (*line != '[') {
        return 0;
    }
    
    line++;  /* Trecem peste '[' */
    
    /* Găsim ']' */
    const char *end = strchr(line, ']');
    if (end == NULL) {
        return 0;  /* Nu s-a găsit ']' */
    }
    
    /* Calculăm lungimea numelui secțiunii */
    int len = end - line;
    if (len <= 0 || len >= max_len) {
        return 0;
    }
    
    /* Copiem numele secțiunii */
    strncpy(section_name, line, len);
    section_name[len] = '\0';
    
    /* Verificăm că după ']' sunt doar spații */
    end++;
    while (*end != '\0') {
        if (!isspace((unsigned char)*end)) {
            return 0;  /* Caractere invalide după ']' */
        }
        end++;
    }
    
    /* Trim pe numele secțiunii */
    char *trimmed = trim(section_name);
    if (trimmed != section_name) {
        memmove(section_name, trimmed, strlen(trimmed) + 1);
    }
    
    return 1;
}

/* SOLUȚIE TODO 6: Parsarea unei perechi cheie=valoare */
int parseaza_pereche(const char *line, char *key, char *value, int key_len, int value_len) {
    /* Găsim '=' */
    const char *eq = strchr(line, '=');
    if (eq == NULL) {
        return 0;
    }
    
    /* Calculăm lungimea cheii */
    int klen = eq - line;
    if (klen <= 0 || klen >= key_len) {
        return 0;
    }
    
    /* Copiem cheia */
    strncpy(key, line, klen);
    key[klen] = '\0';
    
    /* Copiem valoarea (tot ce e după '=') */
    const char *val_start = eq + 1;
    strncpy(value, val_start, value_len - 1);
    value[value_len - 1] = '\0';
    
    /* Trim pe ambele */
    char *trimmed_key = trim(key);
    if (trimmed_key != key) {
        memmove(key, trimmed_key, strlen(trimmed_key) + 1);
    }
    
    char *trimmed_val = trim(value);
    if (trimmed_val != value) {
        memmove(value, trimmed_val, strlen(trimmed_val) + 1);
    }
    
    /* Verificăm că cheia nu e goală */
    return (strlen(key) > 0);
}

/* Verifică dacă o secțiune există deja */
int sectiune_exista(const Config *config, const char *section) {
    for (int i = 0; i < config->section_count; i++) {
        if (strcmp(config->sections[i], section) == 0) {
            return 1;
        }
    }
    return 0;
}

/* SOLUȚIE TODO 7: Parsarea fișierului INI */
int parseaza_fisier_ini(const char *filename, Config *config) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Eroare la deschiderea '%s': %s\n", filename, strerror(errno));
        return -1;
    }
    
    /* Inițializare */
    config->count = 0;
    config->section_count = 0;
    
    char current_section[MAX_SECTION_NAME] = "";
    char line[MAX_LINE];
    int line_num = 0;
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        line_num++;
        
        /* Elimină newline */
        line[strcspn(line, "\r\n")] = '\0';
        
        /* Ignoră comentarii și linii goale */
        if (este_comentariu(line)) {
            continue;
        }
        
        /* Verifică dacă e secțiune */
        char section_name[MAX_SECTION_NAME];
        if (este_sectiune(line, section_name, sizeof(section_name))) {
            strncpy(current_section, section_name, MAX_SECTION_NAME - 1);
            current_section[MAX_SECTION_NAME - 1] = '\0';
            
            /* Adaugă secțiunea dacă nu există */
            if (!sectiune_exista(config, current_section) && 
                config->section_count < MAX_SECTIUNI) {
                strncpy(config->sections[config->section_count], 
                        current_section, MAX_SECTION_NAME - 1);
                config->sections[config->section_count][MAX_SECTION_NAME - 1] = '\0';
                config->section_count++;
            }
            continue;
        }
        
        /* Încearcă să parseze ca pereche cheie=valoare */
        char key[MAX_KEY], value[MAX_VALUE];
        if (parseaza_pereche(line, key, value, sizeof(key), sizeof(value))) {
            if (config->count < MAX_PERECHI_TOTAL) {
                ConfigEntry *entry = &config->entries[config->count];
                strncpy(entry->section, current_section, MAX_SECTION_NAME - 1);
                entry->section[MAX_SECTION_NAME - 1] = '\0';
                strncpy(entry->key, key, MAX_KEY - 1);
                entry->key[MAX_KEY - 1] = '\0';
                strncpy(entry->value, value, MAX_VALUE - 1);
                entry->value[MAX_VALUE - 1] = '\0';
                entry->type = detecteaza_tip(value);
                config->count++;
            }
        }
    }
    
    if (ferror(fp)) {
        fprintf(stderr, "Eroare la citire: %s\n", strerror(errno));
        fclose(fp);
        return -1;
    }
    
    fclose(fp);
    return 0;
}

/* SOLUȚIE TODO 8: Căutarea unei valori */
const char *cauta_valoare(const Config *config, const char *section, const char *key) {
    for (int i = 0; i < config->count; i++) {
        /* Dacă section e NULL sau gol, potrivim orice secțiune */
        int section_match = (section == NULL || *section == '\0' ||
                           strcmp(config->entries[i].section, section) == 0);
        
        if (section_match && strcmp(config->entries[i].key, key) == 0) {
            return config->entries[i].value;
        }
    }
    return NULL;
}

/* SOLUȚIE TODO 9: Obținere valoare ca întreg */
int get_int(const Config *config, const char *section, const char *key, int default_val) {
    const char *value = cauta_valoare(config, section, key);
    if (value == NULL) {
        return default_val;
    }
    
    char *endptr;
    long result = strtol(value, &endptr, 10);
    if (*endptr != '\0') {
        return default_val;
    }
    
    return (int)result;
}

/* SOLUȚIE TODO 10: Obținere valoare ca double */
double get_double(const Config *config, const char *section, const char *key, double default_val) {
    const char *value = cauta_valoare(config, section, key);
    if (value == NULL) {
        return default_val;
    }
    
    char *endptr;
    double result = strtod(value, &endptr);
    if (*endptr != '\0') {
        return default_val;
    }
    
    return result;
}

/* SOLUȚIE TODO 11: Obținere valoare ca boolean */
int get_bool(const Config *config, const char *section, const char *key, int default_val) {
    const char *value = cauta_valoare(config, section, key);
    if (value == NULL) {
        return default_val;
    }
    
    /* Valori true */
    if (strcasecmp(value, "true") == 0 || strcasecmp(value, "yes") == 0 ||
        strcasecmp(value, "on") == 0 || strcmp(value, "1") == 0) {
        return 1;
    }
    
    /* Valori false */
    if (strcasecmp(value, "false") == 0 || strcasecmp(value, "no") == 0 ||
        strcasecmp(value, "off") == 0 || strcmp(value, "0") == 0) {
        return 0;
    }
    
    return default_val;
}

/* Funcția de afișare (furnizată) */
void afiseaza_config(const Config *config) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║               CONFIGURAȚIE PARSATĂ                            ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    
    const char *current_section = "";
    
    for (int i = 0; i < config->count; i++) {
        if (strcmp(config->entries[i].section, current_section) != 0) {
            current_section = config->entries[i].section;
            printf("╠───────────────────────────────────────────────────────────────╣\n");
            printf("║  [%-58s] ║\n", current_section);
            printf("╠───────────────────────────────────────────────────────────────╣\n");
        }
        
        const char *type_str;
        switch (config->entries[i].type) {
            case TYPE_INTEGER: type_str = "INT"; break;
            case TYPE_FLOAT:   type_str = "FLOAT"; break;
            case TYPE_BOOLEAN: type_str = "BOOL"; break;
            default:           type_str = "STR"; break;
        }
        
        printf("║  %-20s = %-25s [%5s] ║\n",
               config->entries[i].key,
               config->entries[i].value,
               type_str);
    }
    
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\nTotal: %d intrări în %d secțiuni\n", config->count, config->section_count);
}

/* SOLUȚIE TODO 12: Export în format JSON */
int exporta_json(const Config *config, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Eroare la crearea '%s': %s\n", filename, strerror(errno));
        return -1;
    }
    
    fprintf(fp, "{\n");
    
    const char *current_section = NULL;
    int first_section = 1;
    int first_entry = 1;
    
    for (int i = 0; i < config->count; i++) {
        /* Verificăm dacă s-a schimbat secțiunea */
        if (current_section == NULL || 
            strcmp(config->entries[i].section, current_section) != 0) {
            
            /* Închidem secțiunea anterioară */
            if (current_section != NULL) {
                fprintf(fp, "\n  }");
            }
            
            /* Virgulă între secțiuni */
            if (!first_section) {
                fprintf(fp, ",");
            }
            first_section = 0;
            
            /* Deschidem noua secțiune */
            current_section = config->entries[i].section;
            fprintf(fp, "\n  \"%s\": {", current_section);
            first_entry = 1;
        }
        
        /* Virgulă între intrări */
        if (!first_entry) {
            fprintf(fp, ",");
        }
        first_entry = 0;
        
        /* Scriem intrarea */
        fprintf(fp, "\n    \"%s\": ", config->entries[i].key);
        
        switch (config->entries[i].type) {
            case TYPE_INTEGER:
            case TYPE_FLOAT:
                fprintf(fp, "%s", config->entries[i].value);
                break;
            case TYPE_BOOLEAN:
                if (strcasecmp(config->entries[i].value, "true") == 0 ||
                    strcasecmp(config->entries[i].value, "yes") == 0 ||
                    strcasecmp(config->entries[i].value, "on") == 0 ||
                    strcmp(config->entries[i].value, "1") == 0) {
                    fprintf(fp, "true");
                } else {
                    fprintf(fp, "false");
                }
                break;
            default:
                fprintf(fp, "\"%s\"", config->entries[i].value);
                break;
        }
    }
    
    /* Închidem ultima secțiune */
    if (current_section != NULL) {
        fprintf(fp, "\n  }");
    }
    
    fprintf(fp, "\n}\n");
    
    fclose(fp);
    return 0;
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    const char *input_file = (argc > 1) ? argv[1] : "../data/config_sample.ini";
    const char *output_file = (argc > 2) ? argv[2] : "config_output.json";
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║    SOLUȚIE EXERCIȚIUL 2: Parser de Configurație INI           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Config config;
    
    printf("📂 Parsare fișier '%s'...\n", input_file);
    
    if (parseaza_fisier_ini(input_file, &config) != 0) {
        fprintf(stderr, "❌ Eroare la parsarea fișierului!\n");
        return EXIT_FAILURE;
    }
    
    printf("✓ Fișier parsat cu succes.\n");
    
    afiseaza_config(&config);
    
    printf("\n📋 Demonstrare funcții de căutare:\n");
    printf("   ─────────────────────────────────────────────────────────\n");
    
    const char *host = cauta_valoare(&config, "Database", "host");
    int port = get_int(&config, "Database", "port", 0);
    int enabled = get_bool(&config, "Database", "enabled", 0);
    double timeout = get_double(&config, "Server", "timeout", 0.0);
    
    printf("   Database.host = %s\n", host ? host : "(not found)");
    printf("   Database.port = %d\n", port);
    printf("   Database.enabled = %s\n", enabled ? "true" : "false");
    printf("   Server.timeout = %.1f\n", timeout);
    
    printf("\n📝 Export în format JSON '%s'...\n", output_file);
    
    if (exporta_json(&config, output_file) == 0) {
        printf("✓ Export JSON completat!\n");
        
        printf("\n   Conținutul fișierului JSON:\n");
        printf("   ─────────────────────────────────────────────────────────\n");
        FILE *fp = fopen(output_file, "r");
        if (fp) {
            char line[256];
            while (fgets(line, sizeof(line), fp)) {
                printf("   %s", line);
            }
            fclose(fp);
        }
    } else {
        fprintf(stderr, "❌ Eroare la export JSON!\n");
        return EXIT_FAILURE;
    }
    
    printf("\n✅ Procesare completă!\n");
    
    return EXIT_SUCCESS;
}
