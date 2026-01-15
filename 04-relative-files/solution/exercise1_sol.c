/**
 * =============================================================================
 * SOLUȚIE - EXERCIȚIUL 1: Lista Simplu Înlănțuită de Studenți
 * =============================================================================
 *
 * ATENȚIE: Acest fișier conține soluția completă.
 * Pentru instructori și pentru auto-verificare.
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUME 50

/* =============================================================================
 * DEFINIȚIA STRUCTURII
 * =============================================================================
 */

typedef struct StudentNode {
    int id;
    char nume[MAX_NUME];
    float medie;
    struct StudentNode *next;
} StudentNode;

/* =============================================================================
 * SOLUȚIA TODO 2: create_student
 * =============================================================================
 */

StudentNode *create_student(int id, const char *nume, float medie) {
    /* Alocăm memorie pentru noul nod */
    StudentNode *new_node = (StudentNode *)malloc(sizeof(StudentNode));
    
    /* Verificăm dacă alocarea a reușit */
    if (new_node == NULL) {
        fprintf(stderr, "Eroare: malloc() a eșuat în create_student()!\n");
        return NULL;
    }
    
    /* Copiem datele în noul nod */
    new_node->id = id;
    strncpy(new_node->nume, nume, MAX_NUME - 1);
    new_node->nume[MAX_NUME - 1] = '\0';  /* Asigurăm terminarea șirului */
    new_node->medie = medie;
    new_node->next = NULL;
    
    return new_node;
}

/* =============================================================================
 * SOLUȚIA TODO 3: free_list
 * =============================================================================
 */

void free_list(StudentNode **head) {
    StudentNode *current = *head;
    StudentNode *next_node;
    
    while (current != NULL) {
        next_node = current->next;  /* Salvăm referința către următorul */
        free(current);              /* Eliberăm nodul curent */
        current = next_node;        /* Avansăm la următorul */
    }
    
    *head = NULL;  /* Setăm head la NULL pentru siguranță */
}

/* =============================================================================
 * SOLUȚIA TODO 4: insert_at_end
 * =============================================================================
 */

void insert_at_end(StudentNode **head, int id, const char *nume, float medie) {
    /* Creăm noul nod */
    StudentNode *new_node = create_student(id, nume, medie);
    if (new_node == NULL) return;
    
    /* Caz special: lista este goală */
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    
    /* Parcurgem lista până la ultimul nod */
    StudentNode *current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    
    /* Atașăm noul nod la sfârșit */
    current->next = new_node;
}

/* =============================================================================
 * SOLUȚIA TODO 5: insert_sorted_by_media
 * =============================================================================
 */

void insert_sorted_by_media(StudentNode **head, int id, const char *nume, float medie) {
    StudentNode *new_node = create_student(id, nume, medie);
    if (new_node == NULL) return;
    
    /* Caz special: lista goală sau noul student are cea mai mare medie */
    if (*head == NULL || (*head)->medie < medie) {
        new_node->next = *head;
        *head = new_node;
        return;
    }
    
    /* Căutăm poziția corectă (lista e sortată descrescător) */
    StudentNode *current = *head;
    while (current->next != NULL && current->next->medie >= medie) {
        current = current->next;
    }
    
    /* Inserăm noul nod */
    new_node->next = current->next;
    current->next = new_node;
}

/* =============================================================================
 * SOLUȚIA TODO 6: find_by_id
 * =============================================================================
 */

StudentNode *find_by_id(StudentNode *head, int id) {
    StudentNode *current = head;
    
    while (current != NULL) {
        if (current->id == id) {
            return current;  /* Găsit! */
        }
        current = current->next;
    }
    
    return NULL;  /* Nu a fost găsit */
}

/* =============================================================================
 * SOLUȚIA TODO 7: find_by_name
 * =============================================================================
 */

StudentNode *find_by_name(StudentNode *head, const char *nume) {
    StudentNode *current = head;
    
    while (current != NULL) {
        if (strcmp(current->nume, nume) == 0) {
            return current;  /* Găsit - potrivire exactă */
        }
        current = current->next;
    }
    
    return NULL;  /* Nu a fost găsit */
}

/* =============================================================================
 * SOLUȚIA TODO 8: delete_by_id
 * =============================================================================
 */

int delete_by_id(StudentNode **head, int id) {
    /* Caz special: lista goală */
    if (*head == NULL) {
        return 0;
    }
    
    /* Caz special: ștergem head-ul */
    if ((*head)->id == id) {
        StudentNode *temp = *head;
        *head = (*head)->next;
        free(temp);
        return 1;
    }
    
    /* Căutăm nodul anterior celui de șters */
    StudentNode *current = *head;
    while (current->next != NULL && current->next->id != id) {
        current = current->next;
    }
    
    /* Dacă am găsit nodul, îl ștergem */
    if (current->next != NULL) {
        StudentNode *temp = current->next;
        current->next = temp->next;
        free(temp);
        return 1;
    }
    
    return 0;  /* Nu a fost găsit */
}

/* =============================================================================
 * FUNCȚII AUXILIARE (NESCHIMBATE)
 * =============================================================================
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
    printf("║     EXERCIȚIUL 1: Lista de Studenți - SOLUȚIE                 ║\n");
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
