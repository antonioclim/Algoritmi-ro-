/**
 * =============================================================================
 * EXERCIȚIUL 1: Calculator cu Dispatch Table
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementează un calculator care folosește pointeri la funcții și un
 *   dispatch table în loc de instrucțiuni switch.
 *
 * CERINȚE:
 *   1. Implementează cele 5 operații aritmetice (+, -, *, /, %)
 *   2. Gestionează împărțirea la zero cu grație
 *   3. Inițializează un dispatch table cu pointeri la funcții
 *   4. Citește operații de la stdin în format: "operand1 operator operand2"
 *   5. Procesează până la EOF sau input invalid
 *
 * EXEMPLU INPUT:
 *   10 + 5
 *   20 - 8
 *   7 * 3
 *   15 / 4
 *   17 % 5
 *   10 / 0
 *
 * OUTPUT AȘTEPTAT:
 *   10 + 5 = 15
 *   20 - 8 = 12
 *   7 * 3 = 21
 *   15 / 4 = 3
 *   17 % 5 = 2
 *   Eroare: Împărțire la zero
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

/* =============================================================================
 * DEFINIȚII DE TIP
 * =============================================================================
 */

/**
 * TODO 1: Definește un typedef pentru un tip pointer la funcție
 *
 * Pointerul la funcție trebuie să:
 *   - Primească doi parametri int
 *   - Returneze un int
 *   - Fie numit 'Operatie'
 *
 * Hint: typedef tip_returnat (*NumeTip)(tipuri_parametri);
 */

/* CODUL TĂU AICI */


/* =============================================================================
 * OPERAȚII ARITMETICE
 * =============================================================================
 */

/**
 * TODO 2: Implementează funcția de adunare
 *
 * @param a Primul operand
 * @param b Al doilea operand
 * @return Suma lui a și b
 */
int op_aduna(int a, int b) {
    /* CODUL TĂU AICI */
    return 0;  /* Înlocuiește aceasta */
}

/**
 * TODO 3: Implementează funcția de scădere
 */
int op_scade(int a, int b) {
    /* CODUL TĂU AICI */
    return 0;  /* Înlocuiește aceasta */
}

/**
 * TODO 4: Implementează funcția de înmulțire
 */
int op_inmulteste(int a, int b) {
    /* CODUL TĂU AICI */
    return 0;  /* Înlocuiește aceasta */
}

/**
 * TODO 5: Implementează împărțirea sigură
 *
 * IMPORTANT: Gestionează împărțirea la zero!
 * Dacă b este 0, afișează "Eroare: Împărțire la zero\n" la stderr
 * și returnează 0 (sau folosește un indicator special de eroare)
 *
 * Pentru acest exercițiu, vom folosi un flag global pentru a indica erori.
 */
static int eroare_impartire = 0;  /* Flag global de eroare */

int op_imparte(int a, int b) {
    /* CODUL TĂU AICI */
    /* Amintește-ți să setezi eroare_impartire = 1 dacă b == 0 */
    return 0;  /* Înlocuiește aceasta */
}

/**
 * TODO 6: Implementează operația modulo cu gestionare erori
 */
int op_modulo(int a, int b) {
    /* CODUL TĂU AICI */
    /* Amintește-ți să gestionezi împărțirea la zero ca op_imparte */
    return 0;  /* Înlocuiește aceasta */
}


/* =============================================================================
 * DISPATCH TABLE
 * =============================================================================
 */

/**
 * Dispatch table-ul: un array de pointeri la funcții indexat după
 * valoarea ASCII a caracterului operator.
 *
 * Folosim dimensiunea 256 pentru a acoperi toate valorile posibile char.
 * Majoritatea intrărilor vor fi NULL (operatori nefolosiți).
 */

/* TODO 7: Declară dispatch table-ul
 *
 * Hint: Operatie dispatch[256] = {NULL};
 * sau:  Operatie dispatch[256] = {0};
 */

/* CODUL TĂU AICI */


/**
 * TODO 8: Inițializează dispatch table-ul
 *
 * Mapează fiecare caracter operator la funcția corespunzătoare:
 *   '+' -> op_aduna
 *   '-' -> op_scade
 *   '*' -> op_inmulteste
 *   '/' -> op_imparte
 *   '%' -> op_modulo
 *
 * Hint: dispatch['+'] = op_aduna;
 */
void initializeaza_dispatch_table(void) {
    /* CODUL TĂU AICI */

}


/**
 * TODO 9: Implementează funcția de calcul
 *
 * Folosește dispatch table-ul pentru a efectua operația.
 *
 * @param a Primul operand
 * @param op Caracterul operator
 * @param b Al doilea operand
 * @param eroare Pointer pentru a stoca statusul erorii (1 dacă eroare, 0 altfel)
 * @return Rezultatul operației, sau 0 dacă operatorul e necunoscut
 *
 * Pași:
 *   1. Resetează flag-ul global eroare_impartire
 *   2. Verifică dacă dispatch[(unsigned char)op] nu este NULL
 *   3. Dacă e valid, apelează funcția și returnează rezultatul
 *   4. Dacă e NULL, setează *eroare = 1 și afișează "Eroare: Operator necunoscut"
 *   5. După apel, verifică eroare_impartire și setează *eroare corespunzător
 */
int calculeaza(int a, char op, int b, int *eroare) {
    *eroare = 0;
    eroare_impartire = 0;

    /* CODUL TĂU AICI */

    return 0;  /* Înlocuiește aceasta */
}


/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    int a, b;
    char op;
    int eroare;

    /* Inițializează dispatch table-ul */
    initializeaza_dispatch_table();

    printf("Calculator cu Dispatch Table\n");
    printf("Introduceți operații (ex: '10 + 5'), Ctrl+D pentru ieșire:\n");
    printf("─────────────────────────────────────────────────\n");

    /* Citește și procesează operații până la EOF */
    while (scanf("%d %c %d", &a, &op, &b) == 3) {

        /* TODO 10: Apelează calculeaza() și gestionează rezultatul
         *
         * Pași:
         *   1. Apelează calculeaza(a, op, b, &eroare)
         *   2. Dacă nu e eroare, afișează: "a op b = rezultat\n"
         *   3. Dacă e eroare, mesajul de eroare a fost deja afișat
         *
         * Folosește printf("%d %c %d = %d\n", a, op, b, rezultat);
         */

        /* CODUL TĂU AICI */

    }

    printf("─────────────────────────────────────────────────\n");
    printf("Calculator terminat.\n");

    return 0;
}


/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Adaugă operația de ridicare la putere ('^') folosind o buclă sau recursie
 *    Notă: Nu poți folosi pow() fără flag-ul -lm
 *
 * 2. Adaugă operații pe biți: & (AND), | (OR), ~ (XOR)
 *
 * 3. Implementează o funcție de istoric care stochează ultimele N calcule
 *
 * 4. Adaugă suport pentru paranteze folosind un parser simplu de expresii
 *
 * =============================================================================
 */
