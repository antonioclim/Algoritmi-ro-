/**
 * =============================================================================
 * EXERCIȚIUL 1: Lista Simplu Înlănțuită de Studenți
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unei liste simplu înlănțuite pentru gestionarea datelor
 *   studenților. Veți exersa crearea nodurilor, inserarea, căutarea,
 *   ștergerea și eliberarea memoriei.
 *
 * CERINȚE:
 *   1. Completați toate funcțiile marcate cu TODO
 *   2. Nu modificați semnăturile funcțiilor
 *   3. Verificați cu Valgrind că nu aveți memory leaks
 *   4. Programul trebuie să compileze fără warning-uri
 *
 * STRUCTURA STUDENT:
 *   - id: număr unic de identificare
 *   - nume: șir de caractere (max 50)
 *   - medie: nota medie (float)
 *
 * EXEMPLU INPUT (data/studenti.txt):
 *   101 Popescu_Ion 8.75
 *   102 Ionescu_Maria 9.50
 *   103 Georgescu_Ana 7.25
 *
 * EXEMPLU OUTPUT:
 *   Lista studenților:
 *     ID: 101, Nume: Popescu_Ion, Medie: 8.75
 *     ID: 102, Nume: Ionescu_Maria, Medie: 9.50
 *     ID: 103, Nume: Georgescu_Ana, Medie: 7.25
 *   Total: 3 studenți
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -g -o exercise1 exercise1.c
 * UTILIZARE: ./exercise1 < data/studenti.txt
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

#define MAX_NUME 50

/**
 * TODO 1: Definiți structura StudentNode
 *
 * Structura trebuie să conțină:
 *   - id: int - numărul matricol al studentului
 *   - nume: char[MAX_NUME] - numele studentului
 *   - medie: float - media studentului
 *   - next: pointer către următorul StudentNode
 *
 * Hint: Folosiți typedef pentru a crea aliasul StudentNode
 *
 * Exemplu similar:
 *   typedef struct Node {
 *       int data;
 *       struct Node *next;
 *   } Node;
 */

/* CODUL TĂU AICI - Definiția structurii StudentNode */
typedef struct StudentNode {
    int id;
    char nume[MAX_NUME];
    float medie;
    struct StudentNode *next;
} StudentNode;


/* =============================================================================
 * FUNCȚII DE CREARE ȘI ELIBERARE
 * =============================================================================
 */

/**
 * TODO 2: Implementați funcția create_student
 *
 * Creează un nou nod StudentNode cu datele specificate.
 *
 * @param id Numărul matricol al studentului
 * @param nume Numele studentului (se copiază în nod)
 * @param medie Media studentului
 * @return Pointer către noul nod sau NULL dacă alocarea eșuează
 *
 * Pași:
 *   1. Alocați memorie pentru un StudentNode cu malloc()
 *   2. Verificați dacă alocarea a reușit
 *   3. Copiați datele în noul nod (folosiți strncpy pentru nume!)
 *   4. Setați next la NULL
 *   5. Returnați pointer-ul către noul nod
 *
 * Hint: strncpy(dest, src, n) copiază maxim n caractere
 */
StudentNode *create_student(int id, const char *nume, float medie) {
    /* CODUL TĂU AICI */
    
    return NULL;  /* Înlocuiește cu implementarea ta */
}

/**
 * TODO 3: Implementați funcția free_list
 *
 * Eliberează toată memoria alocată pentru lista de studenți.
 *
 * @param head Pointer la pointer-ul head (pentru a-l seta la NULL)
 *
 * Pași:
 *   1. Parcurgeți lista
 *   2. Pentru fiecare nod: salvați next, eliberați nodul curent
 *   3. La final, setați *head = NULL
 *
 * IMPORTANT: Această funcție TREBUIE apelată înainte de exit!
 */
void free_list(StudentNode **head) {
    /* CODUL TĂU AICI */
    
}

/* =============================================================================
 * FUNCȚII DE INSERARE
 * =============================================================================
 */

/**
 * TODO 4: Implementați funcția insert_at_end
 *
 * Inserează un student nou la sfârșitul listei.
 *
 * @param head Pointer la pointer-ul head
 * @param id ID-ul studentului
 * @param nume Numele studentului
 * @param medie Media studentului
 *
 * Pași:
 *   1. Creați un nod nou cu create_student()
 *   2. Dacă lista e goală, noul nod devine head
 *   3. Altfel, parcurgeți până la ultimul nod și atașați
 *
 * Complexitate: O(n)
 */
void insert_at_end(StudentNode **head, int id, const char *nume, float medie) {
    /* CODUL TĂU AICI */
    
}

/**
 * TODO 5: Implementați funcția insert_sorted_by_media
 *
 * Inserează un student în listă astfel încât lista să rămână
 * sortată descrescător după medie.
 *
 * @param head Pointer la pointer-ul head
 * @param id ID-ul studentului
 * @param nume Numele studentului
 * @param medie Media studentului
 *
 * Pași:
 *   1. Creați nodul nou
 *   2. Caz special: lista goală sau noul student are cea mai mare medie
 *   3. Altfel, găsiți poziția corectă și inserați
 *
 * Exemplu: Dacă lista are [9.5, 8.0, 7.0] și inserăm 8.5,
 *          rezultatul va fi [9.5, 8.5, 8.0, 7.0]
 */
void insert_sorted_by_media(StudentNode **head, int id, const char *nume, float medie) {
    /* CODUL TĂU AICI */
    
}

/* =============================================================================
 * FUNCȚII DE CĂUTARE
 * =============================================================================
 */

/**
 * TODO 6: Implementați funcția find_by_id
 *
 * Caută un student după ID.
 *
 * @param head Pointer către capul listei
 * @param id ID-ul căutat
 * @return Pointer către nodul găsit sau NULL dacă nu există
 *
 * Complexitate: O(n)
 */
StudentNode *find_by_id(StudentNode *head, int id) {
    /* CODUL TĂU AICI */
    
    return NULL;  /* Înlocuiește cu implementarea ta */
}

/**
 * TODO 7: Implementați funcția find_by_name
 *
 * Caută un student după nume (potrivire exactă).
 *
 * @param head Pointer către capul listei
 * @param nume Numele căutat
 * @return Pointer către nodul găsit sau NULL dacă nu există
 *
 * Hint: Folosiți strcmp() pentru compararea șirurilor
 *       strcmp(s1, s2) returnează 0 dacă sunt egale
 */
StudentNode *find_by_name(StudentNode *head, const char *nume) {
    /* CODUL TĂU AICI */
    
    return NULL;  /* Înlocuiește cu implementarea ta */
}

/* =============================================================================
 * FUNCȚII DE ȘTERGERE
 * =============================================================================
 */

/**
 * TODO 8: Implementați funcția delete_by_id
 *
 * Șterge studentul cu ID-ul specificat din listă.
 *
 * @param head Pointer la pointer-ul head
 * @param id ID-ul studentului de șters
 * @return 1 dacă studentul a fost șters, 0 dacă nu a fost găsit
 *
 * Pași:
 *   1. Caz special: lista goală
 *   2. Caz special: ștergem head-ul
 *   3. Altfel: găsiți nodul anterior și actualizați legăturile
 *   4. Eliberați memoria nodului șters cu free()
 *
 * IMPORTANT: Nu uitați să eliberați memoria!
 */
int delete_by_id(StudentNode **head, int id) {
    /* CODUL TĂU AICI */
    
    return 0;  /* Înlocuiește cu implementarea ta */
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE ȘI STATISTICI
 * =============================================================================
 */

/**
 * Afișează toți studenții din listă.
 * (Această funcție este deja implementată)
 */
void print_students(StudentNode *head) {
    if (head == NULL) {
        printf("Lista este goală.\n");
        return;
    }
    
    printf("Lista studenților:\n");
    printf("─────────────────────────────────────────────────\n");
    
    StudentNode *current = head;
    int count = 0;
    
    while (current != NULL) {
        printf("  ID: %d, Nume: %s, Medie: %.2f\n",
               current->id, current->nume, current->medie);
        count++;
        current = current->next;
    }
    
    printf("─────────────────────────────────────────────────\n");
    printf("Total: %d studenți\n", count);
}

/**
 * Calculează media generală a tuturor studenților.
 * (Această funcție este deja implementată)
 */
float calculate_average(StudentNode *head) {
    if (head == NULL) return 0.0f;
    
    float sum = 0.0f;
    int count = 0;
    StudentNode *current = head;
    
    while (current != NULL) {
        sum += current->medie;
        count++;
        current = current->next;
    }
    
    return sum / count;
}

/**
 * Numără studenții cu media peste un prag.
 * (Această funcție este deja implementată)
 */
int count_above_threshold(StudentNode *head, float threshold) {
    int count = 0;
    StudentNode *current = head;
    
    while (current != NULL) {
        if (current->medie >= threshold) {
            count++;
        }
        current = current->next;
    }
    
    return count;
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    StudentNode *head = NULL;
    int id;
    char nume[MAX_NUME];
    float medie;
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 1: Lista de Studenți                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Citim studenții din input */
    printf("Citire studenți (format: ID Nume Media)...\n\n");
    
    while (scanf("%d %49s %f", &id, nume, &medie) == 3) {
        insert_at_end(&head, id, nume, medie);
    }
    
    /* Afișăm lista */
    print_students(head);
    
    /* Calculăm statistici */
    printf("\nStatistici:\n");
    printf("  Media generală: %.2f\n", calculate_average(head));
    printf("  Studenți cu media >= 8.0: %d\n", count_above_threshold(head, 8.0f));
    printf("  Studenți cu media >= 9.0: %d\n", count_above_threshold(head, 9.0f));
    
    /* Testăm căutarea */
    printf("\nTestare căutare:\n");
    int search_id = 102;
    StudentNode *found = find_by_id(head, search_id);
    if (found != NULL) {
        printf("  Studentul cu ID %d: %s (medie: %.2f)\n",
               search_id, found->nume, found->medie);
    } else {
        printf("  Studentul cu ID %d nu a fost găsit.\n", search_id);
    }
    
    /* Testăm ștergerea */
    printf("\nTestare ștergere:\n");
    int delete_id = 102;
    if (delete_by_id(&head, delete_id)) {
        printf("  Studentul cu ID %d a fost șters.\n", delete_id);
    } else {
        printf("  Studentul cu ID %d nu a fost găsit.\n", delete_id);
    }
    
    /* Afișăm lista după ștergere */
    printf("\nLista după ștergere:\n");
    print_students(head);
    
    /* FOARTE IMPORTANT: Eliberăm memoria! */
    free_list(&head);
    printf("\n✓ Memoria a fost eliberată.\n");
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Implementați funcția reverse_list() care inversează lista.
 *
 * 2. Implementați funcția sort_by_name() care sortează lista alfabetic
 *    după nume (folosind bubble sort sau insertion sort pe liste).
 *
 * 3. Implementați funcția merge_lists() care unește două liste sortate
 *    într-o singură listă sortată.
 *
 * 4. Implementați funcția save_to_file() care salvează lista într-un fișier.
 *
 * 5. Implementați funcția load_from_file() care încarcă lista dintr-un fișier.
 *
 * =============================================================================
 */
