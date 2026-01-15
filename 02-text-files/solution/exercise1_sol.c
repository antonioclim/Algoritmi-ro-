/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 1: Analizator de Note Studenți
 * =============================================================================
 *
 * ATENȚIE: Acest fișier conține soluția completă!
 * Destinat DOAR instructorilor pentru verificare și evaluare.
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
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

/* SOLUȚIE TODO 1: Structura Student completă */
typedef struct {
    int id;
    char nume[MAX_NUME];
    char prenume[MAX_NUME];
    int an;
    int grupa;
    int note[NUM_NOTE];
    double medie;
} Student;

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

/* SOLUȚIE TODO 2: Calcularea mediei */
double calculeaza_medie(const int note[], int n) {
    double suma = 0;
    int count = 0;
    
    for (int i = 0; i < n; i++) {
        if (note[i] > 0) {
            suma += note[i];
            count++;
        }
    }
    
    return (count > 0) ? suma / count : 0.0;
}

/* SOLUȚIE TODO 3: Parsarea unei linii CSV */
int parseaza_linie(const char *line, Student *student) {
    /* Folosim sscanf pentru a extrage valorile */
    int items = sscanf(line, "%d %49s %49s %d %d %d %d %d %d %d %d %d %d %d %d",
                       &student->id, student->nume, student->prenume, 
                       &student->an, &student->grupa,
                       &student->note[0], &student->note[1], &student->note[2], 
                       &student->note[3], &student->note[4], &student->note[5], 
                       &student->note[6], &student->note[7], &student->note[8], 
                       &student->note[9]);
    
    if (items >= 5) {
        /* Inițializăm notele lipsă cu 0 */
        for (int i = items - 5; i < NUM_NOTE; i++) {
            student->note[i] = 0;
        }
        /* Calculăm media */
        student->medie = calculeaza_medie(student->note, NUM_NOTE);
        return 1;
    }
    
    return 0;
}

/* SOLUȚIE TODO 4: Citirea studenților din fișier */
int citeste_studenti(const char *filename, Student studenti[], int max_studenti) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Eroare la deschiderea fișierului '%s': %s\n", 
                filename, strerror(errno));
        return -1;
    }
    
    char line[MAX_LINE];
    int count = 0;
    int line_num = 0;
    
    while (count < max_studenti && fgets(line, sizeof(line), fp) != NULL) {
        line_num++;
        
        /* Elimină newline de la final */
        line[strcspn(line, "\r\n")] = '\0';
        
        /* Ignoră liniile goale */
        if (strlen(line) == 0) {
            continue;
        }
        
        /* Parsează linia */
        if (parseaza_linie(line, &studenti[count])) {
            count++;
        } else {
            fprintf(stderr, "Avertisment: Format invalid la linia %d\n", line_num);
        }
    }
    
    /* Verifică dacă a apărut o eroare la citire */
    if (ferror(fp)) {
        fprintf(stderr, "Eroare la citirea fișierului: %s\n", strerror(errno));
        fclose(fp);
        return -1;
    }
    
    fclose(fp);
    return count;
}

/* SOLUȚIE TODO 5: Funcție de comparare pentru qsort */
int compara_studenti(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;
    
    /* Sortare descrescătoare după medie */
    if (s2->medie > s1->medie) return 1;
    if (s2->medie < s1->medie) return -1;
    return 0;
}

/* SOLUȚIE TODO 6: Calcularea statisticilor */
Statistici calculeaza_statistici(const Student studenti[], int n) {
    Statistici stats = {n, 0, 0, 0.0, 11.0, 0.0};
    double suma_medii = 0;
    
    for (int i = 0; i < n; i++) {
        suma_medii += studenti[i].medie;
        
        /* Actualizare min/max */
        if (studenti[i].medie < stats.nota_min) {
            stats.nota_min = studenti[i].medie;
        }
        if (studenti[i].medie > stats.nota_max) {
            stats.nota_max = studenti[i].medie;
        }
        
        /* Numărare promovați/nepromovați */
        if (studenti[i].medie >= NOTA_PROMOVARE) {
            stats.promovati++;
        } else {
            stats.nepromovati++;
        }
    }
    
    stats.medie_generala = (n > 0) ? suma_medii / n : 0.0;
    
    /* Corectare pentru cazul fără studenți */
    if (n == 0) {
        stats.nota_min = 0.0;
    }
    
    return stats;
}

/* SOLUȚIE TODO 7: Generarea raportului */
int genereaza_raport(const char *filename, const Student studenti[], int n, Statistici stats) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Eroare la crearea raportului '%s': %s\n", 
                filename, strerror(errno));
        return -1;
    }
    
    /* Header */
    fprintf(fp, "=== RAPORT NOTE STUDENȚI ===\n\n");
    
    /* Tabel cu studenți */
    fprintf(fp, "Nr. | ID      | Nume              | Grupa | Media | Status\n");
    fprintf(fp, "----+---------+-------------------+-------+-------+------------\n");
    
    for (int i = 0; i < n; i++) {
        char nume_complet[MAX_NUME * 2];
        snprintf(nume_complet, sizeof(nume_complet), "%s %s", 
                 studenti[i].nume, studenti[i].prenume);
        
        const char *status = (studenti[i].medie >= NOTA_PROMOVARE) ? 
                             "PROMOVAT" : "NEPROMOVAT";
        
        fprintf(fp, "%-3d | %-7d | %-17s | %5d | %5.2f | %s\n",
                i + 1, studenti[i].id, nume_complet, 
                studenti[i].grupa, studenti[i].medie, status);
    }
    
    /* Statistici */
    fprintf(fp, "\n=== STATISTICI ===\n");
    fprintf(fp, "Total studenți: %d\n", stats.total_studenti);
    fprintf(fp, "Promovați: %d (%.1f%%)\n", stats.promovati, 
            (n > 0) ? (double)stats.promovati / n * 100 : 0.0);
    fprintf(fp, "Nepromovați: %d (%.1f%%)\n", stats.nepromovati,
            (n > 0) ? (double)stats.nepromovati / n * 100 : 0.0);
    fprintf(fp, "Media generală: %.2f\n", stats.medie_generala);
    fprintf(fp, "Nota minimă: %.2f\n", stats.nota_min);
    fprintf(fp, "Nota maximă: %.2f\n", stats.nota_max);
    
    fclose(fp);
    return 0;
}

/* SOLUȚIE TODO 8: Afișarea studenților în consolă */
void afiseaza_studenti(const Student studenti[], int n) {
    printf("\n");
    printf("┌─────┬─────────┬──────────────────────┬───────┬────────┬────────────┐\n");
    printf("│ Nr. │   ID    │ Nume                 │ Grupa │ Medie  │ Status     │\n");
    printf("├─────┼─────────┼──────────────────────┼───────┼────────┼────────────┤\n");
    
    for (int i = 0; i < n; i++) {
        char nume_complet[MAX_NUME * 2];
        snprintf(nume_complet, sizeof(nume_complet), "%s %s", 
                 studenti[i].nume, studenti[i].prenume);
        
        const char *status = (studenti[i].medie >= NOTA_PROMOVARE) ? 
                             "PROMOVAT" : "NEPROMOVAT";
        
        printf("│ %3d │ %7d │ %-20s │ %5d │ %6.2f │ %-10s │\n",
               i + 1, studenti[i].id, nume_complet, 
               studenti[i].grupa, studenti[i].medie, status);
    }
    
    printf("└─────┴─────────┴──────────────────────┴───────┴────────┴────────────┘\n");
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    const char *input_file = (argc > 1) ? argv[1] : "../data/studgrades.csv";
    const char *output_file = (argc > 2) ? argv[2] : "report.txt";
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║   SOLUȚIE EXERCIȚIUL 1: Analizator de Note Studenți           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
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
           stats.promovati, (n > 0) ? (double)stats.promovati / n * 100 : 0.0);
    printf("║  Nepromovați:       %3d (%.1f%%)                               ║\n",
           stats.nepromovati, (n > 0) ? (double)stats.nepromovati / n * 100 : 0.0);
    printf("║  Media generală:    %.2f                                      ║\n", stats.medie_generala);
    printf("║  Nota minimă:       %.2f                                      ║\n", stats.nota_min);
    printf("║  Nota maximă:       %.2f                                      ║\n", stats.nota_max);
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    printf("\n✅ Procesare completă!\n");
    
    return EXIT_SUCCESS;
}
