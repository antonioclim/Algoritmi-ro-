/**
 * =============================================================================
 * SOLUȚIE - TEMA 1: Sistem de Gestiune Bibliotecă
 * =============================================================================
 *
 * ATENȚIE: Acest fișier conține soluția completă.
 * Pentru instructori și pentru auto-verificare.
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -g -o homework1_sol homework1_sol.c
 * Verificare: valgrind --leak-check=full ./homework1_sol
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

typedef struct Carte {
    int id;
    char titlu[100];
    char autor[50];
    int an_publicare;
    int este_imprumutata;      /* 0 = disponibilă, 1 = împrumutată */
    char imprumutat_de[50];    /* Numele cititorului */
    struct Carte *next;
} Carte;

/* =============================================================================
 * FUNCȚII DE CREARE ȘI ELIBERARE
 * =============================================================================
 */

/**
 * Creează un nod nou pentru o carte.
 */
Carte *create_carte(int id, const char *titlu, const char *autor, int an) {
    Carte *new_carte = (Carte *)malloc(sizeof(Carte));
    
    if (new_carte == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        return NULL;
    }
    
    new_carte->id = id;
    strncpy(new_carte->titlu, titlu, 99);
    new_carte->titlu[99] = '\0';
    strncpy(new_carte->autor, autor, 49);
    new_carte->autor[49] = '\0';
    new_carte->an_publicare = an;
    new_carte->este_imprumutata = 0;
    new_carte->imprumutat_de[0] = '\0';
    new_carte->next = NULL;
    
    return new_carte;
}

/**
 * Eliberează toată memoria bibliotecii.
 */
void free_biblioteca(Carte **biblioteca) {
    Carte *current = *biblioteca;
    Carte *next;
    
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    
    *biblioteca = NULL;
}

/* =============================================================================
 * CERINȚA 1: adauga_carte (8p)
 * =============================================================================
 */

void adauga_carte(Carte **biblioteca, int id, const char *titlu, 
                  const char *autor, int an) {
    Carte *new_carte = create_carte(id, titlu, autor, an);
    if (new_carte == NULL) return;
    
    /* Adăugăm la sfârșitul listei */
    if (*biblioteca == NULL) {
        *biblioteca = new_carte;
    } else {
        Carte *current = *biblioteca;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_carte;
    }
    
    printf("✓ Carte adăugată: \"%s\" de %s (%d)\n", titlu, autor, an);
}

/* =============================================================================
 * CERINȚA 2: cauta_dupa_titlu (7p) - potrivire parțială
 * =============================================================================
 */

void cauta_dupa_titlu(Carte *biblioteca, const char *titlu_partial) {
    int gasit = 0;
    Carte *current = biblioteca;
    
    printf("\nCăutare după titlu: \"%s\"\n", titlu_partial);
    printf("────────────────────────────────────────────\n");
    
    while (current != NULL) {
        /* strstr caută substring-ul în string */
        if (strstr(current->titlu, titlu_partial) != NULL) {
            printf("  ID: %d | \"%s\" de %s (%d) - %s\n",
                   current->id, current->titlu, current->autor,
                   current->an_publicare,
                   current->este_imprumutata ? "ÎMPRUMUTATĂ" : "DISPONIBILĂ");
            gasit++;
        }
        current = current->next;
    }
    
    if (gasit == 0) {
        printf("  Nu s-au găsit cărți cu acest titlu.\n");
    } else {
        printf("────────────────────────────────────────────\n");
        printf("  Total: %d carte(cărți) găsite.\n", gasit);
    }
}

/* =============================================================================
 * CERINȚA 3: cauta_dupa_autor (7p)
 * =============================================================================
 */

void cauta_dupa_autor(Carte *biblioteca, const char *autor) {
    int gasit = 0;
    Carte *current = biblioteca;
    
    printf("\nCărțile autorului: %s\n", autor);
    printf("────────────────────────────────────────────\n");
    
    while (current != NULL) {
        if (strstr(current->autor, autor) != NULL) {
            printf("  ID: %d | \"%s\" (%d) - %s\n",
                   current->id, current->titlu, current->an_publicare,
                   current->este_imprumutata ? "ÎMPRUMUTATĂ" : "DISPONIBILĂ");
            gasit++;
        }
        current = current->next;
    }
    
    if (gasit == 0) {
        printf("  Nu s-au găsit cărți ale acestui autor.\n");
    }
}

/* =============================================================================
 * CERINȚA 4: imprumuta_carte (8p)
 * =============================================================================
 */

int imprumuta_carte(Carte **biblioteca, int id, const char *cititor) {
    Carte *current = *biblioteca;
    
    while (current != NULL) {
        if (current->id == id) {
            if (current->este_imprumutata) {
                printf("✗ Cartea \"%s\" este deja împrumutată de %s.\n",
                       current->titlu, current->imprumutat_de);
                return 0;
            }
            
            current->este_imprumutata = 1;
            strncpy(current->imprumutat_de, cititor, 49);
            current->imprumutat_de[49] = '\0';
            
            printf("✓ Cartea \"%s\" a fost împrumutată de %s.\n",
                   current->titlu, cititor);
            return 1;
        }
        current = current->next;
    }
    
    printf("✗ Cartea cu ID %d nu a fost găsită.\n", id);
    return 0;
}

/* =============================================================================
 * CERINȚA 5: returneaza_carte (7p)
 * =============================================================================
 */

int returneaza_carte(Carte **biblioteca, int id) {
    Carte *current = *biblioteca;
    
    while (current != NULL) {
        if (current->id == id) {
            if (!current->este_imprumutata) {
                printf("✗ Cartea \"%s\" nu era împrumutată.\n", current->titlu);
                return 0;
            }
            
            printf("✓ Cartea \"%s\" a fost returnată de %s.\n",
                   current->titlu, current->imprumutat_de);
            
            current->este_imprumutata = 0;
            current->imprumutat_de[0] = '\0';
            return 1;
        }
        current = current->next;
    }
    
    printf("✗ Cartea cu ID %d nu a fost găsită.\n", id);
    return 0;
}

/* =============================================================================
 * CERINȚA 6: afiseaza_disponibile (5p)
 * =============================================================================
 */

void afiseaza_disponibile(Carte *biblioteca) {
    int count = 0;
    Carte *current = biblioteca;
    
    printf("\n╔═════════════════════════════════════════════════════════════╗\n");
    printf("║              CĂRȚI DISPONIBILE                              ║\n");
    printf("╠═════════════════════════════════════════════════════════════╣\n");
    
    while (current != NULL) {
        if (!current->este_imprumutata) {
            printf("║ %3d | %-30s | %-15s | %4d ║\n",
                   current->id, current->titlu, current->autor,
                   current->an_publicare);
            count++;
        }
        current = current->next;
    }
    
    printf("╠═════════════════════════════════════════════════════════════╣\n");
    printf("║ Total cărți disponibile: %-33d ║\n", count);
    printf("╚═════════════════════════════════════════════════════════════╝\n");
}

/* =============================================================================
 * CERINȚA 7: sterge_carte (8p)
 * =============================================================================
 */

int sterge_carte(Carte **biblioteca, int id) {
    if (*biblioteca == NULL) {
        printf("✗ Biblioteca este goală.\n");
        return 0;
    }
    
    /* Caz special: ștergem prima carte */
    if ((*biblioteca)->id == id) {
        Carte *temp = *biblioteca;
        printf("✓ Cartea \"%s\" a fost ștearsă din sistem.\n", temp->titlu);
        *biblioteca = (*biblioteca)->next;
        free(temp);
        return 1;
    }
    
    /* Căutăm cartea */
    Carte *current = *biblioteca;
    while (current->next != NULL && current->next->id != id) {
        current = current->next;
    }
    
    if (current->next != NULL) {
        Carte *temp = current->next;
        printf("✓ Cartea \"%s\" a fost ștearsă din sistem.\n", temp->titlu);
        current->next = temp->next;
        free(temp);
        return 1;
    }
    
    printf("✗ Cartea cu ID %d nu a fost găsită.\n", id);
    return 0;
}

/* =============================================================================
 * FUNCȚIE AUXILIARĂ: afiseaza_toate
 * =============================================================================
 */

void afiseaza_toate(Carte *biblioteca) {
    int count = 0;
    Carte *current = biblioteca;
    
    printf("\n╔═════════════════════════════════════════════════════════════════════╗\n");
    printf("║                    CATALOGUL BIBLIOTECII                            ║\n");
    printf("╠═════════════════════════════════════════════════════════════════════╣\n");
    
    while (current != NULL) {
        const char *status = current->este_imprumutata ? "ÎMPRUMUT" : "DISPON.";
        printf("║ %3d | %-25s | %-15s | %4d | %-8s ║\n",
               current->id, current->titlu, current->autor,
               current->an_publicare, status);
        count++;
        current = current->next;
    }
    
    printf("╠═════════════════════════════════════════════════════════════════════╣\n");
    printf("║ Total cărți în bibliotecă: %-40d ║\n", count);
    printf("╚═════════════════════════════════════════════════════════════════════╝\n");
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    Carte *biblioteca = NULL;
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     TEMA 1: Sistem de Gestiune Bibliotecă - SOLUȚIE          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Adăugăm cărți în bibliotecă */
    printf("═══ Adăugare cărți ═══\n\n");
    adauga_carte(&biblioteca, 1, "Ion", "Liviu Rebreanu", 1920);
    adauga_carte(&biblioteca, 2, "Enigma Otiliei", "George Calinescu", 1938);
    adauga_carte(&biblioteca, 3, "Maitreyi", "Mircea Eliade", 1933);
    adauga_carte(&biblioteca, 4, "Morometii", "Marin Preda", 1955);
    adauga_carte(&biblioteca, 5, "Baltagul", "Mihail Sadoveanu", 1930);
    
    /* Afișăm catalogul */
    afiseaza_toate(biblioteca);
    
    /* Testăm căutarea */
    printf("\n═══ Testare căutare ═══\n");
    cauta_dupa_titlu(biblioteca, "Enigma");
    cauta_dupa_autor(biblioteca, "Eliade");
    
    /* Testăm împrumutul */
    printf("\n═══ Testare împrumut ═══\n\n");
    imprumuta_carte(&biblioteca, 1, "Popescu Ion");
    imprumuta_carte(&biblioteca, 3, "Ionescu Maria");
    imprumuta_carte(&biblioteca, 1, "Alt Cititor");  /* Ar trebui să eșueze */
    
    /* Afișăm cărțile disponibile */
    afiseaza_disponibile(biblioteca);
    
    /* Testăm returnarea */
    printf("\n═══ Testare returnare ═══\n\n");
    returneaza_carte(&biblioteca, 1);
    returneaza_carte(&biblioteca, 1);  /* Ar trebui să eșueze */
    
    /* Testăm ștergerea */
    printf("\n═══ Testare ștergere ═══\n\n");
    sterge_carte(&biblioteca, 5);
    sterge_carte(&biblioteca, 99);  /* Nu există */
    
    /* Afișăm catalogul final */
    printf("\n═══ Catalogul final ═══\n");
    afiseaza_toate(biblioteca);
    
    /* Eliberăm memoria */
    free_biblioteca(&biblioteca);
    printf("\n✓ Memoria a fost eliberată corect.\n");
    
    return 0;
}
