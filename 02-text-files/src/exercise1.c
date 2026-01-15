/**
 * =============================================================================
 * EXERCIȚIUL 1: Analizator de Note Studenți
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unui sistem complet de procesare a datelor studențești
 *   dintr-un fișier CSV, cu calculare statistici și generare raport.
 *
 * CERINȚE:
 *   1. Citirea datelor din fișierul CSV "studgrades.csv"
 *   2. Parsarea fiecărei linii pentru a extrage: ID, Nume, Prenume, An, Grupa, 10 note
 *   3. Calcularea mediei pentru fiecare student (ignorând notele de 0)
 *   4. Sortarea studenților după medie (descrescător)
 *   5. Generarea unui raport în fișierul "report.txt"
 *   6. Calcularea și afișarea statisticilor generale
 *
 * EXEMPLU INPUT (studgrades.csv):
 *   1122334 Antonio Clim 2022 1050 5 6 7 8 9 10 7 6 6 5
 *   222222 Badass Vader 2022 1051 9 10 10 10 10 9 6 10 10 9
 *
 * EXPECTED OUTPUT (report.txt):
 *   === RAPORT NOTE STUDENȚI ===
 *   1. Badass Vader (ID: 222222) - Media: 9.30 [PROMOVAT]
 *   2. Antonio Clim (ID: 1122334) - Media: 6.90 [PROMOVAT]
 *   ...
 *   Statistici: Total: 2, Promovați: 2, Media generală: 8.10
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_STUDENTI 100
#define MAX_LINE 512
#define MAX_NUME 50
#define NUM_NOTE 10
#define NOTA_PROMOVARE 5.0

/* =============================================================================
 * TIPURI DE DATE
 * =============================================================================
 */

/**
 * TODO 1: Completează structura Student cu câmpurile necesare
 *
 * Structura trebuie să conțină:
 *   - id: identificator numeric (int)
 *   - nume: string de maxim MAX_NUME caractere
 *   - prenume: string de maxim MAX_NUME caractere
 *   - an: anul de studiu (int)
 *   - grupa: numărul grupei (int)
 *   - note: array de NUM_NOTE întregi pentru notele studentului
 *   - medie: media calculată (double)
 *
 * Hint: Folosește typedef pentru a defini tipul Student
 */
typedef struct {
    /* YOUR CODE HERE */
    int id;
    /* Adaugă restul câmpurilor */
    
} Student;

/**
 * Structură pentru statistici (furnizată complet)
 */
typedef struct {
    int total_studenti;
    int promovati;
    int nepromovati;
    double medie_generala;
    double nota_min;
    double nota_max;
} Statistici;

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

/**
 * TODO 2: Implementează funcția care calculează media notelor
 *
 * @param note Array-ul de note
 * @param n Numărul de note din array
 * @return Media notelor (ignorând notele de 0)
 *
 * Algoritm:
 *   1. Inițializează suma și counter-ul la 0
 *   2. Parcurge toate notele
 *   3. Pentru fiecare notă > 0, adaugă la sumă și incrementează counter
 *   4. Returnează suma / counter (sau 0 dacă counter e 0)
 *
 * Hint: Folosește double pentru suma și rezultat
 */
double calculeaza_medie(const int note[], int n) {
    /* YOUR CODE HERE */
    
    return 0.0;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 3: Implementează funcția de parsare a unei linii CSV
 *
 * @param line Linia de text de parsat
 * @param student Pointer la structura Student unde se salvează datele
 * @return 1 dacă parsarea a reușit, 0 în caz contrar
 *
 * Format linie: ID Nume Prenume An Grupa Nota1 Nota2 ... Nota10
 * Exemplu: "1122334 Antonio Clim 2022 1050 5 6 7 8 9 10 7 6 6 5"
 *
 * Algoritm:
 *   1. Folosește sscanf pentru a extrage valorile
 *   2. Verifică dacă s-au citit cel puțin 5 câmpuri (ID, Nume, Prenume, An, Grupa)
 *   3. Inițializează notele lipsă cu 0
 *   4. Calculează media folosind funcția calculeaza_medie
 *   5. Returnează 1 pentru succes, 0 pentru eroare
 *
 * Hint: sscanf returnează numărul de câmpuri citite cu succes
 * Hint: Limitează lungimea string-urilor în format (ex: %49s pentru MAX_NUME-1)
 */
int parseaza_linie(const char *line, Student *student) {
    /* YOUR CODE HERE */
    
    return 0;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 4: Implementează funcția de citire a studenților din fișier
 *
 * @param filename Numele fișierului CSV
 * @param studenti Array-ul unde se salvează studenții
 * @param max_studenti Dimensiunea maximă a array-ului
 * @return Numărul de studenți citiți, sau -1 în caz de eroare
 *
 * Algoritm:
 *   1. Deschide fișierul pentru citire ("r")
 *   2. Verifică dacă deschiderea a reușit
 *   3. Citește linie cu linie folosind fgets
 *   4. Pentru fiecare linie, apelează parseaza_linie
 *   5. Închide fișierul
 *   6. Returnează numărul de studenți citiți
 *
 * Hint: Folosește fgets(buffer, sizeof(buffer), fp) pentru citire
 * Hint: Verifică ferror(fp) după bucla de citire
 */
int citeste_studenti(const char *filename, Student studenti[], int max_studenti) {
    /* YOUR CODE HERE */
    
    return -1;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 5: Implementează funcția de comparare pentru qsort
 *
 * @param a Pointer la primul element
 * @param b Pointer la al doilea element
 * @return Valoare negativă dacă a > b, pozitivă dacă a < b, 0 dacă egale
 *
 * Această funcție sortează DESCRESCĂTOR după medie (cel mai mare primul)
 *
 * Hint: Cast-ează parametrii la (const Student *)
 * Hint: Pentru sortare descrescătoare, returnează:
 *       1 dacă b->medie > a->medie
 *       -1 dacă b->medie < a->medie
 *       0 dacă sunt egale
 */
int compara_studenti(const void *a, const void *b) {
    /* YOUR CODE HERE */
    
    return 0;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 6: Implementează funcția de calculare statistici
 *
 * @param studenti Array-ul de studenți
 * @param n Numărul de studenți
 * @return Structura Statistici completată
 *
 * Statistici de calculat:
 *   - total_studenti: n
 *   - promovati: numărul de studenți cu media >= 5.0
 *   - nepromovati: numărul de studenți cu media < 5.0
 *   - medie_generala: media aritmetică a mediilor tuturor studenților
 *   - nota_min: cea mai mică medie
 *   - nota_max: cea mai mare medie
 *
 * Hint: Inițializează nota_min cu o valoare mare (ex: 11.0)
 * Hint: Inițializează nota_max cu o valoare mică (ex: 0.0)
 */
Statistici calculeaza_statistici(const Student studenti[], int n) {
    Statistici stats = {0, 0, 0, 0.0, 11.0, 0.0};
    
    /* YOUR CODE HERE */
    
    return stats;
}

/**
 * TODO 7: Implementează funcția de generare a raportului
 *
 * @param filename Numele fișierului de output
 * @param studenti Array-ul de studenți (deja sortat)
 * @param n Numărul de studenți
 * @param stats Statisticile calculate
 * @return 0 pentru succes, -1 pentru eroare
 *
 * Formatul raportului:
 *   === RAPORT NOTE STUDENȚI ===
 *   
 *   Nr. | ID      | Nume              | Grupa | Media | Status
 *   ----+---------+-------------------+-------+-------+------------
 *   1   | 222222  | Badass Vader      | 1051  | 9.30  | PROMOVAT
 *   2   | 1122334 | Antonio Clim      | 1050  | 6.90  | PROMOVAT
 *   ...
 *   
 *   === STATISTICI ===
 *   Total studenți: X
 *   Promovați: X (XX.X%)
 *   Nepromovați: X (XX.X%)
 *   Media generală: X.XX
 *   Nota minimă: X.XX
 *   Nota maximă: X.XX
 *
 * Hint: Folosește fprintf pentru scriere formatată
 * Hint: Status = (medie >= 5.0) ? "PROMOVAT" : "NEPROMOVAT"
 */
int genereaza_raport(const char *filename, const Student studenti[], int n, Statistici stats) {
    /* YOUR CODE HERE */
    
    return -1;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 8: Implementează funcția de afișare a studenților în consolă
 *
 * @param studenti Array-ul de studenți
 * @param n Numărul de studenți
 *
 * Afișează un tabel formatat în consolă cu toți studenții.
 *
 * Hint: Folosește printf cu specificatori de lățime (ex: %-20s, %6.2f)
 */
void afiseaza_studenti(const Student studenti[], int n) {
    printf("\n");
    printf("┌─────┬─────────┬──────────────────────┬───────┬────────┬────────────┐\n");
    printf("│ Nr. │   ID    │ Nume                 │ Grupa │ Medie  │ Status     │\n");
    printf("├─────┼─────────┼──────────────────────┼───────┼────────┼────────────┤\n");
    
    /* YOUR CODE HERE */
    /* Pentru fiecare student, afișează o linie în tabel */
    
    printf("└─────┴─────────┴──────────────────────┴───────┴────────┴────────────┘\n");
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    /* Numele fișierelor */
    const char *input_file = (argc > 1) ? argv[1] : "../data/studgrades.csv";
    const char *output_file = (argc > 2) ? argv[2] : "report.txt";
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║       EXERCIȚIUL 1: Analizator de Note Studenți               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Array pentru studenți */
    Student studenti[MAX_STUDENTI];
    
    /* Pasul 1: Citire date */
    printf("📂 Citire date din '%s'...\n", input_file);
    int n = citeste_studenti(input_file, studenti, MAX_STUDENTI);
    
    if (n < 0) {
        fprintf(stderr, "❌ Eroare la citirea fișierului!\n");
        return EXIT_FAILURE;
    }
    
    if (n == 0) {
        printf("⚠️  Nu s-au găsit studenți în fișier.\n");
        return EXIT_SUCCESS;
    }
    
    printf("✓ %d studenți citiți cu succes.\n\n", n);
    
    /* Pasul 2: Sortare după medie */
    printf("📊 Sortare studenți după medie (descrescător)...\n");
    qsort(studenti, n, sizeof(Student), compara_studenti);
    printf("✓ Sortare completă.\n\n");
    
    /* Pasul 3: Calculare statistici */
    printf("📈 Calculare statistici...\n");
    Statistici stats = calculeaza_statistici(studenti, n);
    printf("✓ Statistici calculate.\n\n");
    
    /* Pasul 4: Afișare în consolă */
    printf("📋 Lista studenților:\n");
    afiseaza_studenti(studenti, n);
    
    /* Pasul 5: Generare raport */
    printf("\n📝 Generare raport în '%s'...\n", output_file);
    if (genereaza_raport(output_file, studenti, n, stats) == 0) {
        printf("✓ Raport generat cu succes!\n");
    } else {
        fprintf(stderr, "❌ Eroare la generarea raportului!\n");
        return EXIT_FAILURE;
    }
    
    /* Afișare statistici în consolă */
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                      STATISTICI                               ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║  Total studenți:    %3d                                       ║\n", stats.total_studenti);
    printf("║  Promovați:         %3d (%.1f%%)                               ║\n", 
           stats.promovati, (n > 0) ? (double)stats.promovati / n * 100 : 0);
    printf("║  Nepromovați:       %3d (%.1f%%)                               ║\n",
           stats.nepromovati, (n > 0) ? (double)stats.nepromovati / n * 100 : 0);
    printf("║  Media generală:    %.2f                                      ║\n", stats.medie_generala);
    printf("║  Nota minimă:       %.2f                                      ║\n", stats.nota_min);
    printf("║  Nota maximă:       %.2f                                      ║\n", stats.nota_max);
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    printf("\n✅ Procesare completă!\n");
    
    return EXIT_SUCCESS;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Adaugă suport pentru citirea header-ului CSV (prima linie = nume coloane)
 *
 * 2. Implementează filtrare după grupă (ex: afișează doar studenții din grupa 1050)
 *
 * 3. Adaugă opțiune pentru sortare după alte criterii (ID, nume, grupă)
 *
 * 4. Implementează export în format HTML cu tabel stilizat
 *
 * 5. Adaugă detectarea și raportarea liniilor cu format invalid
 *
 * =============================================================================
 */
