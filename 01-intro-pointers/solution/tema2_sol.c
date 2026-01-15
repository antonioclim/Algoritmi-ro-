/**
 * =============================================================================
 * SOLUȚIE: Tema 2 - Parser de Expresii
 * =============================================================================
 * DOAR PENTRU INSTRUCTOR - Nu distribuiți studenților
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

/* =============================================================================
 * DEFINIȚII DE TIP
 * =============================================================================
 */

typedef struct {
    int operand1;
    char operator;
    int operand2;
} Expresie;

typedef int (*Operatie)(int, int, int *eroare);

/* =============================================================================
 * OPERAȚII ARITMETICE
 * =============================================================================
 */

int op_aduna(int a, int b, int *eroare) {
    *eroare = 0;
    /* Verificare overflow */
    if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b)) {
        fprintf(stderr, "Eroare: Overflow la adunare\n");
        *eroare = 1;
        return 0;
    }
    return a + b;
}

int op_scade(int a, int b, int *eroare) {
    *eroare = 0;
    /* Verificare overflow */
    if ((b < 0 && a > INT_MAX + b) || (b > 0 && a < INT_MIN + b)) {
        fprintf(stderr, "Eroare: Overflow la scădere\n");
        *eroare = 1;
        return 0;
    }
    return a - b;
}

int op_inmulteste(int a, int b, int *eroare) {
    *eroare = 0;
    /* Verificare overflow */
    if (a > 0 && b > 0 && a > INT_MAX / b) {
        fprintf(stderr, "Eroare: Overflow la înmulțire\n");
        *eroare = 1;
        return 0;
    }
    if (a < 0 && b < 0 && a < INT_MAX / b) {
        fprintf(stderr, "Eroare: Overflow la înmulțire\n");
        *eroare = 1;
        return 0;
    }
    if (a > 0 && b < 0 && b < INT_MIN / a) {
        fprintf(stderr, "Eroare: Overflow la înmulțire\n");
        *eroare = 1;
        return 0;
    }
    if (a < 0 && b > 0 && a < INT_MIN / b) {
        fprintf(stderr, "Eroare: Overflow la înmulțire\n");
        *eroare = 1;
        return 0;
    }
    return a * b;
}

int op_imparte(int a, int b, int *eroare) {
    *eroare = 0;
    if (b == 0) {
        fprintf(stderr, "Eroare: Împărțire la zero\n");
        *eroare = 1;
        return 0;
    }
    /* Verificare overflow: INT_MIN / -1 */
    if (a == INT_MIN && b == -1) {
        fprintf(stderr, "Eroare: Overflow la împărțire\n");
        *eroare = 1;
        return 0;
    }
    return a / b;
}

int op_modulo(int a, int b, int *eroare) {
    *eroare = 0;
    if (b == 0) {
        fprintf(stderr, "Eroare: Modulo cu zero\n");
        *eroare = 1;
        return 0;
    }
    return a % b;
}

/**
 * Funcția de putere (iterativă pentru a evita stack overflow)
 */
int op_putere(int baza, int exp, int *eroare) {
    *eroare = 0;

    if (exp < 0) {
        fprintf(stderr, "Eroare: Exponent negativ nu este suportat\n");
        *eroare = 1;
        return 0;
    }

    if (exp == 0) {
        return 1;
    }

    long long rezultat = 1;
    long long b = baza;

    while (exp > 0) {
        if (exp % 2 == 1) {
            rezultat *= b;
            if (rezultat > INT_MAX || rezultat < INT_MIN) {
                fprintf(stderr, "Eroare: Overflow la putere\n");
                *eroare = 1;
                return 0;
            }
        }
        exp /= 2;
        if (exp > 0) {
            b *= b;
            if (b > INT_MAX || b < INT_MIN) {
                fprintf(stderr, "Eroare: Overflow la putere\n");
                *eroare = 1;
                return 0;
            }
        }
    }

    return (int)rezultat;
}

/* =============================================================================
 * DISPATCH TABLE
 * =============================================================================
 */

static Operatie dispatch[256] = {NULL};

void initializeaza_dispatch_table(void) {
    dispatch['+'] = op_aduna;
    dispatch['-'] = op_scade;
    dispatch['*'] = op_inmulteste;
    dispatch['/'] = op_imparte;
    dispatch['%'] = op_modulo;
    dispatch['^'] = op_putere;
}

/* =============================================================================
 * PARSERUL DE EXPRESII
 * =============================================================================
 */

/**
 * Sari peste spațiile albe dintr-un string
 */
const char* sari_spatii(const char *str) {
    while (*str && isspace((unsigned char)*str)) {
        str++;
    }
    return str;
}

/**
 * Parsează un întreg din string
 * Returnează pointer la caracterul de după număr, sau NULL la eroare
 */
const char* parseaza_int(const char *str, int *valoare) {
    str = sari_spatii(str);

    if (*str == '\0') {
        return NULL;
    }

    char *endptr;
    long val = strtol(str, &endptr, 10);

    if (endptr == str) {
        return NULL;  /* Nu s-au găsit cifre */
    }

    if (val > INT_MAX || val < INT_MIN) {
        fprintf(stderr, "Eroare: Număr prea mare\n");
        return NULL;
    }

    *valoare = (int)val;
    return endptr;
}

/**
 * Parsează o expresie din string
 * Format: "operand1 operator operand2"
 *
 * @param sir Stringul de input
 * @param expr Structura de output pentru expresie
 * @return 1 la succes, 0 la eroare
 */
int parseaza_expresie(const char *sir, Expresie *expr) {
    const char *ptr = sir;

    /* Parsează primul operand */
    ptr = parseaza_int(ptr, &expr->operand1);
    if (ptr == NULL) {
        fprintf(stderr, "Eroare: Nu pot parsa primul operand\n");
        return 0;
    }

    /* Sari spații și ia operatorul */
    ptr = sari_spatii(ptr);
    if (*ptr == '\0') {
        fprintf(stderr, "Eroare: Lipsește operatorul\n");
        return 0;
    }

    expr->operator = *ptr;
    ptr++;

    /* Parsează al doilea operand */
    ptr = parseaza_int(ptr, &expr->operand2);
    if (ptr == NULL) {
        fprintf(stderr, "Eroare: Nu pot parsa al doilea operand\n");
        return 0;
    }

    /* Verifică pentru caractere extra */
    ptr = sari_spatii(ptr);
    if (*ptr != '\0') {
        fprintf(stderr, "Eroare: Caractere extra după expresie: '%s'\n", ptr);
        return 0;
    }

    return 1;
}

/* =============================================================================
 * EVALUATORUL DE EXPRESII
 * =============================================================================
 */

/**
 * Evaluează o expresie
 *
 * @param expr Expresia de evaluat
 * @param rezultat Pointer pentru a stoca rezultatul
 * @return 1 la succes, 0 la eroare
 */
int evalueaza(const Expresie *expr, int *rezultat) {
    Operatie op = dispatch[(unsigned char)expr->operator];

    if (op == NULL) {
        fprintf(stderr, "Eroare: Operator necunoscut '%c'\n", expr->operator);
        return 0;
    }

    int eroare;
    *rezultat = op(expr->operand1, expr->operand2, &eroare);

    return !eroare;
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    char linie[256];
    Expresie expr;
    int rezultat;

    initializeaza_dispatch_table();

    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║              CALCULATOR DE EXPRESII - SOLUȚIE                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    printf("Operatori disponibili: + - * / %% ^\n");
    printf("Introduceți expresii (ex: '5 + 3'), 'exit' pentru ieșire.\n");
    printf("─────────────────────────────────────────────────────────────────\n");

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(linie, sizeof(linie), stdin) == NULL) {
            printf("\n");
            break;  /* EOF */
        }

        /* Elimină newline-ul */
        linie[strcspn(linie, "\n")] = '\0';

        /* Verifică comanda de ieșire */
        if (strcmp(linie, "exit") == 0 || strcmp(linie, "iesire") == 0) {
            break;
        }

        /* Sari liniile goale */
        if (strlen(linie) == 0) {
            continue;
        }

        /* Parsează și evaluează */
        if (parseaza_expresie(linie, &expr)) {
            if (evalueaza(&expr, &rezultat)) {
                printf("%d %c %d = %d\n", expr.operand1, expr.operator,
                       expr.operand2, rezultat);
            }
        }
    }

    printf("─────────────────────────────────────────────────────────────────\n");
    printf("La revedere!\n");

    return 0;
}
