/**
 * =============================================================================
 * SOLUȚIE - TEMA 2: Editor de Text Simplificat
 * =============================================================================
 *
 * ATENȚIE: Acest fișier conține soluția completă.
 * Pentru instructori și pentru auto-verificare.
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -g -o homework2_sol homework2_sol.c
 * Verificare: valgrind --leak-check=full ./homework2_sol
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =============================================================================
 * DEFINIREA STRUCTURILOR
 * =============================================================================
 */

#define MAX_LINE_LENGTH 256

typedef struct Linie {
    int numar_linie;
    char text[MAX_LINE_LENGTH];
    struct Linie *prev;
    struct Linie *next;
} Linie;

typedef struct {
    Linie *head;
    Linie *tail;
    Linie *cursor;
    int total_linii;
} Document;

/* =============================================================================
 * FUNCȚII DE INIȚIALIZARE ȘI ELIBERARE
 * =============================================================================
 */

/**
 * Inițializează un document gol.
 */
void document_init(Document *doc) {
    doc->head = NULL;
    doc->tail = NULL;
    doc->cursor = NULL;
    doc->total_linii = 0;
}

/**
 * Creează o linie nouă.
 */
Linie *create_linie(const char *text, int numar) {
    Linie *new_line = (Linie *)malloc(sizeof(Linie));
    
    if (new_line == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        return NULL;
    }
    
    new_line->numar_linie = numar;
    strncpy(new_line->text, text, MAX_LINE_LENGTH - 1);
    new_line->text[MAX_LINE_LENGTH - 1] = '\0';
    new_line->prev = NULL;
    new_line->next = NULL;
    
    return new_line;
}

/**
 * Eliberează toată memoria documentului.
 */
void document_free(Document *doc) {
    Linie *current = doc->head;
    Linie *next;
    
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    
    doc->head = NULL;
    doc->tail = NULL;
    doc->cursor = NULL;
    doc->total_linii = 0;
}

/* =============================================================================
 * CERINȚA 7: renumeroteaza (8p)
 * =============================================================================
 */

void renumeroteaza(Document *doc) {
    Linie *current = doc->head;
    int numar = 1;
    
    while (current != NULL) {
        current->numar_linie = numar++;
        current = current->next;
    }
}

/* =============================================================================
 * CERINȚA 1: insereaza_linie (8p)
 * =============================================================================
 */

void insereaza_linie(Document *doc, const char *text) {
    Linie *new_line = create_linie(text, doc->total_linii + 1);
    if (new_line == NULL) return;
    
    if (doc->head == NULL) {
        /* Document gol */
        doc->head = new_line;
        doc->tail = new_line;
        doc->cursor = new_line;
    } else if (doc->cursor == NULL) {
        /* Cursor invalid, inserăm la început */
        new_line->next = doc->head;
        doc->head->prev = new_line;
        doc->head = new_line;
        doc->cursor = new_line;
    } else {
        /* Inserăm după cursor */
        new_line->next = doc->cursor->next;
        new_line->prev = doc->cursor;
        
        if (doc->cursor->next != NULL) {
            doc->cursor->next->prev = new_line;
        } else {
            doc->tail = new_line;
        }
        
        doc->cursor->next = new_line;
        doc->cursor = new_line;  /* Cursor-ul se mută pe noua linie */
    }
    
    doc->total_linii++;
    renumeroteaza(doc);
}

/* =============================================================================
 * CERINȚA 2: sterge_linie (8p)
 * =============================================================================
 */

int sterge_linie(Document *doc) {
    if (doc->cursor == NULL) {
        printf("Nu există linie de șters.\n");
        return 0;
    }
    
    Linie *to_delete = doc->cursor;
    
    /* Actualizăm legăturile */
    if (to_delete->prev != NULL) {
        to_delete->prev->next = to_delete->next;
    } else {
        doc->head = to_delete->next;
    }
    
    if (to_delete->next != NULL) {
        to_delete->next->prev = to_delete->prev;
    } else {
        doc->tail = to_delete->prev;
    }
    
    /* Actualizăm cursor-ul */
    if (to_delete->next != NULL) {
        doc->cursor = to_delete->next;
    } else {
        doc->cursor = to_delete->prev;
    }
    
    printf("✓ Linia %d a fost ștearsă.\n", to_delete->numar_linie);
    
    free(to_delete);
    doc->total_linii--;
    renumeroteaza(doc);
    
    return 1;
}

/* =============================================================================
 * CERINȚA 3: modifica_linie (7p)
 * =============================================================================
 */

int modifica_linie(Document *doc, const char *text_nou) {
    if (doc->cursor == NULL) {
        printf("Nu există linie de modificat.\n");
        return 0;
    }
    
    strncpy(doc->cursor->text, text_nou, MAX_LINE_LENGTH - 1);
    doc->cursor->text[MAX_LINE_LENGTH - 1] = '\0';
    
    printf("✓ Linia %d a fost modificată.\n", doc->cursor->numar_linie);
    return 1;
}

/* =============================================================================
 * CERINȚA 4: muta_cursor (7p)
 * =============================================================================
 */

Linie *muta_cursor(Document *doc, int n) {
    if (doc->cursor == NULL) {
        printf("Documentul este gol.\n");
        return NULL;
    }
    
    if (n > 0) {
        /* Mută în jos */
        while (n > 0 && doc->cursor->next != NULL) {
            doc->cursor = doc->cursor->next;
            n--;
        }
    } else {
        /* Mută în sus */
        while (n < 0 && doc->cursor->prev != NULL) {
            doc->cursor = doc->cursor->prev;
            n++;
        }
    }
    
    printf("→ Cursor pe linia %d\n", doc->cursor->numar_linie);
    return doc->cursor;
}

/* =============================================================================
 * CERINȚA 5: cauta_text (7p)
 * =============================================================================
 */

Linie *cauta_text(Document *doc, const char *text) {
    Linie *current = doc->head;
    
    while (current != NULL) {
        if (strstr(current->text, text) != NULL) {
            doc->cursor = current;
            printf("✓ Text găsit pe linia %d.\n", current->numar_linie);
            return current;
        }
        current = current->next;
    }
    
    printf("✗ Textul \"%s\" nu a fost găsit.\n", text);
    return NULL;
}

/* =============================================================================
 * CERINȚA 6: afiseaza_document (5p)
 * =============================================================================
 */

void afiseaza_document(Document *doc) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                      DOCUMENT                                 ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    
    if (doc->head == NULL) {
        printf("║                   (Document gol)                              ║\n");
    } else {
        Linie *current = doc->head;
        while (current != NULL) {
            const char *marker = (current == doc->cursor) ? "▶" : " ";
            printf("║ %s %3d │ %-55.55s ║\n",
                   marker, current->numar_linie, current->text);
            current = current->next;
        }
    }
    
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║ Total linii: %-3d                                             ║\n",
           doc->total_linii);
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
}

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

/**
 * Mergi la prima linie.
 */
void goto_start(Document *doc) {
    if (doc->head != NULL) {
        doc->cursor = doc->head;
        printf("→ Cursor la începutul documentului (linia %d)\n",
               doc->cursor->numar_linie);
    }
}

/**
 * Mergi la ultima linie.
 */
void goto_end(Document *doc) {
    if (doc->tail != NULL) {
        doc->cursor = doc->tail;
        printf("→ Cursor la sfârșitul documentului (linia %d)\n",
               doc->cursor->numar_linie);
    }
}

/**
 * Afișează linia curentă.
 */
void afiseaza_linia_curenta(Document *doc) {
    if (doc->cursor == NULL) {
        printf("Nu există linie curentă.\n");
        return;
    }
    
    printf("Linia %d: %s\n", doc->cursor->numar_linie, doc->cursor->text);
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    Document doc;
    document_init(&doc);
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     TEMA 2: Editor de Text Simplificat - SOLUȚIE              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Inserăm câteva linii */
    printf("═══ Inserare linii ═══\n\n");
    insereaza_linie(&doc, "Prima linie a documentului.");
    insereaza_linie(&doc, "A doua linie cu text important.");
    insereaza_linie(&doc, "A treia linie continuă povestea.");
    insereaza_linie(&doc, "A patra linie adaugă detalii.");
    insereaza_linie(&doc, "Ultima linie încheie documentul.");
    
    afiseaza_document(&doc);
    
    /* Testăm navigarea */
    printf("\n═══ Testare navigare ═══\n\n");
    
    goto_start(&doc);
    afiseaza_linia_curenta(&doc);
    
    printf("\nMutăm cursor-ul în jos cu 2 poziții:\n");
    muta_cursor(&doc, 2);
    afiseaza_linia_curenta(&doc);
    
    printf("\nMutăm cursor-ul în sus cu 1 poziție:\n");
    muta_cursor(&doc, -1);
    afiseaza_linia_curenta(&doc);
    
    /* Testăm modificarea */
    printf("\n═══ Testare modificare ═══\n\n");
    modifica_linie(&doc, "LINIA 2 MODIFICATĂ: Text complet nou!");
    afiseaza_document(&doc);
    
    /* Testăm căutarea */
    printf("\n═══ Testare căutare ═══\n\n");
    cauta_text(&doc, "detalii");
    afiseaza_linia_curenta(&doc);
    
    cauta_text(&doc, "inexistent");
    
    /* Testăm ștergerea */
    printf("\n═══ Testare ștergere ═══\n\n");
    
    goto_start(&doc);
    printf("Ștergem prima linie:\n");
    sterge_linie(&doc);
    afiseaza_document(&doc);
    
    goto_end(&doc);
    printf("\nȘtergem ultima linie:\n");
    sterge_linie(&doc);
    afiseaza_document(&doc);
    
    /* Inserăm din nou pentru test */
    printf("\n═══ Inserare după ștergere ═══\n\n");
    muta_cursor(&doc, 1);  /* Mergem pe linia 2 */
    insereaza_linie(&doc, "O nouă linie inserată la mijloc!");
    afiseaza_document(&doc);
    
    /* Eliberăm memoria */
    document_free(&doc);
    printf("\n✓ Memoria a fost eliberată corect.\n");
    
    return 0;
}
