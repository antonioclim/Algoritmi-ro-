/**
 * =============================================================================
 * SĂPTĂMÂNA 04: LISTE ÎNLĂNȚUITE (LINKED LISTS)
 * Exemplu Complet Demonstrativ
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Definiția și crearea nodurilor
 *   2. Operații pe liste simplu înlănțuite (inserare, ștergere, căutare)
 *   3. Operații pe liste dublu înlănțuite
 *   4. Algoritmi clasici (inversare, detectare ciclu, găsire mijloc)
 *   5. Gestiunea corectă a memoriei
 *   6. Tehnici de debugging și bune practici
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -g -o example1 example1.c
 * Utilizare: ./example1
 *
 * Verificare memorie: valgrind --leak-check=full ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* =============================================================================
 * PARTEA 1: DEFINIȚII ȘI STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Structura unui nod pentru lista simplu înlănțuită.
 * Conține date (int) și un pointer către următorul nod.
 */
typedef struct Node {
    int data;           /* Datele stocate în nod */
    struct Node *next;  /* Pointer către următorul nod (sau NULL) */
} Node;

/**
 * Structura unui nod pentru lista dublu înlănțuită.
 * Conține date și pointeri către nodurile anterior și următor.
 */
typedef struct DNode {
    int data;            /* Datele stocate în nod */
    struct DNode *prev;  /* Pointer către nodul anterior */
    struct DNode *next;  /* Pointer către nodul următor */
} DNode;

/**
 * Structura pentru lista dublu înlănțuită cu head și tail.
 * Permite operații O(1) la ambele capete.
 */
typedef struct {
    DNode *head;  /* Pointer către primul nod */
    DNode *tail;  /* Pointer către ultimul nod */
    int size;     /* Numărul de elemente */
} DoublyLinkedList;

/* =============================================================================
 * PARTEA 2: FUNCȚII PENTRU LISTA SIMPLU ÎNLĂNȚUITĂ
 * =============================================================================
 */

/**
 * Creează un nod nou cu valoarea specificată.
 *
 * @param value Valoarea de stocat în nod
 * @return Pointer către noul nod sau NULL dacă alocarea eșuează
 */
Node *create_node(int value) {
    /* Alocă memorie pentru noul nod */
    Node *new_node = (Node *)malloc(sizeof(Node));
    
    /* Verifică dacă alocarea a reușit */
    if (new_node == NULL) {
        fprintf(stderr, "Eroare: malloc() a eșuat în create_node()!\n");
        return NULL;
    }
    
    /* Inițializează câmpurile nodului */
    new_node->data = value;
    new_node->next = NULL;
    
    return new_node;
}

/**
 * Inserează un nod nou la începutul listei (prepend).
 * Complexitate: O(1)
 *
 * @param head Pointer la pointer-ul head (pentru a-l putea modifica)
 * @param value Valoarea de inserat
 */
void prepend(Node **head, int value) {
    Node *new_node = create_node(value);
    if (new_node == NULL) return;
    
    /* Noul nod pointează către fostul cap al listei */
    new_node->next = *head;
    
    /* Actualizăm head-ul să pointeze către noul nod */
    *head = new_node;
}

/**
 * Inserează un nod nou la sfârșitul listei (append).
 * Complexitate: O(n) - trebuie să parcurgem toată lista
 *
 * @param head Pointer la pointer-ul head
 * @param value Valoarea de inserat
 */
void append(Node **head, int value) {
    Node *new_node = create_node(value);
    if (new_node == NULL) return;
    
    /* Caz special: lista este goală */
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    
    /* Parcurgem lista până la ultimul nod */
    Node *current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    
    /* Atașăm noul nod la sfârșit */
    current->next = new_node;
}

/**
 * Inserează un nod nou la o poziție specificată (0-indexed).
 * Complexitate: O(n)
 *
 * @param head Pointer la pointer-ul head
 * @param value Valoarea de inserat
 * @param position Poziția la care se inserează (0 = început)
 */
void insert_at(Node **head, int value, int position) {
    /* Caz special: inserare la început */
    if (position == 0) {
        prepend(head, value);
        return;
    }
    
    Node *new_node = create_node(value);
    if (new_node == NULL) return;
    
    /* Parcurgem lista până la poziția (position - 1) */
    Node *current = *head;
    for (int i = 0; i < position - 1 && current != NULL; i++) {
        current = current->next;
    }
    
    /* Verificăm dacă poziția este validă */
    if (current == NULL) {
        fprintf(stderr, "Eroare: poziție %d invalidă!\n", position);
        free(new_node);
        return;
    }
    
    /* Inserăm noul nod */
    new_node->next = current->next;
    current->next = new_node;
}

/**
 * Afișează conținutul listei.
 * Complexitate: O(n)
 *
 * @param head Pointer către capul listei
 */
void print_list(Node *head) {
    printf("  Lista: ");
    
    if (head == NULL) {
        printf("(goală)\n");
        return;
    }
    
    Node *current = head;
    while (current != NULL) {
        printf("%d", current->data);
        if (current->next != NULL) {
            printf(" -> ");
        }
        current = current->next;
    }
    printf(" -> NULL\n");
}

/**
 * Numără elementele din listă.
 * Complexitate: O(n)
 *
 * @param head Pointer către capul listei
 * @return Numărul de noduri din listă
 */
int count_nodes(Node *head) {
    int count = 0;
    Node *current = head;
    
    while (current != NULL) {
        count++;
        current = current->next;
    }
    
    return count;
}

/**
 * Caută un nod cu valoarea specificată.
 * Complexitate: O(n)
 *
 * @param head Pointer către capul listei
 * @param value Valoarea căutată
 * @return Pointer către nodul găsit sau NULL
 */
Node *search(Node *head, int value) {
    Node *current = head;
    
    while (current != NULL) {
        if (current->data == value) {
            return current;  /* Găsit! */
        }
        current = current->next;
    }
    
    return NULL;  /* Nu a fost găsit */
}

/**
 * Returnează nodul de la indexul specificat (0-indexed).
 * Complexitate: O(n)
 *
 * @param head Pointer către capul listei
 * @param index Indexul dorit
 * @return Pointer către nodul găsit sau NULL
 */
Node *get_at(Node *head, int index) {
    if (index < 0) return NULL;
    
    Node *current = head;
    int i = 0;
    
    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }
    
    return current;
}

/**
 * Șterge primul nod cu valoarea specificată.
 * Complexitate: O(n)
 *
 * @param head Pointer la pointer-ul head
 * @param value Valoarea nodului de șters
 */
void delete_node(Node **head, int value) {
    if (*head == NULL) return;
    
    /* Caz special: ștergem capul listei */
    if ((*head)->data == value) {
        Node *temp = *head;
        *head = (*head)->next;
        free(temp);
        return;
    }
    
    /* Căutăm nodul anterior celui de șters */
    Node *current = *head;
    while (current->next != NULL && current->next->data != value) {
        current = current->next;
    }
    
    /* Dacă am găsit nodul, îl ștergem */
    if (current->next != NULL) {
        Node *temp = current->next;
        current->next = temp->next;
        free(temp);
    }
}

/**
 * Eliberează toată memoria alocată pentru listă.
 * OBLIGATORIU de apelat înainte de terminarea programului!
 *
 * @param head Pointer la pointer-ul head
 */
void free_list(Node **head) {
    Node *current = *head;
    Node *next_node;
    
    while (current != NULL) {
        next_node = current->next;  /* Salvăm referința către următorul */
        free(current);              /* Eliberăm nodul curent */
        current = next_node;        /* Avansăm la următorul */
    }
    
    *head = NULL;  /* Setăm head la NULL pentru siguranță */
}

/* =============================================================================
 * PARTEA 3: ALGORITMI CLASICI PE LISTE
 * =============================================================================
 */

/**
 * Inversează o listă simplu înlănțuită in-place.
 * Complexitate: O(n) timp, O(1) spațiu
 *
 * @param head Pointer la pointer-ul head
 */
void reverse_list(Node **head) {
    Node *prev = NULL;
    Node *current = *head;
    Node *next_node = NULL;
    
    while (current != NULL) {
        /* Salvăm pointer-ul către următorul nod */
        next_node = current->next;
        
        /* Inversăm legătura: nodul curent pointează acum înapoi */
        current->next = prev;
        
        /* Avansăm pointerii */
        prev = current;
        current = next_node;
    }
    
    /* prev este acum noul cap al listei */
    *head = prev;
}

/**
 * Găsește nodul din mijlocul listei într-o singură traversare.
 * Folosește tehnica "slow and fast pointers".
 * Complexitate: O(n)
 *
 * @param head Pointer către capul listei
 * @return Pointer către nodul din mijloc
 */
Node *find_middle(Node *head) {
    if (head == NULL) return NULL;
    
    Node *slow = head;  /* Avansează cu 1 pas */
    Node *fast = head;  /* Avansează cu 2 pași */
    
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    return slow;  /* slow este la mijloc când fast ajunge la final */
}

/**
 * Detectează dacă lista are un ciclu (algoritm Floyd).
 * Folosește doi pointeri cu viteze diferite.
 * Complexitate: O(n) timp, O(1) spațiu
 *
 * @param head Pointer către capul listei
 * @return 1 dacă există ciclu, 0 altfel
 */
int has_cycle(Node *head) {
    if (head == NULL) return 0;
    
    Node *slow = head;  /* Broasca țestoasă */
    Node *fast = head;  /* Iepurele */
    
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;           /* Avansează cu 1 */
        fast = fast->next->next;     /* Avansează cu 2 */
        
        if (slow == fast) {
            return 1;  /* S-au întâlnit = ciclu detectat! */
        }
    }
    
    return 0;  /* fast a ajuns la NULL = nu există ciclu */
}

/**
 * Găsește al n-lea nod de la sfârșit într-o singură traversare.
 * Folosește doi pointeri la distanță n unul de celălalt.
 * Complexitate: O(n)
 *
 * @param head Pointer către capul listei
 * @param n Poziția de la sfârșit (1 = ultimul, 2 = penultimul, etc.)
 * @return Pointer către nodul găsit sau NULL
 */
Node *get_nth_from_end(Node *head, int n) {
    if (head == NULL || n <= 0) return NULL;
    
    Node *first = head;
    Node *second = head;
    
    /* Avansăm primul pointer cu n poziții */
    for (int i = 0; i < n; i++) {
        if (first == NULL) return NULL;  /* Lista e prea scurtă */
        first = first->next;
    }
    
    /* Avansăm ambii pointeri până first ajunge la NULL */
    while (first != NULL) {
        first = first->next;
        second = second->next;
    }
    
    return second;  /* second este la poziția n de la sfârșit */
}

/**
 * Elimină duplicatele dintr-o listă sortată.
 * Complexitate: O(n)
 *
 * @param head Pointer către capul listei sortate
 */
void remove_duplicates_sorted(Node *head) {
    if (head == NULL) return;
    
    Node *current = head;
    
    while (current->next != NULL) {
        if (current->data == current->next->data) {
            /* Duplicat găsit - îl eliminăm */
            Node *temp = current->next;
            current->next = temp->next;
            free(temp);
        } else {
            current = current->next;
        }
    }
}

/* =============================================================================
 * PARTEA 4: LISTA DUBLU ÎNLĂNȚUITĂ
 * =============================================================================
 */

/**
 * Inițializează o listă dublu înlănțuită goală.
 *
 * @param list Pointer către structura listei
 */
void dll_init(DoublyLinkedList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

/**
 * Creează un nod nou pentru lista dublu înlănțuită.
 *
 * @param value Valoarea de stocat
 * @return Pointer către noul nod sau NULL
 */
DNode *dll_create_node(int value) {
    DNode *new_node = (DNode *)malloc(sizeof(DNode));
    
    if (new_node == NULL) {
        fprintf(stderr, "Eroare: malloc() a eșuat în dll_create_node()!\n");
        return NULL;
    }
    
    new_node->data = value;
    new_node->prev = NULL;
    new_node->next = NULL;
    
    return new_node;
}

/**
 * Inserează la începutul listei dublu înlănțuite.
 * Complexitate: O(1)
 *
 * @param list Pointer către lista
 * @param value Valoarea de inserat
 */
void dll_prepend(DoublyLinkedList *list, int value) {
    DNode *new_node = dll_create_node(value);
    if (new_node == NULL) return;
    
    if (list->head == NULL) {
        /* Lista era goală */
        list->head = new_node;
        list->tail = new_node;
    } else {
        new_node->next = list->head;
        list->head->prev = new_node;
        list->head = new_node;
    }
    
    list->size++;
}

/**
 * Inserează la sfârșitul listei dublu înlănțuite.
 * Complexitate: O(1) - datorită pointer-ului tail!
 *
 * @param list Pointer către lista
 * @param value Valoarea de inserat
 */
void dll_append(DoublyLinkedList *list, int value) {
    DNode *new_node = dll_create_node(value);
    if (new_node == NULL) return;
    
    if (list->tail == NULL) {
        /* Lista era goală */
        list->head = new_node;
        list->tail = new_node;
    } else {
        new_node->prev = list->tail;
        list->tail->next = new_node;
        list->tail = new_node;
    }
    
    list->size++;
}

/**
 * Șterge un nod specific din lista dublu înlănțuită.
 * Complexitate: O(1) - când avem pointer-ul la nod!
 *
 * @param list Pointer către lista
 * @param node Pointer către nodul de șters
 */
void dll_remove_node(DoublyLinkedList *list, DNode *node) {
    if (list == NULL || node == NULL) return;
    
    /* Actualizăm legăturile */
    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;  /* Ștergem head-ul */
    }
    
    if (node->next != NULL) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;  /* Ștergem tail-ul */
    }
    
    free(node);
    list->size--;
}

/**
 * Afișează lista dublu înlănțuită în ambele direcții.
 *
 * @param list Pointer către lista
 */
void dll_print(DoublyLinkedList *list) {
    printf("  Înainte:  ");
    DNode *current = list->head;
    while (current != NULL) {
        printf("%d", current->data);
        if (current->next != NULL) printf(" <-> ");
        current = current->next;
    }
    printf("\n");
    
    printf("  Înapoi:   ");
    current = list->tail;
    while (current != NULL) {
        printf("%d", current->data);
        if (current->prev != NULL) printf(" <-> ");
        current = current->prev;
    }
    printf("\n");
}

/**
 * Eliberează toată memoria listei dublu înlănțuite.
 *
 * @param list Pointer către lista
 */
void dll_free(DoublyLinkedList *list) {
    DNode *current = list->head;
    DNode *next_node;
    
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

/* =============================================================================
 * PARTEA 5: FUNCȚII DEMONSTRATIVE
 * =============================================================================
 */

void demo_basic_operations(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 1: OPERAȚII DE BAZĂ PE LISTA SIMPLU ÎNLĂNȚUITĂ    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Node *head = NULL;
    
    printf("Creăm o listă goală și adăugăm elemente:\n\n");
    
    /* Demonstrăm prepend */
    printf("→ prepend(30), prepend(20), prepend(10):\n");
    prepend(&head, 30);
    prepend(&head, 20);
    prepend(&head, 10);
    print_list(head);
    
    /* Demonstrăm append */
    printf("\n→ append(40), append(50):\n");
    append(&head, 40);
    append(&head, 50);
    print_list(head);
    
    /* Demonstrăm insert_at */
    printf("\n→ insert_at(poziția 2, valoarea 25):\n");
    insert_at(&head, 25, 2);
    print_list(head);
    
    /* Demonstrăm count */
    printf("\n→ Număr de noduri: %d\n", count_nodes(head));
    
    /* Demonstrăm search */
    printf("\n→ Căutăm valoarea 25: ");
    Node *found = search(head, 25);
    if (found != NULL) {
        printf("GĂSIT (adresa: %p)\n", (void *)found);
    } else {
        printf("NU EXISTĂ\n");
    }
    
    printf("→ Căutăm valoarea 100: ");
    found = search(head, 100);
    if (found != NULL) {
        printf("GĂSIT\n");
    } else {
        printf("NU EXISTĂ\n");
    }
    
    /* Demonstrăm get_at */
    printf("\n→ Elementul de la indexul 3: ");
    Node *node_at_3 = get_at(head, 3);
    if (node_at_3 != NULL) {
        printf("%d\n", node_at_3->data);
    } else {
        printf("Index invalid\n");
    }
    
    /* Demonstrăm delete */
    printf("\n→ delete_node(25):\n");
    delete_node(&head, 25);
    print_list(head);
    
    printf("\n→ delete_node(10) - ștergem head-ul:\n");
    delete_node(&head, 10);
    print_list(head);
    
    printf("\n→ delete_node(50) - ștergem tail-ul:\n");
    delete_node(&head, 50);
    print_list(head);
    
    /* Eliberăm memoria */
    free_list(&head);
    printf("\n✓ Memoria a fost eliberată corect.\n");
}

void demo_algorithms(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 2: ALGORITMI CLASICI PE LISTE                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Node *head = NULL;
    
    /* Creăm o listă de test */
    for (int i = 1; i <= 7; i++) {
        append(&head, i * 10);
    }
    
    printf("Lista inițială:\n");
    print_list(head);
    
    /* Demonstrăm inversarea */
    printf("\n→ reverse_list():\n");
    reverse_list(&head);
    print_list(head);
    
    /* Inversăm din nou pentru următoarele demo-uri */
    reverse_list(&head);
    printf("\n→ Inversăm din nou pentru demo:\n");
    print_list(head);
    
    /* Demonstrăm găsirea mijlocului */
    printf("\n→ find_middle():\n");
    Node *middle = find_middle(head);
    if (middle != NULL) {
        printf("  Elementul din mijloc: %d\n", middle->data);
    }
    
    /* Demonstrăm găsirea nodului n de la sfârșit */
    printf("\n→ get_nth_from_end(2) - al 2-lea de la sfârșit:\n");
    Node *nth = get_nth_from_end(head, 2);
    if (nth != NULL) {
        printf("  Rezultat: %d\n", nth->data);
    }
    
    printf("\n→ get_nth_from_end(5) - al 5-lea de la sfârșit:\n");
    nth = get_nth_from_end(head, 5);
    if (nth != NULL) {
        printf("  Rezultat: %d\n", nth->data);
    }
    
    /* Demonstrăm detectarea ciclului */
    printf("\n→ has_cycle() pe lista normală:\n");
    printf("  Rezultat: %s\n", has_cycle(head) ? "CICLU DETECTAT" : "Fără ciclu");
    
    /* Eliberăm memoria */
    free_list(&head);
    
    /* Demonstrăm eliminarea duplicatelor */
    printf("\n→ remove_duplicates_sorted() pe listă cu duplicate:\n");
    head = NULL;
    int values[] = {1, 1, 2, 3, 3, 3, 4, 5, 5};
    for (int i = 0; i < 9; i++) {
        append(&head, values[i]);
    }
    printf("  Înainte: ");
    print_list(head);
    remove_duplicates_sorted(head);
    printf("  După:    ");
    print_list(head);
    
    free_list(&head);
    printf("\n✓ Memoria a fost eliberată corect.\n");
}

void demo_doubly_linked(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 3: LISTA DUBLU ÎNLĂNȚUITĂ                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    DoublyLinkedList list;
    dll_init(&list);
    
    printf("Creăm o listă dublu înlănțuită:\n\n");
    
    /* Adăugăm elemente */
    printf("→ dll_append(20), dll_append(30), dll_append(40):\n");
    dll_append(&list, 20);
    dll_append(&list, 30);
    dll_append(&list, 40);
    dll_print(&list);
    
    printf("\n→ dll_prepend(10), dll_prepend(5):\n");
    dll_prepend(&list, 10);
    dll_prepend(&list, 5);
    dll_print(&list);
    
    printf("\n→ Dimensiune: %d\n", list.size);
    printf("→ Head: %d, Tail: %d\n", list.head->data, list.tail->data);
    
    /* Demonstrăm ștergerea */
    printf("\n→ Ștergem al doilea element (10):\n");
    DNode *second = list.head->next;
    dll_remove_node(&list, second);
    dll_print(&list);
    
    printf("\n→ Ștergem head-ul (5):\n");
    dll_remove_node(&list, list.head);
    dll_print(&list);
    
    printf("\n→ Ștergem tail-ul (40):\n");
    dll_remove_node(&list, list.tail);
    dll_print(&list);
    
    /* Eliberăm memoria */
    dll_free(&list);
    printf("\n✓ Memoria a fost eliberată corect.\n");
}

void demo_memory_safety(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 4: SIGURANȚĂ ȘI BUNE PRACTICI                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("1. VERIFICĂ ÎNTOTDEAUNA REZULTATUL LUI malloc():\n");
    printf("   ┌─────────────────────────────────────────────────────┐\n");
    printf("   │ Node *node = (Node *)malloc(sizeof(Node));         │\n");
    printf("   │ if (node == NULL) {                                │\n");
    printf("   │     fprintf(stderr, \"Eroare: memorie insuficientă!\\n\");│\n");
    printf("   │     return NULL;                                   │\n");
    printf("   │ }                                                  │\n");
    printf("   └─────────────────────────────────────────────────────┘\n\n");
    
    printf("2. ELIBEREAZĂ MEMORIA CU free() ȘI SETEAZĂ POINTER-UL LA NULL:\n");
    printf("   ┌─────────────────────────────────────────────────────┐\n");
    printf("   │ free(node);                                        │\n");
    printf("   │ node = NULL;  // Previne dangling pointer          │\n");
    printf("   └─────────────────────────────────────────────────────┘\n\n");
    
    printf("3. FOLOSEȘTE Node **head PENTRU A MODIFICA POINTER-UL HEAD:\n");
    printf("   ┌─────────────────────────────────────────────────────┐\n");
    printf("   │ void prepend(Node **head, int value) {             │\n");
    printf("   │     // ...                                         │\n");
    printf("   │     *head = new_node;  // Modifică pointer-ul real │\n");
    printf("   │ }                                                  │\n");
    printf("   │                                                    │\n");
    printf("   │ // Apel: prepend(&head, 42);                       │\n");
    printf("   └─────────────────────────────────────────────────────┘\n\n");
    
    printf("4. ORDINEA LA INSERARE - CONECTEAZĂ MAI ÎNTÂI NOUL NOD:\n");
    printf("   ┌─────────────────────────────────────────────────────┐\n");
    printf("   │ // CORECT:                                         │\n");
    printf("   │ new_node->next = current->next;  // Pasul 1        │\n");
    printf("   │ current->next = new_node;        // Pasul 2        │\n");
    printf("   │                                                    │\n");
    printf("   │ // GREȘIT (pierde restul listei!):                 │\n");
    printf("   │ current->next = new_node;  // Pierdem referința!   │\n");
    printf("   │ new_node->next = ???       // Prea târziu...       │\n");
    printf("   └─────────────────────────────────────────────────────┘\n\n");
    
    printf("5. VERIFICĂ POINTER-II ÎNAINTE DE DEREFERENȚIERE:\n");
    printf("   ┌─────────────────────────────────────────────────────┐\n");
    printf("   │ if (head != NULL) {                                │\n");
    printf("   │     printf(\"%%d\", head->data);  // Sigur           │\n");
    printf("   │ }                                                  │\n");
    printf("   │                                                    │\n");
    printf("   │ // SAU verificare în while:                        │\n");
    printf("   │ while (current != NULL) { ... }                    │\n");
    printf("   └─────────────────────────────────────────────────────┘\n\n");
    
    printf("6. FOLOSEȘTE VALGRIND PENTRU VERIFICAREA MEMORIEI:\n");
    printf("   $ valgrind --leak-check=full ./program\n\n");
}

void demo_complexity_comparison(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 5: COMPARAȚIE COMPLEXITATE                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("┌────────────────────┬─────────────┬─────────────────────────┐\n");
    printf("│      Operație      │    Array    │      Linked List        │\n");
    printf("├────────────────────┼─────────────┼─────────────────────────┤\n");
    printf("│ Acces la index[i]  │    O(1)     │         O(n)            │\n");
    printf("│ Inserare început   │    O(n)     │         O(1)            │\n");
    printf("│ Inserare sfârșit   │   O(1)*     │    O(n) sau O(1)**      │\n");
    printf("│ Inserare mijloc    │    O(n)     │         O(n)            │\n");
    printf("│ Ștergere început   │    O(n)     │         O(1)            │\n");
    printf("│ Ștergere sfârșit   │    O(1)     │    O(n) sau O(1)***     │\n");
    printf("│ Căutare            │    O(n)     │         O(n)            │\n");
    printf("│ Spațiu extra       │    O(1)     │         O(n)            │\n");
    printf("└────────────────────┴─────────────┴─────────────────────────┘\n\n");
    
    printf("Legendă:\n");
    printf("  *   Array dinamic (amortizat)\n");
    printf("  **  O(1) dacă avem pointer tail\n");
    printf("  *** O(1) pentru liste dublu înlănțuite cu tail\n\n");
    
    printf("CÂND SĂ FOLOSEȘTI LINKED LIST:\n");
    printf("  ✓ Inserări/ștergeri frecvente la început\n");
    printf("  ✓ Dimensiune foarte variabilă și impredictibilă\n");
    printf("  ✓ Nu ai nevoie de acces aleatoriu (random access)\n");
    printf("  ✓ Implementezi stive sau cozi\n\n");
    
    printf("CÂND SĂ FOLOSEȘTI ARRAY:\n");
    printf("  ✓ Acces frecvent la elemente după index\n");
    printf("  ✓ Dimensiune relativ fixă sau previzibilă\n");
    printf("  ✓ Sortare și căutare binară\n");
    printf("  ✓ Performanță cache (cache locality)\n");
}

/* =============================================================================
 * FUNCȚIA MAIN
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 04: LISTE ÎNLĂNȚUITE - Exemplu Complet          ║\n");
    printf("║     Algoritmi și Tehnici de Programare (ATP)                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Rulăm demonstrațiile */
    demo_basic_operations();
    demo_algorithms();
    demo_doubly_linked();
    demo_memory_safety();
    demo_complexity_comparison();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     ✓ Toate demonstrațiile s-au încheiat cu succes!           ║\n");
    printf("║     Verifică memoria cu: valgrind --leak-check=full ./example1║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
