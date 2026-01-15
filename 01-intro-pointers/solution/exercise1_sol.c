/**
 * =============================================================================
 * SOLUȚIE: Exercițiul 1 - Calculator cu Dispatch Table
 * =============================================================================
 * DOAR PENTRU INSTRUCTOR - Nu distribuiți studenților
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

/* Definiție de tip pentru pointer la funcție de operație */
typedef int (*Operatie)(int, int);

/* Flag global de eroare pentru operațiile de împărțire */
static int eroare_impartire = 0;

/* Operații aritmetice */
int op_aduna(int a, int b) {
    return a + b;
}

int op_scade(int a, int b) {
    return a - b;
}

int op_inmulteste(int a, int b) {
    return a * b;
}

int op_imparte(int a, int b) {
    if (b == 0) {
        fprintf(stderr, "Eroare: Împărțire la zero\n");
        eroare_impartire = 1;
        return 0;
    }
    eroare_impartire = 0;
    return a / b;
}

int op_modulo(int a, int b) {
    if (b == 0) {
        fprintf(stderr, "Eroare: Împărțire la zero\n");
        eroare_impartire = 1;
        return 0;
    }
    eroare_impartire = 0;
    return a % b;
}

/* Dispatch table */
static Operatie dispatch[256] = {NULL};

void initializeaza_dispatch_table(void) {
    dispatch['+'] = op_aduna;
    dispatch['-'] = op_scade;
    dispatch['*'] = op_inmulteste;
    dispatch['/'] = op_imparte;
    dispatch['%'] = op_modulo;
}

int calculeaza(int a, char op, int b, int *eroare) {
    *eroare = 0;
    eroare_impartire = 0;

    if (dispatch[(unsigned char)op] == NULL) {
        fprintf(stderr, "Eroare: Operator necunoscut '%c'\n", op);
        *eroare = 1;
        return 0;
    }

    int rezultat = dispatch[(unsigned char)op](a, b);

    if (eroare_impartire) {
        *eroare = 1;
    }

    return rezultat;
}

int main(void) {
    int a, b;
    char op;
    int eroare;

    initializeaza_dispatch_table();

    printf("Calculator cu Dispatch Table\n");
    printf("Introduceți operații (ex: '10 + 5'), Ctrl+D pentru ieșire:\n");
    printf("─────────────────────────────────────────────────\n");

    while (scanf("%d %c %d", &a, &op, &b) == 3) {
        int rezultat = calculeaza(a, op, b, &eroare);

        if (!eroare) {
            printf("%d %c %d = %d\n", a, op, b, rezultat);
        }
    }

    printf("─────────────────────────────────────────────────\n");
    printf("Calculator terminat.\n");

    return 0;
}
