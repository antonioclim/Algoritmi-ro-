/**
 * =============================================================================
 * SĂPTĂMÂNA 01: POINTERI LA FUNCȚII ȘI CALLBACKS ÎN C
 * Exemplu Complet Funcțional
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Declararea și utilizarea pointerilor la funcții
 *   2. Callback-uri pentru operații generice
 *   3. qsort() cu multiple funcții comparator
 *   4. bsearch() pentru căutare eficientă
 *   5. Dispatch tables folosind array-uri de pointeri la funcții
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Utilizare: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =============================================================================
 * PARTEA 1: POINTERI LA FUNCȚII - BAZĂ
 * =============================================================================
 */

/**
 * Operații aritmetice simple
 * Aceste funcții vor fi folosite prin pointeri la funcții
 */
int aduna(int a, int b) { return a + b; }
int scade(int a, int b) { return a - b; }
int inmulteste(int a, int b) { return a * b; }
int imparte_sigur(int a, int b) { return (b != 0) ? (a / b) : 0; }
int modulo(int a, int b) { return (b != 0) ? (a % b) : 0; }

/**
 * Demonstrează utilizarea de bază a pointerilor la funcții
 */
void demo_pointeri_functii_baza(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 1: POINTERI LA FUNCȚII - BAZĂ                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    /* Declară un pointer la funcție */
    int (*operatie)(int, int);

    int a = 20, b = 6;

    /* Metoda 1: Atribuire directă (numele funcției se convertește în pointer) */
    operatie = aduna;
    printf("Atribuire directă:\n");
    printf("  operatie = aduna;      → %d + %d = %d\n", a, b, operatie(a, b));

    /* Metoda 2: Folosind operatorul adresă (explicit) */
    operatie = &scade;
    printf("  operatie = &scade;     → %d - %d = %d\n", a, b, operatie(a, b));

    /* Metoda 3: Dereferențiere explicită la apel (stil mai vechi) */
    operatie = inmulteste;
    printf("  (*operatie)(a, b)      → %d * %d = %d\n", a, b, (*operatie)(a, b));

    printf("\n");
}

/* =============================================================================
 * PARTEA 2: CALLBACK-URI PENTRU OPERAȚII GENERICE
 * =============================================================================
 */

/**
 * Definiție de tip pentru cod mai curat
 */
typedef int (*OperatieBinara)(int, int);

/**
 * Funcție generică care aplică orice operație binară la două numere
 * Acesta este pattern-ul callback în acțiune!
 *
 * @param x Primul operand
 * @param y Al doilea operand
 * @param op Pointer la funcția de operație
 * @return Rezultatul aplicării op la x și y
 */
int aplica_operatie(int x, int y, OperatieBinara op) {
    if (op == NULL) {
        fprintf(stderr, "Eroare: Pointer la funcție NULL\n");
        return 0;
    }
    return op(x, y);
}

/**
 * Aplică o operație la întreg array-ul
 */
void aplica_la_array(int *arr, int n, int operand, OperatieBinara op) {
    for (int i = 0; i < n; i++) {
        arr[i] = op(arr[i], operand);
    }
}

void demo_callbacks(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 2: CALLBACK-URI PENTRU OPERAȚII GENERICE         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    int a = 15, b = 4;

    printf("Folosind aplica_operatie() cu diferite callback-uri:\n");
    printf("  aplica_operatie(%d, %d, aduna)      = %d\n", a, b, aplica_operatie(a, b, aduna));
    printf("  aplica_operatie(%d, %d, scade)      = %d\n", a, b, aplica_operatie(a, b, scade));
    printf("  aplica_operatie(%d, %d, inmulteste) = %d\n", a, b, aplica_operatie(a, b, inmulteste));
    printf("  aplica_operatie(%d, %d, imparte)    = %d\n", a, b, aplica_operatie(a, b, imparte_sigur));

    /* Aplică la array */
    printf("\nAplică operație la array:\n");
    int arr[] = {1, 2, 3, 4, 5};
    int n = 5;

    printf("  Original: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");

    aplica_la_array(arr, n, 10, inmulteste);
    printf("  După înmulțire cu 10: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n\n");
}

/* =============================================================================
 * PARTEA 3: QSORT() CU STRUCTURI
 * =============================================================================
 */

/**
 * Structura Student pentru demonstrație
 */
typedef struct {
    int id;
    char nume[50];
    float nota;
} Student;

/* Funcții comparator pentru qsort() */

/**
 * Compară studenți după ID (crescător)
 */
int cmp_dupa_id(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;
    return s1->id - s2->id;
}

/**
 * Compară studenți după nume (alfabetic)
 */
int cmp_dupa_nume(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;
    return strcmp(s1->nume, s2->nume);
}

/**
 * Compară studenți după notă (descrescător - cea mai mare prima)
 * Notă: Pentru float, nu folosi scădere! Folosește comparație condițională.
 */
int cmp_dupa_nota_desc(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;

    if (s2->nota > s1->nota) return 1;
    if (s2->nota < s1->nota) return -1;
    return 0;
}

/**
 * Compară studenți după notă (crescător)
 */
int cmp_dupa_nota_cresc(const void *a, const void *b) {
    return -cmp_dupa_nota_desc(a, b);
}

/**
 * Afișează lista de studenți într-un tabel formatat
 */
void afiseaza_studenti(Student *studenti, int n, const char *titlu) {
    printf("\n%s:\n", titlu);
    printf("  ┌─────┬────────────────────────┬────────┐\n");
    printf("  │ ID  │ Nume                   │ Nota   │\n");
    printf("  ├─────┼────────────────────────┼────────┤\n");
    for (int i = 0; i < n; i++) {
        printf("  │ %3d │ %-22s │ %6.2f │\n",
               studenti[i].id, studenti[i].nume, studenti[i].nota);
    }
    printf("  └─────┴────────────────────────┴────────┘\n");
}

void demo_qsort(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 3: QSORT() CU STRUCTURI                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    Student studenti[] = {
        {105, "Popescu Alexandru", 8.50},
        {101, "Ionescu Maria", 9.25},
        {103, "Georgescu Ana", 7.80},
        {102, "Vasilescu Dan", 9.50},
        {104, "Marinescu Elena", 8.90},
        {106, "Dumitrescu Mihai", 6.75}
    };
    int n = sizeof(studenti) / sizeof(studenti[0]);

    afiseaza_studenti(studenti, n, "Lista originală");

    /* Sortează după ID */
    qsort(studenti, n, sizeof(Student), cmp_dupa_id);
    afiseaza_studenti(studenti, n, "Sortare după ID (crescător)");

    /* Sortează după nume */
    qsort(studenti, n, sizeof(Student), cmp_dupa_nume);
    afiseaza_studenti(studenti, n, "Sortare după Nume (alfabetic)");

    /* Sortează după notă (descrescător) */
    qsort(studenti, n, sizeof(Student), cmp_dupa_nota_desc);
    afiseaza_studenti(studenti, n, "Sortare după Notă (descrescător)");

    /* Afișează top 3 */
    printf("\n🏆 Top 3 Studenți:\n");
    for (int i = 0; i < 3 && i < n; i++) {
        printf("   %d. %s - %.2f\n", i + 1, studenti[i].nume, studenti[i].nota);
    }
    printf("\n");
}

/* =============================================================================
 * PARTEA 4: BSEARCH()
 * =============================================================================
 */

/**
 * Comparator pentru bsearch: cheia este un string (nume de găsit)
 * Notă: În bsearch, primul parametru este întotdeauna CHEIA!
 */
int cmp_cauta_dupa_nume(const void *cheie, const void *element) {
    const char *nume_cautat = (const char *)cheie;
    const Student *student = (const Student *)element;
    return strcmp(nume_cautat, student->nume);
}

/**
 * Comparator pentru bsearch: cheia este un int (ID de găsit)
 */
int cmp_cauta_dupa_id(const void *cheie, const void *element) {
    int id_cautat = *(const int *)cheie;
    const Student *student = (const Student *)element;
    return id_cautat - student->id;
}

void demo_bsearch(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 4: BSEARCH()                                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    Student studenti[] = {
        {101, "Georgescu Ana", 7.80},
        {102, "Ionescu Maria", 9.25},
        {103, "Marinescu Elena", 8.90},
        {104, "Popescu Alexandru", 8.50},
        {105, "Vasilescu Dan", 9.50}
    };
    int n = sizeof(studenti) / sizeof(studenti[0]);

    /* Array-ul TREBUIE să fie sortat după cheia de căutare! */
    qsort(studenti, n, sizeof(Student), cmp_dupa_nume);
    afiseaza_studenti(studenti, n, "Sortat după nume (necesar pentru bsearch)");

    /* Caută studenți */
    printf("\nCăutare după nume:\n");
    const char *nume_de_gasit[] = {"Ionescu Maria", "Popescu Alexandru", "Persoană Necunoscută"};

    for (int i = 0; i < 3; i++) {
        Student *gasit = (Student *)bsearch(
            nume_de_gasit[i],
            studenti,
            n,
            sizeof(Student),
            cmp_cauta_dupa_nume
        );

        if (gasit) {
            printf("  ✓ Găsit '%s': ID=%d, Nota=%.2f\n",
                   nume_de_gasit[i], gasit->id, gasit->nota);
        } else {
            printf("  ✗ '%s' nu a fost găsit\n", nume_de_gasit[i]);
        }
    }

    /* Căutare după ID (trebuie re-sortat!) */
    printf("\nCăutare după ID:\n");
    qsort(studenti, n, sizeof(Student), cmp_dupa_id);

    int id_de_gasit[] = {103, 101, 999};
    for (int i = 0; i < 3; i++) {
        Student *gasit = (Student *)bsearch(
            &id_de_gasit[i],
            studenti,
            n,
            sizeof(Student),
            cmp_cauta_dupa_id
        );

        if (gasit) {
            printf("  ✓ Găsit ID %d: %s, Nota=%.2f\n",
                   id_de_gasit[i], gasit->nume, gasit->nota);
        } else {
            printf("  ✗ ID %d nu a fost găsit\n", id_de_gasit[i]);
        }
    }
    printf("\n");
}

/* =============================================================================
 * PARTEA 5: DISPATCH TABLE
 * =============================================================================
 */

/**
 * Dispatch table: array de pointeri la funcții indexat după caracterul operator
 */
static OperatieBinara dispatch_table[256] = {NULL};

/**
 * Inițializează dispatch table-ul
 */
void initializeaza_dispatch_table(void) {
    dispatch_table['+'] = aduna;
    dispatch_table['-'] = scade;
    dispatch_table['*'] = inmulteste;
    dispatch_table['/'] = imparte_sigur;
    dispatch_table['%'] = modulo;
}

/**
 * Calculează folosind dispatch table (fără instrucțiune switch!)
 */
int calculeaza(int a, char op, int b) {
    if (dispatch_table[(unsigned char)op] != NULL) {
        return dispatch_table[(unsigned char)op](a, b);
    }
    fprintf(stderr, "Operator necunoscut: '%c'\n", op);
    return 0;
}

void demo_dispatch_table(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 5: DISPATCH TABLE                                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    initializeaza_dispatch_table();

    printf("Calculator folosind dispatch table:\n");
    printf("  15 + 4 = %d\n", calculeaza(15, '+', 4));
    printf("  15 - 4 = %d\n", calculeaza(15, '-', 4));
    printf("  15 * 4 = %d\n", calculeaza(15, '*', 4));
    printf("  15 / 4 = %d\n", calculeaza(15, '/', 4));
    printf("  15 %% 4 = %d\n", calculeaza(15, '%', 4));
    printf("  15 ^ 4 = %d (operator necunoscut)\n", calculeaza(15, '^', 4));

    printf("\nAvantajele dispatch tables:\n");
    printf("  • Căutare O(1) (acces direct în array)\n");
    printf("  • Ușor de adăugat/eliminat operații\n");
    printf("  • Mai curat decât instrucțiuni switch lungi\n");
    printf("  • Operațiile pot fi modificate la runtime\n");
    printf("\n");
}

/* =============================================================================
 * PARTEA 6: SORTARE ÎNTREGI CU DIVERSE CRITERII
 * =============================================================================
 */

/* Pattern comparator sigur pentru întregi */
int cmp_int_cresc(const void *a, const void *b) {
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    return (ia > ib) - (ia < ib);  /* Sigur: fără overflow */
}

int cmp_int_desc(const void *a, const void *b) {
    return -cmp_int_cresc(a, b);
}

int cmp_int_abs(const void *a, const void *b) {
    int ia = abs(*(const int *)a);
    int ib = abs(*(const int *)b);
    return (ia > ib) - (ia < ib);
}

/* Pare primele, apoi impare */
int cmp_par_impar(const void *a, const void *b) {
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    int par_a = (ia % 2 == 0);
    int par_b = (ib % 2 == 0);

    if (par_a != par_b) {
        return par_b - par_a;  /* Pare primele */
    }
    return ia - ib;  /* Aceeași paritate: sortează după valoare */
}

void afiseaza_vector_int(int *arr, int n, const char *titlu) {
    printf("  %s: ", titlu);
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void demo_sortare_intregi(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 6: SORTARE ÎNTREGI CU DIVERSE CRITERII           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    int original[] = {-7, 3, -2, 8, 1, -5, 6, 4, -9, 2};
    int n = sizeof(original) / sizeof(original[0]);
    int arr[10];

    afiseaza_vector_int(original, n, "Original       ");
    printf("\n");

    memcpy(arr, original, sizeof(original));
    qsort(arr, n, sizeof(int), cmp_int_cresc);
    afiseaza_vector_int(arr, n, "Crescător      ");

    memcpy(arr, original, sizeof(original));
    qsort(arr, n, sizeof(int), cmp_int_desc);
    afiseaza_vector_int(arr, n, "Descrescător   ");

    memcpy(arr, original, sizeof(original));
    qsort(arr, n, sizeof(int), cmp_int_abs);
    afiseaza_vector_int(arr, n, "După |valoare| ");

    memcpy(arr, original, sizeof(original));
    qsort(arr, n, sizeof(int), cmp_par_impar);
    afiseaza_vector_int(arr, n, "Pare primele   ");

    printf("\n");
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     SĂPTĂMÂNA 01: POINTERI LA FUNCȚII ȘI CALLBACKS ÎN C      ║\n");
    printf("║                    Exemplu Complet                           ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    demo_pointeri_functii_baza();
    demo_callbacks();
    demo_qsort();
    demo_bsearch();
    demo_dispatch_table();
    demo_sortare_intregi();

    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    DEMONSTRAȚIE COMPLETĂ                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    return 0;
}
