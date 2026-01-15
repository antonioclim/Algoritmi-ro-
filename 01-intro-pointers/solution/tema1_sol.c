/**
 * =============================================================================
 * SOLUȚIE: Tema 1 - Sistem de Filtrare Generică
 * =============================================================================
 * DOAR PENTRU INSTRUCTOR - Nu distribuiți studenților
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =============================================================================
 * STRUCTURA PRODUS
 * =============================================================================
 */

typedef struct {
    int id;
    char nume[50];
    float pret;
    int cantitate;
} Produs;

/* =============================================================================
 * FUNCȚIA GENERICĂ DE FILTRARE
 * =============================================================================
 */

/**
 * Funcție generică de filtrare care funcționează cu orice tip de date
 *
 * @param sursa Array-ul sursă
 * @param dest Array-ul destinație (pre-alocat)
 * @param n Numărul de elemente în sursă
 * @param dim_elem Dimensiunea fiecărui element în octeți
 * @param predicat Funcție care returnează 1 pentru a păstra elementul, 0 pentru a-l exclude
 * @return Numărul de elemente în array-ul destinație
 */
int filtreaza_array(void *sursa, void *dest, int n, size_t dim_elem,
                    int (*predicat)(const void *)) {
    char *ptr_sursa = (char *)sursa;
    char *ptr_dest = (char *)dest;
    int contor = 0;

    for (int i = 0; i < n; i++) {
        void *curent = ptr_sursa + i * dim_elem;

        if (predicat(curent)) {
            memcpy(ptr_dest + contor * dim_elem, curent, dim_elem);
            contor++;
        }
    }

    return contor;
}

/* =============================================================================
 * PREDICATE PENTRU ÎNTREGI
 * =============================================================================
 */

int este_pozitiv(const void *elem) {
    return *(const int *)elem > 0;
}

int este_par(const void *elem) {
    return *(const int *)elem % 2 == 0;
}

int este_mai_mare_decat_10(const void *elem) {
    return *(const int *)elem > 10;
}

int este_negativ(const void *elem) {
    return *(const int *)elem < 0;
}

/* =============================================================================
 * PREDICATE PENTRU PRODUSE
 * =============================================================================
 */

int este_in_stoc(const void *elem) {
    const Produs *p = (const Produs *)elem;
    return p->cantitate > 0;
}

int este_scump(const void *elem) {
    const Produs *p = (const Produs *)elem;
    return p->pret > 100.0f;
}

int stoc_redus(const void *elem) {
    const Produs *p = (const Produs *)elem;
    return p->cantitate > 0 && p->cantitate < 5;
}

int este_ieftin(const void *elem) {
    const Produs *p = (const Produs *)elem;
    return p->pret <= 50.0f;
}

/* =============================================================================
 * FUNCȚII UTILITARE
 * =============================================================================
 */

void afiseaza_vector_int(int *arr, int n, const char *titlu) {
    printf("%s: ", titlu);
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void afiseaza_produse(Produs *produse, int n, const char *titlu) {
    printf("\n%s (%d produse):\n", titlu, n);
    printf("  ┌──────┬────────────────────────┬──────────┬──────────┐\n");
    printf("  │  ID  │ Nume                   │   Preț   │ Cantitate│\n");
    printf("  ├──────┼────────────────────────┼──────────┼──────────┤\n");

    for (int i = 0; i < n; i++) {
        printf("  │ %4d │ %-22s │ %8.2f │ %8d │\n",
               produse[i].id, produse[i].nume,
               produse[i].pret, produse[i].cantitate);
    }

    printf("  └──────┴────────────────────────┴──────────┴──────────┘\n");
}

/* =============================================================================
 * DEMONSTRAȚIE PRINCIPALĂ
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           SISTEM DE FILTRARE GENERICĂ - SOLUȚIE               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    /* ==========================================================
     * PARTEA 1: Filtrare Întregi
     * ==========================================================
     */
    printf("\n─── PARTEA 1: Filtrare Întregi ───\n\n");

    int numere[] = {-5, 12, 3, -8, 7, 0, 15, -2, 9, 4};
    int n = sizeof(numere) / sizeof(numere[0]);
    int filtrate[10];
    int contor;

    afiseaza_vector_int(numere, n, "Original");

    /* Filtrează numere pozitive */
    contor = filtreaza_array(numere, filtrate, n, sizeof(int), este_pozitiv);
    afiseaza_vector_int(filtrate, contor, "Pozitive");

    /* Filtrează numere pare */
    contor = filtreaza_array(numere, filtrate, n, sizeof(int), este_par);
    afiseaza_vector_int(filtrate, contor, "Pare    ");

    /* Filtrează > 10 */
    contor = filtreaza_array(numere, filtrate, n, sizeof(int), este_mai_mare_decat_10);
    afiseaza_vector_int(filtrate, contor, "> 10    ");

    /* Filtrează negative */
    contor = filtreaza_array(numere, filtrate, n, sizeof(int), este_negativ);
    afiseaza_vector_int(filtrate, contor, "Negative");

    /* ==========================================================
     * PARTEA 2: Filtrare Produse
     * ==========================================================
     */
    printf("\n─── PARTEA 2: Filtrare Produse ───\n");

    Produs produse[] = {
        {1, "Laptop ASUS",      2500.00, 10},
        {2, "Mouse Logitech",   45.00,   50},
        {3, "Tastatură Corsair", 150.00,  0},
        {4, "Monitor LG",       800.00,  3},
        {5, "Cablu USB",        15.00,   100},
        {6, "Webcam HD",        120.00,  2},
        {7, "Mousepad XL",      25.00,   0},
        {8, "Căști Sony",       350.00,  4}
    };
    int nr_produse = sizeof(produse) / sizeof(produse[0]);
    Produs produse_filtrate[10];

    afiseaza_produse(produse, nr_produse, "Toate Produsele");

    /* Filtrează în stoc */
    contor = filtreaza_array(produse, produse_filtrate, nr_produse,
                             sizeof(Produs), este_in_stoc);
    afiseaza_produse(produse_filtrate, contor, "În Stoc");

    /* Filtrează scumpe (> 100) */
    contor = filtreaza_array(produse, produse_filtrate, nr_produse,
                             sizeof(Produs), este_scump);
    afiseaza_produse(produse_filtrate, contor, "Scumpe (> 100)");

    /* Filtrează stoc redus */
    contor = filtreaza_array(produse, produse_filtrate, nr_produse,
                             sizeof(Produs), stoc_redus);
    afiseaza_produse(produse_filtrate, contor, "Stoc Redus (< 5)");

    /* Filtrează ieftine (<= 50) */
    contor = filtreaza_array(produse, produse_filtrate, nr_produse,
                             sizeof(Produs), este_ieftin);
    afiseaza_produse(produse_filtrate, contor, "Ieftine (<= 50)");

    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    DEMONSTRAȚIE COMPLETĂ                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    return 0;
}
