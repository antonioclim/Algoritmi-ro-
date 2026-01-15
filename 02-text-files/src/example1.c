/**
 * =============================================================================
 * SĂPTĂMÂNA 02: FIȘIERE TEXT ÎN C
 * Exemplu Complet Demonstrativ
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Deschiderea și închiderea fișierelor (fopen, fclose)
 *   2. Citirea datelor (fgets, fscanf, fgetc)
 *   3. Scrierea datelor (fprintf, fputs, fputc)
 *   4. Gestionarea erorilor (ferror, feof, perror)
 *   5. Parsarea fișierelor CSV
 *   6. Generarea rapoartelor formatate
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Utilizare: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

/* =============================================================================
 * CONSTANTE ȘI TIPURI DE DATE
 * =============================================================================
 */

#define MAX_STUDENTI 100
#define MAX_LINE 512
#define MAX_NUME 50
#define NUM_NOTE 10

/* Structură pentru a stoca datele unui student */
typedef struct {
    int id;
    char nume[MAX_NUME];
    char prenume[MAX_NUME];
    int an;
    int grupa;
    int note[NUM_NOTE];
    double medie;
} Student;

/* Structură pentru statistici */
typedef struct {
    double medie_generala;
    double nota_minima;
    double nota_maxima;
    int numar_promovati;
    int numar_nepromovati;
    int total_studenti;
} Statistici;

/* =============================================================================
 * PARTEA 1: OPERAȚII DE BAZĂ CU FIȘIERE
 * =============================================================================
 */

/**
 * Demonstrează deschiderea și închiderea fișierelor
 * cu verificare completă a erorilor.
 */
void demo_deschidere_fisiere(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 1: Deschiderea și Închiderea Fișierelor           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    /* 1.1 Deschidere pentru citire */
    printf("1.1 Deschidere pentru citire (mod \"r\"):\n");
    FILE *fp_read = fopen("inexistent.txt", "r");
    if (fp_read == NULL) {
        printf("    ✗ Fișierul nu există: %s\n", strerror(errno));
    } else {
        printf("    ✓ Fișier deschis cu succes\n");
        fclose(fp_read);
    }

    /* 1.2 Deschidere pentru scriere (creează fișier nou) */
    printf("\n1.2 Deschidere pentru scriere (mod \"w\"):\n");
    FILE *fp_write = fopen("test_output.txt", "w");
    if (fp_write == NULL) {
        printf("    ✗ Nu pot crea fișierul: %s\n", strerror(errno));
    } else {
        printf("    ✓ Fișier creat cu succes\n");
        fprintf(fp_write, "Linia 1: Test de scriere\n");
        fprintf(fp_write, "Linia 2: Număr întreg: %d\n", 42);
        fprintf(fp_write, "Linia 3: Număr real: %.2f\n", 3.14159);
        fclose(fp_write);
        printf("    ✓ Date scrise și fișier închis\n");
    }

    /* 1.3 Deschidere pentru append */
    printf("\n1.3 Deschidere pentru adăugare (mod \"a\"):\n");
    FILE *fp_append = fopen("test_output.txt", "a");
    if (fp_append != NULL) {
        fprintf(fp_append, "Linia 4: Adăugată ulterior\n");
        fclose(fp_append);
        printf("    ✓ Linie adăugată la sfârșitul fișierului\n");
    }

    /* 1.4 Verificare conținut */
    printf("\n1.4 Conținutul fișierului test_output.txt:\n");
    printf("    ─────────────────────────────────────\n");
    FILE *fp_verify = fopen("test_output.txt", "r");
    if (fp_verify != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), fp_verify) != NULL) {
            printf("    │ %s", line);
        }
        fclose(fp_verify);
    }
    printf("    ─────────────────────────────────────\n");
}

/* =============================================================================
 * PARTEA 2: CITIREA DATELOR
 * =============================================================================
 */

/**
 * Demonstrează diferite metode de citire din fișiere.
 */
void demo_citire_date(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 2: Metode de Citire din Fișiere                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    /* Crează un fișier de test */
    FILE *fp = fopen("test_citire.txt", "w");
    if (fp != NULL) {
        fprintf(fp, "Prima linie de text\n");
        fprintf(fp, "A doua linie cu numere: 123 456\n");
        fprintf(fp, "Student Ion 9.5\n");
        fprintf(fp, "ABCDEFGHIJ");  /* Fără newline la final */
        fclose(fp);
    }

    /* 2.1 Citire cu fgets() - metoda recomandată */
    printf("2.1 Citire cu fgets() (recomandat pentru linii):\n");
    fp = fopen("test_citire.txt", "r");
    if (fp != NULL) {
        char buffer[256];
        int line_num = 0;
        
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            /* Elimină newline de la final */
            buffer[strcspn(buffer, "\n")] = '\0';
            printf("    Linia %d: [%s]\n", ++line_num, buffer);
        }
        fclose(fp);
    }

    /* 2.2 Citire cu fscanf() - pentru date formatate */
    printf("\n2.2 Citire cu fscanf() (pentru date structurate):\n");
    fp = fopen("test_citire.txt", "r");
    if (fp != NULL) {
        char skip[256];
        /* Sărim primele 2 linii */
        fgets(skip, sizeof(skip), fp);
        fgets(skip, sizeof(skip), fp);
        
        /* Citim date structurate din linia 3 */
        char tip[20], nume[50];
        float nota;
        if (fscanf(fp, "%19s %49s %f", tip, nume, &nota) == 3) {
            printf("    Tip: %s, Nume: %s, Nota: %.1f\n", tip, nume, nota);
        }
        fclose(fp);
    }

    /* 2.3 Citire caracter cu caracter */
    printf("\n2.3 Citire cu fgetc() (caracter cu caracter):\n");
    fp = fopen("test_citire.txt", "r");
    if (fp != NULL) {
        printf("    Primele 20 caractere: ");
        int c;
        int count = 0;
        while ((c = fgetc(fp)) != EOF && count < 20) {
            if (c == '\n') {
                printf("\\n");
            } else {
                putchar(c);
            }
            count++;
        }
        printf("\n");
        fclose(fp);
    }

    /* Cleanup */
    remove("test_citire.txt");
}

/* =============================================================================
 * PARTEA 3: SCRIEREA DATELOR
 * =============================================================================
 */

/**
 * Demonstrează diferite metode de scriere în fișiere.
 */
void demo_scriere_date(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 3: Metode de Scriere în Fișiere                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    FILE *fp = fopen("test_scriere.txt", "w");
    if (fp == NULL) {
        perror("Eroare la deschidere");
        return;
    }

    /* 3.1 Scriere cu fprintf() */
    printf("3.1 Scriere formatată cu fprintf():\n");
    fprintf(fp, "=== RAPORT GENERARE ===\n");
    fprintf(fp, "%-20s %10s %10s\n", "Nume", "Valoare", "Procent");
    fprintf(fp, "%-20s %10d %9.1f%%\n", "Element Alpha", 1234, 45.6);
    fprintf(fp, "%-20s %10d %9.1f%%\n", "Element Beta", 5678, 54.4);
    printf("    ✓ Date formatate scrise\n");

    /* 3.2 Scriere cu fputs() */
    printf("\n3.2 Scriere string cu fputs():\n");
    fputs("\n--- Secțiune text simplu ---\n", fp);
    fputs("Aceasta este o linie scrisă cu fputs.\n", fp);
    printf("    ✓ String-uri scrise\n");

    /* 3.3 Scriere caracter cu fputc() */
    printf("\n3.3 Scriere caracter cu fputc():\n");
    fputs("\nCaractere individuale: ", fp);
    for (char c = 'A'; c <= 'Z'; c++) {
        fputc(c, fp);
    }
    fputc('\n', fp);
    printf("    ✓ Caractere scrise\n");

    /* 3.4 Tabel formatat */
    printf("\n3.4 Generare tabel formatat:\n");
    fprintf(fp, "\n┌────────┬────────────────────┬──────────┐\n");
    fprintf(fp, "│ %-6s │ %-18s │ %8s │\n", "ID", "Produs", "Preț");
    fprintf(fp, "├────────┼────────────────────┼──────────┤\n");
    fprintf(fp, "│ %6d │ %-18s │ %8.2f │\n", 1, "Laptop ASUS", 3499.99);
    fprintf(fp, "│ %6d │ %-18s │ %8.2f │\n", 2, "Mouse Logitech", 149.50);
    fprintf(fp, "│ %6d │ %-18s │ %8.2f │\n", 3, "Tastatură RGB", 299.00);
    fprintf(fp, "└────────┴────────────────────┴──────────┘\n");
    printf("    ✓ Tabel generat\n");

    fclose(fp);

    /* Afișare conținut */
    printf("\n3.5 Conținutul fișierului generat:\n");
    fp = fopen("test_scriere.txt", "r");
    if (fp != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), fp) != NULL) {
            printf("    %s", line);
        }
        fclose(fp);
    }

    /* Cleanup */
    remove("test_scriere.txt");
}

/* =============================================================================
 * PARTEA 4: PARSAREA FIȘIERELOR CSV
 * =============================================================================
 */

/**
 * Elimină spațiile de la începutul și sfârșitul unui string.
 */
char *trim(char *str) {
    char *end;
    
    /* Spații de la început */
    while (isspace((unsigned char)*str)) str++;
    
    if (*str == 0) return str;
    
    /* Spații de la final */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    
    end[1] = '\0';
    return str;
}

/**
 * Calculează media notelor unui student (ignorând notele de 0).
 */
double calculeaza_medie(const int note[], int n) {
    int suma = 0;
    int count = 0;
    
    for (int i = 0; i < n; i++) {
        if (note[i] > 0) {
            suma += note[i];
            count++;
        }
    }
    
    return (count > 0) ? (double)suma / count : 0.0;
}

/**
 * Parsează o linie CSV și extrage datele studentului.
 * Format așteptat: ID Nume Prenume An Grupa Nota1 Nota2 ... Nota10
 */
int parseaza_linie_student(const char *line, Student *s) {
    /* Folosim sscanf pentru a parsa linia */
    int items = sscanf(line, "%d %49s %49s %d %d %d %d %d %d %d %d %d %d %d %d",
                       &s->id, s->nume, s->prenume, &s->an, &s->grupa,
                       &s->note[0], &s->note[1], &s->note[2], &s->note[3], &s->note[4],
                       &s->note[5], &s->note[6], &s->note[7], &s->note[8], &s->note[9]);
    
    if (items >= 5) {
        /* Inițializăm notele lipsă cu 0 */
        for (int i = items - 5; i < NUM_NOTE; i++) {
            s->note[i] = 0;
        }
        s->medie = calculeaza_medie(s->note, NUM_NOTE);
        return 1;
    }
    
    return 0;
}

/**
 * Citește studenții dintr-un fișier CSV.
 */
int citeste_studenti_csv(const char *filename, Student studenti[], int max_studenti) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Nu pot deschide fișierul '%s': %s\n", filename, strerror(errno));
        return -1;
    }
    
    char line[MAX_LINE];
    int count = 0;
    int line_num = 0;
    
    while (count < max_studenti && fgets(line, sizeof(line), fp) != NULL) {
        line_num++;
        
        /* Elimină newline */
        line[strcspn(line, "\r\n")] = '\0';
        
        /* Ignoră liniile goale */
        if (strlen(trim((char*)line)) == 0) {
            continue;
        }
        
        /* Parsează linia */
        if (parseaza_linie_student(line, &studenti[count])) {
            count++;
        } else {
            fprintf(stderr, "Avertisment: Format invalid la linia %d\n", line_num);
        }
    }
    
    if (ferror(fp)) {
        perror("Eroare la citire");
        fclose(fp);
        return -1;
    }
    
    fclose(fp);
    return count;
}

/**
 * Demonstrează parsarea CSV.
 */
void demo_parsare_csv(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 4: Parsarea Fișierelor CSV                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    /* Crează un fișier CSV de test */
    printf("4.1 Crearea fișierului CSV de test:\n");
    FILE *fp = fopen("studenti_test.csv", "w");
    if (fp != NULL) {
        fprintf(fp, "1001 Popescu Ion 2023 1050 8 9 7 10 8 9 7 8 9 10\n");
        fprintf(fp, "1002 Ionescu Maria 2023 1051 10 10 9 10 10 9 10 10 9 10\n");
        fprintf(fp, "1003 Georgescu Andrei 2023 1050 5 6 4 5 6 7 5 6 4 5\n");
        fprintf(fp, "1004 Dumitrescu Ana 2023 1051 7 8 7 8 7 8 7 8 7 8\n");
        fprintf(fp, "1005 Stanescu Mihai 2023 1050 3 4 5 3 4 5 3 4 5 3\n");
        fclose(fp);
        printf("    ✓ Fișier 'studenti_test.csv' creat cu 5 studenți\n");
    }

    /* Citește studenții */
    printf("\n4.2 Citirea și parsarea datelor:\n");
    Student studenti[MAX_STUDENTI];
    int n = citeste_studenti_csv("studenti_test.csv", studenti, MAX_STUDENTI);
    
    if (n > 0) {
        printf("    ✓ %d studenți citiți cu succes\n\n", n);
        
        /* Afișare date parsate */
        printf("4.3 Date parsate:\n");
        printf("    ┌──────┬─────────────────────┬───────┬────────┐\n");
        printf("    │ %4s │ %-19s │ %5s │ %6s │\n", "ID", "Nume Complet", "Grupa", "Medie");
        printf("    ├──────┼─────────────────────┼───────┼────────┤\n");
        
        for (int i = 0; i < n; i++) {
            char nume_complet[100];
            snprintf(nume_complet, sizeof(nume_complet), "%s %s", 
                     studenti[i].nume, studenti[i].prenume);
            printf("    │ %4d │ %-19s │ %5d │ %6.2f │\n",
                   studenti[i].id, nume_complet, studenti[i].grupa, studenti[i].medie);
        }
        printf("    └──────┴─────────────────────┴───────┴────────┘\n");
    }

    /* Cleanup */
    remove("studenti_test.csv");
}

/* =============================================================================
 * PARTEA 5: GENERAREA RAPOARTELOR
 * =============================================================================
 */

/**
 * Funcție de comparare pentru qsort (sortare descrescătoare după medie).
 */
int compara_studenti_medie_desc(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;
    
    if (s2->medie > s1->medie) return 1;
    if (s2->medie < s1->medie) return -1;
    return 0;
}

/**
 * Calculează statisticile pentru un grup de studenți.
 */
Statistici calculeaza_statistici(const Student studenti[], int n) {
    Statistici stats = {0, 11, 0, 0, 0, n};
    double suma_medii = 0;
    
    for (int i = 0; i < n; i++) {
        suma_medii += studenti[i].medie;
        
        if (studenti[i].medie < stats.nota_minima) {
            stats.nota_minima = studenti[i].medie;
        }
        if (studenti[i].medie > stats.nota_maxima) {
            stats.nota_maxima = studenti[i].medie;
        }
        
        if (studenti[i].medie >= 5.0) {
            stats.numar_promovati++;
        } else {
            stats.numar_nepromovati++;
        }
    }
    
    stats.medie_generala = (n > 0) ? suma_medii / n : 0;
    return stats;
}

/**
 * Generează un raport complet în fișier.
 */
void genereaza_raport(const char *filename, Student studenti[], int n) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Nu pot crea raportul");
        return;
    }
    
    /* Sortează studenții după medie */
    qsort(studenti, n, sizeof(Student), compara_studenti_medie_desc);
    
    /* Calculează statistici */
    Statistici stats = calculeaza_statistici(studenti, n);
    
    /* Header raport */
    fprintf(fp, "╔═══════════════════════════════════════════════════════════════════════════╗\n");
    fprintf(fp, "║                        RAPORT NOTE STUDENȚI                               ║\n");
    fprintf(fp, "║                    Sesiunea: Ianuarie 2024                                ║\n");
    fprintf(fp, "╠═══════════════════════════════════════════════════════════════════════════╣\n");
    
    /* Statistici generale */
    fprintf(fp, "║  📊 STATISTICI GENERALE                                                   ║\n");
    fprintf(fp, "║  ─────────────────────────────────────────────────────────────────────── ║\n");
    fprintf(fp, "║  Total studenți:     %3d                                                  ║\n", stats.total_studenti);
    fprintf(fp, "║  Promovați:          %3d (%.1f%%)                                          ║\n", 
            stats.numar_promovati, (double)stats.numar_promovati / n * 100);
    fprintf(fp, "║  Nepromovați:        %3d (%.1f%%)                                          ║\n",
            stats.numar_nepromovati, (double)stats.numar_nepromovati / n * 100);
    fprintf(fp, "║  Media generală:     %.2f                                                 ║\n", stats.medie_generala);
    fprintf(fp, "║  Nota minimă:        %.2f                                                 ║\n", stats.nota_minima);
    fprintf(fp, "║  Nota maximă:        %.2f                                                 ║\n", stats.nota_maxima);
    fprintf(fp, "╠═══════════════════════════════════════════════════════════════════════════╣\n");
    
    /* Lista studenților */
    fprintf(fp, "║  📋 LISTA STUDENȚILOR (ordonată după medie)                               ║\n");
    fprintf(fp, "╠═══════════════════════════════════════════════════════════════════════════╣\n");
    fprintf(fp, "║  Nr │  ID   │ Nume                │ Grupa │ Medie │ Status              ║\n");
    fprintf(fp, "║ ────┼───────┼─────────────────────┼───────┼───────┼──────────────────── ║\n");
    
    for (int i = 0; i < n; i++) {
        char nume_complet[40];
        snprintf(nume_complet, sizeof(nume_complet), "%s %s", 
                 studenti[i].nume, studenti[i].prenume);
        
        const char *status = (studenti[i].medie >= 5.0) ? "✓ Promovat" : "✗ Nepromovat";
        const char *emoji = (i < 3) ? "🏆" : "  ";
        
        fprintf(fp, "║ %s%2d │ %5d │ %-19s │ %5d │ %5.2f │ %-19s ║\n",
                emoji, i + 1, studenti[i].id, nume_complet, 
                studenti[i].grupa, studenti[i].medie, status);
    }
    
    fprintf(fp, "╠═══════════════════════════════════════════════════════════════════════════╣\n");
    
    /* Top 3 studenți */
    fprintf(fp, "║  🏆 TOP 3 STUDENȚI                                                        ║\n");
    fprintf(fp, "║  ─────────────────────────────────────────────────────────────────────── ║\n");
    int top = (n < 3) ? n : 3;
    for (int i = 0; i < top; i++) {
        char medal[10];
        if (i == 0) strcpy(medal, "🥇");
        else if (i == 1) strcpy(medal, "🥈");
        else strcpy(medal, "🥉");
        
        fprintf(fp, "║  %s %d. %s %s - Media: %.2f                                      ║\n",
                medal, i + 1, studenti[i].nume, studenti[i].prenume, studenti[i].medie);
    }
    
    fprintf(fp, "╚═══════════════════════════════════════════════════════════════════════════╝\n");
    fprintf(fp, "\nRaport generat automat - ATP Săptămâna 02\n");
    
    fclose(fp);
}

/**
 * Demonstrează generarea rapoartelor.
 */
void demo_generare_raport(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 5: Generarea Rapoartelor                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    /* Crează date de test */
    Student studenti[] = {
        {1001, "Popescu", "Ion", 2023, 1050, {8, 9, 7, 10, 8, 9, 7, 8, 9, 10}, 0},
        {1002, "Ionescu", "Maria", 2023, 1051, {10, 10, 9, 10, 10, 9, 10, 10, 9, 10}, 0},
        {1003, "Georgescu", "Andrei", 2023, 1050, {5, 6, 4, 5, 6, 7, 5, 6, 4, 5}, 0},
        {1004, "Dumitrescu", "Ana", 2023, 1051, {7, 8, 7, 8, 7, 8, 7, 8, 7, 8}, 0},
        {1005, "Stanescu", "Mihai", 2023, 1050, {3, 4, 5, 3, 4, 5, 3, 4, 5, 3}, 0}
    };
    int n = sizeof(studenti) / sizeof(studenti[0]);
    
    /* Calculează mediile */
    for (int i = 0; i < n; i++) {
        studenti[i].medie = calculeaza_medie(studenti[i].note, NUM_NOTE);
    }
    
    printf("5.1 Generarea raportului 'raport_studenti.txt'...\n");
    genereaza_raport("raport_studenti.txt", studenti, n);
    printf("    ✓ Raport generat cu succes!\n\n");
    
    /* Afișare raport */
    printf("5.2 Conținutul raportului:\n\n");
    FILE *fp = fopen("raport_studenti.txt", "r");
    if (fp != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), fp) != NULL) {
            printf("%s", line);
        }
        fclose(fp);
    }
    
    /* Cleanup */
    remove("raport_studenti.txt");
}

/* =============================================================================
 * PARTEA 6: GESTIONAREA ERORILOR
 * =============================================================================
 */

/**
 * Demonstrează gestionarea erorilor în operațiile I/O.
 */
void demo_gestionare_erori(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 6: Gestionarea Erorilor                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    /* 6.1 Eroare la deschidere */
    printf("6.1 Eroare la deschidere (fișier inexistent):\n");
    FILE *fp = fopen("/path/inexistent/file.txt", "r");
    if (fp == NULL) {
        printf("    Cod eroare (errno): %d\n", errno);
        printf("    Mesaj eroare: %s\n", strerror(errno));
    }

    /* 6.2 Verificare după operații */
    printf("\n6.2 Verificare feof() și ferror():\n");
    fp = fopen("test_erori.txt", "w");
    if (fp != NULL) {
        fprintf(fp, "Test\n");
        fclose(fp);
    }
    
    fp = fopen("test_erori.txt", "r");
    if (fp != NULL) {
        char buf[100];
        
        /* Citim tot conținutul */
        while (fgets(buf, sizeof(buf), fp) != NULL) {
            /* procesare */
        }
        
        printf("    După citire completă:\n");
        printf("    - feof(fp):   %s\n", feof(fp) ? "true (sfârșitul fișierului)" : "false");
        printf("    - ferror(fp): %s\n", ferror(fp) ? "true (eroare)" : "false (OK)");
        
        /* Resetare indicatori */
        clearerr(fp);
        printf("    După clearerr():\n");
        printf("    - feof(fp):   %s\n", feof(fp) ? "true" : "false");
        printf("    - ferror(fp): %s\n", ferror(fp) ? "true" : "false");
        
        fclose(fp);
    }

    /* 6.3 Pattern recomandat */
    printf("\n6.3 Pattern recomandat pentru gestionarea erorilor:\n");
    printf("    ┌─────────────────────────────────────────────────────────┐\n");
    printf("    │ FILE *fp = fopen(filename, mode);                       │\n");
    printf("    │ if (fp == NULL) {                                       │\n");
    printf("    │     perror(\"Mesaj descriptiv\");                         │\n");
    printf("    │     return ERROR_CODE;                                  │\n");
    printf("    │ }                                                       │\n");
    printf("    │ /* operații */                                          │\n");
    printf("    │ if (ferror(fp)) { /* tratare eroare */ }                │\n");
    printf("    │ fclose(fp);                                             │\n");
    printf("    └─────────────────────────────────────────────────────────┘\n");

    /* Cleanup */
    remove("test_erori.txt");
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 02: FIȘIERE TEXT - Exemplu Complet              ║\n");
    printf("║     Algoritmi și Tehnici de Programare (ATP)                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    /* Rulează toate demonstrațiile */
    demo_deschidere_fisiere();
    demo_citire_date();
    demo_scriere_date();
    demo_parsare_csv();
    demo_generare_raport();
    demo_gestionare_erori();

    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     ✓ Toate demonstrațiile finalizate cu succes!              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    /* Cleanup final */
    remove("test_output.txt");

    return 0;
}

/* =============================================================================
 * NOTIȚE PENTRU STUDENȚI
 * =============================================================================
 *
 * Puncte cheie de reținut:
 *
 * 1. VERIFICĂ ÎNTOTDEAUNA returnul fopen() înainte de a folosi FILE*
 * 2. ÎNCHIDE MEREU fișierele cu fclose() după utilizare
 * 3. Folosește fgets() în loc de gets() pentru a preveni buffer overflow
 * 4. Verifică ferror() după bucle de citire pentru a detecta erori
 * 5. Folosește perror() sau strerror(errno) pentru mesaje de eroare descriptive
 * 6. Elimină caracterul newline după fgets() cu strcspn()
 * 7. Folosește fscanf() pentru date structurate, dar verifică returnul
 * 8. fprintf() este flexibil pentru output formatat
 *
 * Compilare cu warning-uri:
 *   gcc -Wall -Wextra -std=c11 -o example1 example1.c
 *
 * Verificare memory leaks:
 *   valgrind --leak-check=full ./example1
 *
 * =============================================================================
 */
