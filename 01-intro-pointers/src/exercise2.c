/**
 * =============================================================================
 * EXERCIȚIUL 2: Bază de Date Studenți cu Sortare și Căutare
 * =============================================================================
 *
 * OBIECTIV:
 *   Creează un sistem de gestiune studenți folosind qsort() pentru sortare
 *   și bsearch() pentru căutare eficientă.
 *
 * CERINȚE:
 *   1. Definește o structură Student (id, nume, nota)
 *   2. Implementează comparatoare pentru sortare după fiecare câmp
 *   3. Încarcă studenți dintr-un fișier
 *   4. Sortează și afișează studenți după diferite criterii
 *   5. Caută studenți după nume folosind bsearch()
 *   6. Afișează top N studenți după notă
 *
 * FORMAT FIȘIER INPUT (data/studenti.txt):
 *   id,nume,nota
 *   101,Popescu Ion,8.50
 *   102,Ionescu Maria,9.25
 *   ...
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =============================================================================
 * CONSTANTE ȘI DEFINIȚII DE TIP
 * =============================================================================
 */

#define MAX_STUDENTI 100
#define MAX_LUNGIME_NUME 50
#define MAX_LUNGIME_LINIE 128

/**
 * TODO 1: Definește structura Student
 *
 * Câmpuri:
 *   - int id          (numărul de identificare al studentului)
 *   - char nume[50]   (numele complet al studentului)
 *   - float nota      (nota de la 0.0 la 10.0)
 */
typedef struct {
    /* CODUL TĂU AICI */
    int id;          /* Elimină acest placeholder */
    char nume[50];   /* Elimină acest placeholder */
    float nota;      /* Elimină acest placeholder */
} Student;


/* =============================================================================
 * FUNCȚII COMPARATOR PENTRU QSORT
 * =============================================================================
 */

/**
 * TODO 2: Compară studenți după ID (crescător)
 *
 * @param a Pointer la primul Student (ca const void*)
 * @param b Pointer la al doilea Student (ca const void*)
 * @return Negativ dacă a < b, 0 dacă sunt egale, pozitiv dacă a > b
 *
 * Pași:
 *   1. Convertește pointerii void la const Student*
 *   2. Compară câmpurile id
 *   3. Returnează diferența (sau folosește comparație sigură)
 */
int cmp_dupa_id(const void *a, const void *b) {
    /* CODUL TĂU AICI */
    return 0;  /* Înlocuiește aceasta */
}

/**
 * TODO 3: Compară studenți după nume (alfabetic, case-sensitive)
 *
 * Hint: Folosește strcmp() din <string.h>
 */
int cmp_dupa_nume(const void *a, const void *b) {
    /* CODUL TĂU AICI */
    return 0;  /* Înlocuiește aceasta */
}

/**
 * TODO 4: Compară studenți după notă (descrescător - cea mai mare prima)
 *
 * IMPORTANT: Pentru comparație float, nu folosi scădere!
 * Folosește comparație condițională pentru a evita probleme cu virgula mobilă.
 *
 * if (nota2 > nota1) return 1;
 * if (nota2 < nota1) return -1;
 * return 0;
 */
int cmp_dupa_nota_desc(const void *a, const void *b) {
    /* CODUL TĂU AICI */
    return 0;  /* Înlocuiește aceasta */
}

/**
 * Compară studenți după notă (crescător - cea mai mică prima)
 */
int cmp_dupa_nota_cresc(const void *a, const void *b) {
    return -cmp_dupa_nota_desc(a, b);
}


/* =============================================================================
 * COMPARATOR PENTRU BSEARCH
 * =============================================================================
 */

/**
 * TODO 5: Comparator pentru căutare după nume
 *
 * IMPORTANT: În bsearch(), PRIMUL parametru este CHEIA (ce căutăm),
 * și AL DOILEA parametru este elementul din array.
 *
 * @param cheie Numele de căutat (const char*)
 * @param element Studentul cu care comparăm (const Student*)
 *
 * Hint:
 *   const char *nume_cautat = (const char*)cheie;
 *   const Student *student = (const Student*)element;
 *   return strcmp(nume_cautat, student->nume);
 */
int cmp_cauta_nume(const void *cheie, const void *element) {
    /* CODUL TĂU AICI */
    return 0;  /* Înlocuiește aceasta */
}


/* =============================================================================
 * FUNCȚII UTILITARE
 * =============================================================================
 */

/**
 * Afișează informațiile unui singur student
 */
void afiseaza_student(const Student *s) {
    printf("  │ %4d │ %-30s │ %6.2f │\n", s->id, s->nume, s->nota);
}

/**
 * Afișează toți studenții într-un tabel formatat
 */
void afiseaza_studenti(Student *studenti, int n, const char *titlu) {
    printf("\n%s (%d studenți):\n", titlu, n);
    printf("  ┌──────┬────────────────────────────────┬────────┐\n");
    printf("  │  ID  │ Nume                           │ Nota   │\n");
    printf("  ├──────┼────────────────────────────────┼────────┤\n");

    for (int i = 0; i < n; i++) {
        afiseaza_student(&studenti[i]);
    }

    printf("  └──────┴────────────────────────────────┴────────┘\n");
}

/**
 * TODO 6: Încarcă studenți dintr-un fișier CSV
 *
 * Format fișier: id,nume,nota (un student pe linie)
 * Prima linie poate fi header (sari dacă începe cu "id")
 *
 * @param nume_fisier Calea către fișierul CSV
 * @param studenti Array pentru stocarea studenților încărcați
 * @param max_studenti Capacitatea maximă a array-ului
 * @return Numărul de studenți încărcați, sau -1 la eroare
 *
 * Pași:
 *   1. Deschide fișierul cu fopen()
 *   2. Citește linii cu fgets()
 *   3. Parsează fiecare linie cu sscanf() sau strtok()
 *   4. Stochează în array-ul studenti
 *   5. Închide fișierul și returnează contorul
 *
 * Hint pentru parsare:
 *   sscanf(linie, "%d,%49[^,],%f", &id, nume, &nota)
 *   %49[^,] citește până la 49 caractere până la virgulă
 */
int incarca_studenti(const char *nume_fisier, Student *studenti, int max_studenti) {
    FILE *fisier = fopen(nume_fisier, "r");
    if (fisier == NULL) {
        perror("Eroare la deschiderea fișierului");
        return -1;
    }

    char linie[MAX_LUNGIME_LINIE];
    int contor = 0;

    /* TODO: Citește și parsează fiecare linie */
    while (fgets(linie, sizeof(linie), fisier) != NULL && contor < max_studenti) {
        /* Sari linia header dacă există */
        if (contor == 0 && strncmp(linie, "id", 2) == 0) {
            continue;
        }

        /* CODUL TĂU AICI */
        /* Parsează linia și stochează în studenti[contor] */
        /* Incrementează contor dacă parsarea a reușit */

    }

    fclose(fisier);
    return contor;
}

/**
 * TODO 7: Găsește și afișează top N studenți după notă
 *
 * @param studenti Array de studenți (va fi sortat pe loc)
 * @param n Numărul de studenți în array
 * @param top_n Numărul de studenți de top de afișat
 *
 * Pași:
 *   1. Sortează studenții după notă (descrescător)
 *   2. Afișează primii N studenți
 */
void afiseaza_top_studenti(Student *studenti, int n, int top_n) {
    /* CODUL TĂU AICI */
    /* Sortează după notă descrescător */
    /* Afișează top N */

    printf("\n🏆 Top %d Studenți:\n", top_n);
    printf("   (Neimplementat încă)\n");
}

/**
 * TODO 8: Caută un student după nume
 *
 * @param studenti Array de studenți (TREBUIE să fie sortat după nume!)
 * @param n Numărul de studenți
 * @param nume Numele de căutat
 * @return Pointer la studentul găsit, sau NULL dacă nu e găsit
 *
 * Pași:
 *   1. Folosește bsearch() cu comparatorul cmp_cauta_nume
 *   2. Returnează rezultatul (pointer sau NULL)
 *
 * IMPORTANT: Array-ul trebuie sortat după nume înainte de apelul bsearch!
 */
Student* gaseste_student_dupa_nume(Student *studenti, int n, const char *nume) {
    /* CODUL TĂU AICI */
    return NULL;  /* Înlocuiește aceasta */
}


/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    Student studenti[MAX_STUDENTI];
    int contor;

    /* Determină fișierul de input */
    const char *nume_fisier = (argc > 1) ? argv[1] : "data/studenti.txt";

    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           SISTEM DE GESTIUNE BAZĂ DE DATE STUDENȚI            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    /* Încarcă studenți din fișier */
    printf("\nÎncarcare studenți din: %s\n", nume_fisier);
    contor = incarca_studenti(nume_fisier, studenti, MAX_STUDENTI);

    if (contor <= 0) {
        printf("Nu s-au încărcat studenți. Se folosesc date exemplu.\n");

        /* Date exemplu dacă fișierul nu e găsit */
        studenti[0] = (Student){105, "Popescu Alexandru", 8.50};
        studenti[1] = (Student){101, "Ionescu Maria", 9.25};
        studenti[2] = (Student){103, "Georgescu Ana", 7.80};
        studenti[3] = (Student){102, "Vasilescu Dan", 9.50};
        studenti[4] = (Student){104, "Marinescu Elena", 8.90};
        studenti[5] = (Student){106, "Dumitrescu Mihai", 6.75};
        contor = 6;
    }

    printf("S-au încărcat %d studenți.\n", contor);

    /* Afișează lista originală */
    afiseaza_studenti(studenti, contor, "Lista Originală");

    /* TODO 9: Sortează după ID și afișează */
    printf("\n─── Sortare după ID ───\n");
    /* CODUL TĂU AICI: qsort după id, apoi afiseaza_studenti */


    /* TODO 10: Sortează după nume și afișează */
    printf("\n─── Sortare după Nume ───\n");
    /* CODUL TĂU AICI: qsort după nume, apoi afiseaza_studenti */


    /* TODO 11: Afișează top 3 studenți */
    printf("\n─── Top Studenți ───\n");
    afiseaza_top_studenti(studenti, contor, 3);


    /* TODO 12: Caută studenți după nume */
    printf("\n─── Rezultate Căutare ───\n");
    /* Mai întâi, sortează după nume pentru ca bsearch să funcționeze! */
    qsort(studenti, contor, sizeof(Student), cmp_dupa_nume);

    const char *nume_de_gasit[] = {"Ionescu Maria", "Vasilescu Dan", "Persoană Necunoscută"};
    for (int i = 0; i < 3; i++) {
        printf("Se caută '%s': ", nume_de_gasit[i]);

        Student *gasit = gaseste_student_dupa_nume(studenti, contor, nume_de_gasit[i]);

        if (gasit) {
            printf("Găsit! ID=%d, Nota=%.2f\n", gasit->id, gasit->nota);
        } else {
            printf("Nu a fost găsit.\n");
        }
    }

    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    PROGRAM COMPLET                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    return 0;
}


/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Implementează comparație de nume case-insensitive folosind strcasecmp()
 *
 * 2. Adaugă o funcție pentru a găsi toți studenții cu nota peste un prag
 *
 * 3. Implementează sortare după mai multe câmpuri (ex: după notă apoi după nume)
 *
 * 4. Adaugă o interfață interactivă bazată pe meniu
 *
 * 5. Implementează salveaza_studenti() pentru a scrie înapoi în CSV
 *
 * =============================================================================
 */
