/**
 * =============================================================================
 * SOLUȚIE: Exercițiul 2 - Bază de Date Studenți cu Sortare și Căutare
 * =============================================================================
 * DOAR PENTRU INSTRUCTOR - Nu distribuiți studenților
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTI 100
#define MAX_LUNGIME_NUME 50
#define MAX_LUNGIME_LINIE 128

typedef struct {
    int id;
    char nume[MAX_LUNGIME_NUME];
    float nota;
} Student;

/* Comparatoare pentru qsort */
int cmp_dupa_id(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;
    return s1->id - s2->id;
}

int cmp_dupa_nume(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;
    return strcmp(s1->nume, s2->nume);
}

int cmp_dupa_nota_desc(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;

    if (s2->nota > s1->nota) return 1;
    if (s2->nota < s1->nota) return -1;
    return 0;
}

int cmp_dupa_nota_cresc(const void *a, const void *b) {
    return -cmp_dupa_nota_desc(a, b);
}

/* Comparator pentru bsearch (cheia este string cu nume) */
int cmp_cauta_nume(const void *cheie, const void *element) {
    const char *nume_cautat = (const char *)cheie;
    const Student *student = (const Student *)element;
    return strcmp(nume_cautat, student->nume);
}

/* Funcții utilitare */
void afiseaza_student(const Student *s) {
    printf("  │ %4d │ %-30s │ %6.2f │\n", s->id, s->nume, s->nota);
}

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

int incarca_studenti(const char *nume_fisier, Student *studenti, int max_studenti) {
    FILE *fisier = fopen(nume_fisier, "r");
    if (fisier == NULL) {
        perror("Eroare la deschiderea fișierului");
        return -1;
    }

    char linie[MAX_LUNGIME_LINIE];
    int contor = 0;
    int nr_linie = 0;

    while (fgets(linie, sizeof(linie), fisier) != NULL && contor < max_studenti) {
        nr_linie++;

        /* Elimină newline-ul de la final */
        linie[strcspn(linie, "\n")] = '\0';

        /* Sari linia header */
        if (nr_linie == 1 && strncmp(linie, "id", 2) == 0) {
            continue;
        }

        /* Sari liniile goale */
        if (strlen(linie) == 0) {
            continue;
        }

        /* Parsează: id,nume,nota */
        int id;
        char nume[MAX_LUNGIME_NUME];
        float nota;

        if (sscanf(linie, "%d,%49[^,],%f", &id, nume, &nota) == 3) {
            studenti[contor].id = id;
            strncpy(studenti[contor].nume, nume, MAX_LUNGIME_NUME - 1);
            studenti[contor].nume[MAX_LUNGIME_NUME - 1] = '\0';
            studenti[contor].nota = nota;
            contor++;
        } else {
            fprintf(stderr, "Avertisment: Nu s-a putut parsa linia %d: %s\n", nr_linie, linie);
        }
    }

    fclose(fisier);
    return contor;
}

void afiseaza_top_studenti(Student *studenti, int n, int top_n) {
    /* Sortează după notă descrescător */
    qsort(studenti, n, sizeof(Student), cmp_dupa_nota_desc);

    printf("\n🏆 Top %d Studenți:\n", top_n);
    for (int i = 0; i < top_n && i < n; i++) {
        printf("   %d. %s - %.2f\n", i + 1, studenti[i].nume, studenti[i].nota);
    }
}

Student* gaseste_student_dupa_nume(Student *studenti, int n, const char *nume) {
    return (Student *)bsearch(nume, studenti, n, sizeof(Student), cmp_cauta_nume);
}

int main(int argc, char *argv[]) {
    Student studenti[MAX_STUDENTI];
    int contor;

    const char *nume_fisier = (argc > 1) ? argv[1] : "data/studenti.txt";

    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           SISTEM DE GESTIUNE BAZĂ DE DATE STUDENȚI            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    printf("\nÎncarcare studenți din: %s\n", nume_fisier);
    contor = incarca_studenti(nume_fisier, studenti, MAX_STUDENTI);

    if (contor <= 0) {
        printf("Nu s-au încărcat studenți. Se folosesc date exemplu.\n");

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

    /* Sortează după ID */
    printf("\n─── Sortare după ID ───\n");
    qsort(studenti, contor, sizeof(Student), cmp_dupa_id);
    afiseaza_studenti(studenti, contor, "Sortat după ID");

    /* Sortează după nume */
    printf("\n─── Sortare după Nume ───\n");
    qsort(studenti, contor, sizeof(Student), cmp_dupa_nume);
    afiseaza_studenti(studenti, contor, "Sortat după Nume");

    /* Afișează top 3 studenți */
    printf("\n─── Top Studenți ───\n");
    afiseaza_top_studenti(studenti, contor, 3);

    /* Caută studenți (re-sortează după nume mai întâi!) */
    printf("\n─── Rezultate Căutare ───\n");
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
