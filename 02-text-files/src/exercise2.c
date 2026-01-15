/**
 * =============================================================================
 * EXERCIȚIUL 2: Parser de Configurație INI
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unui parser robust pentru fișiere de configurație în format INI,
 *   cu suport pentru secțiuni, perechi cheie-valoare și comentarii.
 *
 * CERINȚE:
 *   1. Citirea și parsarea fișierelor în format INI
 *   2. Suport pentru secțiuni: [SectionName]
 *   3. Suport pentru perechi: key = value
 *   4. Ignorarea comentariilor (linii care încep cu # sau ;)
 *   5. Ignorarea liniilor goale
 *   6. Eliminarea spațiilor de la începutul/sfârșitul valorilor
 *   7. Validarea tipurilor de date (întreg, real, boolean, string)
 *   8. Căutarea valorilor după secțiune și cheie
 *
 * EXEMPLU INPUT (config.ini):
 *   # Aceasta este o configurație de test
 *   [Database]
 *   host = localhost
 *   port = 5432
 *   enabled = true
 *
 *   [Server]
 *   timeout = 30.5
 *   name = MyServer
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_SECTIUNI 20
#define MAX_PERECHI_PER_SECTIUNE 50
#define MAX_PERECHI_TOTAL 200
#define MAX_LINE 512
#define MAX_KEY 64
#define MAX_VALUE 256
#define MAX_SECTION_NAME 64

/* Tipuri de valori */
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

/**
 * TODO 1: Completează structura ConfigEntry pentru o pereche cheie-valoare
 *
 * Structura trebuie să conțină:
 *   - section: numele secțiunii din care face parte (string)
 *   - key: numele cheii (string)
 *   - value: valoarea ca string (string)
 *   - type: tipul detectat al valorii (ValueType)
 *
 * Hint: Folosește dimensiunile MAX_SECTION_NAME, MAX_KEY, MAX_VALUE
 */
typedef struct {
    /* YOUR CODE HERE */
    char section[MAX_SECTION_NAME];
    /* Adaugă restul câmpurilor */
    
} ConfigEntry;

/**
 * Structura principală pentru configurație (furnizată complet)
 */
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

/**
 * TODO 2: Implementează funcția trim care elimină spațiile
 *
 * @param str String-ul de procesat (va fi modificat in-place)
 * @return Pointer la începutul string-ului fără spații
 *
 * Funcția trebuie să elimine:
 *   - Spațiile de la ÎNCEPUTUL string-ului
 *   - Spațiile de la SFÂRȘITUL string-ului
 *
 * Caractere considerate spații: ' ', '\t', '\n', '\r'
 *
 * Hint: Folosește isspace() pentru a verifica dacă un caracter e spațiu
 * Hint: Pentru spațiile de la final, găsește ultimul caracter non-spațiu
 *       și pune '\0' după el
 */
char *trim(char *str) {
    /* YOUR CODE HERE */
    
    return str;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 3: Implementează funcția care detectează tipul unei valori
 *
 * @param value String-ul valorii
 * @return Tipul detectat (ValueType)
 *
 * Reguli de detectare:
 *   - TYPE_BOOLEAN: "true", "false", "yes", "no", "1", "0" (case insensitive)
 *   - TYPE_INTEGER: doar cifre, opțional precedat de '-' sau '+'
 *   - TYPE_FLOAT: cifre cu punct zecimal sau notație științifică
 *   - TYPE_STRING: orice altceva
 *
 * Hint: Folosește strtol() și strtod() pentru validare
 * Hint: Folosește strcasecmp() pentru comparare case-insensitive
 */
ValueType detecteaza_tip(const char *value) {
    /* YOUR CODE HERE */
    
    return TYPE_STRING;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 4: Implementează funcția care verifică dacă o linie e comentariu
 *
 * @param line Linia de verificat
 * @return 1 dacă e comentariu sau linie goală, 0 altfel
 *
 * Considerăm comentariu:
 *   - Linii care încep cu '#' (după eliminarea spațiilor inițiale)
 *   - Linii care încep cu ';' (după eliminarea spațiilor inițiale)
 *   - Linii goale (doar spații)
 *
 * Hint: Mai întâi sari peste spațiile de la început
 */
int este_comentariu(const char *line) {
    /* YOUR CODE HERE */
    
    return 0;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 5: Implementează funcția care verifică dacă o linie e secțiune
 *
 * @param line Linia de verificat
 * @param section_name Buffer unde se salvează numele secțiunii (dacă e secțiune)
 * @param max_len Dimensiunea maximă a buffer-ului
 * @return 1 dacă linia e o secțiune validă, 0 altfel
 *
 * Format secțiune: [NueleSecțiunii]
 *
 * Algoritm:
 *   1. Sari peste spațiile de la început
 *   2. Verifică dacă primul caracter e '['
 *   3. Găsește caracterul ']'
 *   4. Extrage textul dintre '[' și ']'
 *   5. Verifică că nu sunt caractere invalide după ']' (doar spații permise)
 *
 * Hint: Folosește strchr() pentru a găsi ']'
 */
int este_sectiune(const char *line, char *section_name, int max_len) {
    /* YOUR CODE HERE */
    
    return 0;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 6: Implementează funcția care parsează o pereche cheie=valoare
 *
 * @param line Linia de parsat
 * @param key Buffer unde se salvează cheia
 * @param value Buffer unde se salvează valoarea
 * @param key_len Dimensiunea maximă pentru key
 * @param value_len Dimensiunea maximă pentru value
 * @return 1 dacă parsarea a reușit, 0 altfel
 *
 * Format: key = value
 * Sau: key=value (fără spații)
 *
 * Algoritm:
 *   1. Găsește poziția caracterului '='
 *   2. Extrage partea din stânga (key)
 *   3. Extrage partea din dreapta (value)
 *   4. Aplică trim() pe ambele
 *   5. Verifică că key nu e gol
 *
 * Hint: Folosește strchr() pentru a găsi '='
 * Hint: Folosește strncpy() pentru copiere sigură
 */
int parseaza_pereche(const char *line, char *key, char *value, int key_len, int value_len) {
    /* YOUR CODE HERE */
    
    return 0;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 7: Implementează funcția principală de parsare a fișierului
 *
 * @param filename Numele fișierului INI
 * @param config Pointer la structura Config unde se salvează datele
 * @return 0 pentru succes, -1 pentru eroare
 *
 * Algoritm:
 *   1. Deschide fișierul
 *   2. Inițializează config->count și config->section_count la 0
 *   3. Inițializează secțiunea curentă la "" (gol)
 *   4. Pentru fiecare linie:
 *      a. Ignoră dacă e comentariu
 *      b. Dacă e secțiune, actualizează secțiunea curentă
 *      c. Dacă e pereche, adaugă în entries cu secțiunea curentă
 *   5. Închide fișierul
 *
 * Hint: Menține o variabilă pentru secțiunea curentă
 * Hint: Pentru fiecare secțiune nouă, verifică dacă există deja în array
 */
int parseaza_fisier_ini(const char *filename, Config *config) {
    /* YOUR CODE HERE */
    
    return -1;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 8: Implementează funcția de căutare a unei valori
 *
 * @param config Pointer la configurație
 * @param section Numele secțiunii (poate fi NULL pentru a căuta în toate)
 * @param key Numele cheii
 * @return Pointer la valoare sau NULL dacă nu s-a găsit
 *
 * Hint: Parcurge entries și compară section și key
 * Hint: Dacă section e NULL sau gol, potrivește orice secțiune
 */
const char *cauta_valoare(const Config *config, const char *section, const char *key) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 9: Implementează funcția care returnează o valoare ca întreg
 *
 * @param config Pointer la configurație
 * @param section Numele secțiunii
 * @param key Numele cheii
 * @param default_val Valoarea implicită dacă nu se găsește
 * @return Valoarea ca int sau default_val
 *
 * Hint: Folosește cauta_valoare() și atoi() sau strtol()
 */
int get_int(const Config *config, const char *section, const char *key, int default_val) {
    /* YOUR CODE HERE */
    
    return default_val;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 10: Implementează funcția care returnează o valoare ca double
 *
 * @param config Pointer la configurație
 * @param section Numele secțiunii
 * @param key Numele cheii
 * @param default_val Valoarea implicită dacă nu se găsește
 * @return Valoarea ca double sau default_val
 *
 * Hint: Folosește cauta_valoare() și atof() sau strtod()
 */
double get_double(const Config *config, const char *section, const char *key, double default_val) {
    /* YOUR CODE HERE */
    
    return default_val;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 11: Implementează funcția care returnează o valoare ca boolean
 *
 * @param config Pointer la configurație
 * @param section Numele secțiunii
 * @param key Numele cheii
 * @param default_val Valoarea implicită dacă nu se găsește
 * @return 1 pentru true, 0 pentru false, sau default_val
 *
 * Valori considerate true: "true", "yes", "1", "on"
 * Valori considerate false: "false", "no", "0", "off"
 *
 * Hint: Folosește strcasecmp() pentru comparare case-insensitive
 */
int get_bool(const Config *config, const char *section, const char *key, int default_val) {
    /* YOUR CODE HERE */
    
    return default_val;  /* Înlocuiește cu implementarea corectă */
}

/**
 * Funcție de afișare a configurației (furnizată complet)
 */
void afiseaza_config(const Config *config) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║               CONFIGURAȚIE PARSATĂ                            ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    
    const char *current_section = "";
    
    for (int i = 0; i < config->count; i++) {
        /* Afișează header secțiune dacă s-a schimbat */
        if (strcmp(config->entries[i].section, current_section) != 0) {
            current_section = config->entries[i].section;
            printf("╠───────────────────────────────────────────────────────────────╣\n");
            printf("║  [%-58s] ║\n", current_section);
            printf("╠───────────────────────────────────────────────────────────────╣\n");
        }
        
        /* Determină tipul ca string */
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

/**
 * TODO 12: Implementează funcția de export în format JSON simplificat
 *
 * @param config Pointer la configurație
 * @param filename Numele fișierului JSON de output
 * @return 0 pentru succes, -1 pentru eroare
 *
 * Format JSON:
 * {
 *   "SectionName": {
 *     "key1": "value1",
 *     "key2": 123,
 *     "key3": true
 *   },
 *   ...
 * }
 *
 * Hint: Valorile INTEGER și FLOAT se scriu fără ghilimele
 * Hint: Valorile BOOLEAN se scriu ca true/false fără ghilimele
 * Hint: Valorile STRING se scriu cu ghilimele
 */
int exporta_json(const Config *config, const char *filename) {
    /* YOUR CODE HERE */
    
    return -1;  /* Înlocuiește cu implementarea corectă */
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    const char *input_file = (argc > 1) ? argv[1] : "../data/config_sample.ini";
    const char *output_file = (argc > 2) ? argv[2] : "config_output.json";
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║        EXERCIȚIUL 2: Parser de Configurație INI               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Config config;
    
    /* Pasul 1: Parsare fișier INI */
    printf("📂 Parsare fișier '%s'...\n", input_file);
    
    if (parseaza_fisier_ini(input_file, &config) != 0) {
        fprintf(stderr, "❌ Eroare la parsarea fișierului!\n");
        return EXIT_FAILURE;
    }
    
    printf("✓ Fișier parsat cu succes.\n");
    
    /* Pasul 2: Afișare configurație */
    afiseaza_config(&config);
    
    /* Pasul 3: Demonstrare căutare */
    printf("\n📋 Demonstrare funcții de căutare:\n");
    printf("   ─────────────────────────────────────────────────────────\n");
    
    /* Exemple de căutare - adaptează la fișierul tău de test */
    const char *host = cauta_valoare(&config, "Database", "host");
    int port = get_int(&config, "Database", "port", 0);
    int enabled = get_bool(&config, "Database", "enabled", 0);
    double timeout = get_double(&config, "Server", "timeout", 0.0);
    
    printf("   Database.host = %s\n", host ? host : "(not found)");
    printf("   Database.port = %d\n", port);
    printf("   Database.enabled = %s\n", enabled ? "true" : "false");
    printf("   Server.timeout = %.1f\n", timeout);
    
    /* Pasul 4: Export JSON */
    printf("\n📝 Export în format JSON '%s'...\n", output_file);
    
    if (exporta_json(&config, output_file) == 0) {
        printf("✓ Export JSON completat!\n");
        
        /* Afișare conținut JSON */
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
        printf("⚠️  Export JSON nu a fost implementat sau a eșuat.\n");
    }
    
    printf("\n✅ Procesare completă!\n");
    
    return EXIT_SUCCESS;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Adaugă suport pentru valori pe mai multe linii (continuare cu \)
 *
 * 2. Implementează interpolarea variabilelor: ${Section.key}
 *
 * 3. Adaugă funcție de validare care verifică dacă toate cheile obligatorii
 *    sunt prezente într-o secțiune
 *
 * 4. Implementează citirea din mai multe fișiere cu override (include)
 *
 * 5. Adaugă suport pentru array-uri: key[] = value1, value2, value3
 *
 * =============================================================================
 */
