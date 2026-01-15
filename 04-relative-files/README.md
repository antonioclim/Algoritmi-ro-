# Săptămâna 04: Liste Înlănțuite (Linked Lists)

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, studenții vor fi capabili să:

1. **Recunoască** și **descrie** structura fundamentală a unei linked list și componentele sale (nodes, pointer-i)
2. **Înțeleagă** diferențele dintre alocarea statică și dinamică a memoriei, inclusiv rolul funcțiilor `malloc()` și `free()`
3. **Aplice** operațiile fundamentale asupra listelor: inserare (la început, la sfârșit, la poziție), ștergere și căutare
4. **Analizeze** complexitatea temporală și spațială a diferitelor operații pe liste înlănțuite
5. **Evalueze** când este preferabil să folosești linked list vs. array în funcție de contextul problemei
6. **Creeze** implementări complete de liste simplu și dublu înlănțuite cu gestiune corectă a memoriei

---

## 📜 Context Istoric

### Originile Listelor Înlănțuite

Conceptul de linked list își are originile în primii ani ai informaticii, când cercetătorii căutau modalități eficiente de a gestiona date de dimensiuni variabile. În anii 1955-1956, Allen Newell, Cliff Shaw și Herbert A. Simon au dezvoltat limbajul **IPL (Information Processing Language)** la RAND Corporation și Carnegie Mellon University, care a introdus pentru prima dată conceptul de structuri de date înlănțuite.

IPL a fost creat pentru a implementa programul **Logic Theorist**, considerat primul program de inteligență artificială. Necesitatea de a reprezenta expresii logice de dimensiuni arbitrare a condus la inventarea listelor înlănțuite ca structură de date fundamentală.

În anii 1960, John McCarthy a dezvoltat limbajul **LISP** (List Processing), care a făcut din liste înlănțuite paradigma centrală de programare. LISP a demonstrat puterea și eleganța listelor înlănțuite pentru procesarea simbolică, influențând profund dezvoltarea ulterioară a informaticii.

### Figură Cheie: Allen Newell (1927-1992)

Allen Newell a fost un pionier al inteligenței artificiale și al psihologiei cognitive. Împreună cu Herbert Simon, a primit Premiul Turing în 1975 pentru contribuțiile la inteligența artificială și psihologia procesării informației.

> *„Computerul este remarcabil nu pentru ceea ce poate face, ci pentru ceea ce ne permite să gândim."*
> — Allen Newell

Newell a contribuit fundamental la înțelegerea modului în care structurile de date pot modela procesele cognitive umane, iar linked lists au fost instrumentul său preferat pentru reprezentarea cunoștințelor.

---

## 📚 Fundamente Teoretice

### 1. Structura unui Node

Un **node** (nod) este unitatea fundamentală a unei liste înlănțuite. Fiecare nod conține două componente esențiale:

```
┌─────────────────────────────────┐
│           NODE                  │
├─────────────┬───────────────────┤
│    DATA     │      NEXT         │
│  (payload)  │    (pointer)      │
├─────────────┼───────────────────┤
│   valoare   │  adresa către     │
│   stocată   │  următorul nod    │
└─────────────┴───────────────────┘
```

În C, reprezentarea clasică a unui nod:

```c
typedef struct Node {
    int data;           /* Datele stocate în nod */
    struct Node *next;  /* Pointer către următorul nod */
} Node;
```

Observați utilizarea `struct Node *next` în interiorul definiției structurii. Aceasta este o **referință recursivă** — structura se referă la ea însăși, permițând construirea lanțului de noduri.

### 2. Tipuri de Liste Înlănțuite

```
LISTĂ SIMPLU ÎNLĂNȚUITĂ (Singly Linked List):

HEAD
  │
  ▼
┌─────┬───┐    ┌─────┬───┐    ┌─────┬───┐    ┌─────┬──────┐
│  A  │ ●─┼───►│  B  │ ●─┼───►│  C  │ ●─┼───►│  D  │ NULL │
└─────┴───┘    └─────┴───┘    └─────┴───┘    └─────┴──────┘


LISTĂ DUBLU ÎNLĂNȚUITĂ (Doubly Linked List):

HEAD                                                    TAIL
  │                                                       │
  ▼                                                       ▼
┌──────┬───┬───┐    ┌───┬───┬───┐    ┌───┬───┬───┐    ┌───┬───┬──────┐
│ NULL │ A │ ●─┼───►│ ●─┤ B │ ●─┼───►│ ●─┤ C │ ●─┼───►│ ●─┤ D │ NULL │
└──────┴───┴───┘◄───┼─● └───┴───┘◄───┼─● └───┴───┘◄───┼─● └───┴──────┘


LISTĂ CIRCULARĂ (Circular Linked List):

HEAD
  │
  ▼
┌─────┬───┐    ┌─────┬───┐    ┌─────┬───┐    ┌─────┬───┐
│  A  │ ●─┼───►│  B  │ ●─┼───►│  C  │ ●─┼───►│  D  │ ●─┼───┐
└─────┴───┘    └─────┴───┘    └─────┴───┘    └─────┴───┘   │
     ▲                                                     │
     └─────────────────────────────────────────────────────┘
```

### 3. Complexitatea Operațiilor

| Operație | Array | Linked List | Observații |
|----------|-------|-------------|------------|
| Acces la index `i` | O(1) | O(n) | Array-ul folosește aritmetică de pointeri |
| Inserare la început | O(n) | O(1) | Lista nu necesită deplasarea elementelor |
| Inserare la sfârșit | O(1)* | O(n)** | *Array amortizat; **O(1) cu pointer la tail |
| Inserare la poziție `i` | O(n) | O(n) | Ambele necesită traversare/deplasare |
| Ștergere de la început | O(n) | O(1) | Array-ul necesită deplasarea tuturor elementelor |
| Ștergere de la sfârșit | O(1) | O(n)** | **O(1) pentru liste dublu înlănțuite |
| Căutare | O(n) | O(n) | Ambele necesită traversare liniară |
| Spațiu auxiliar | O(1) | O(n) | Pointer-ii ocupă memorie suplimentară |

**Când să folosești Linked List:**
- Inserări/ștergeri frecvente la început
- Dimensiune necunoscută sau foarte variabilă
- Implementarea stivelor și cozilor
- Când nu ai nevoie de acces aleatoriu

**Când să folosești Array:**
- Acces frecvent la elemente după index
- Dimensiune fixă sau previzibilă
- Căutare binară
- Utilizare eficientă a cache-ului CPU

---

## 🏭 Aplicații Industriale

### 1. Sistemul de Fișiere Unix (Inodes)

În sistemele de operare Unix/Linux, inodurile folosesc liste înlănțuite pentru a urmări blocurile de date ale fișierelor mari:

```c
/* Structură simplificată inode */
struct inode {
    int direct_blocks[12];      /* Primele 12 blocuri */
    int *single_indirect;       /* Pointer la bloc de pointeri */
    int *double_indirect;       /* Pointer la bloc de pointeri la pointeri */
    int *triple_indirect;       /* Pentru fișiere foarte mari */
};
```

### 2. Undo/Redo în Editoare de Text

Operațiile de undo/redo în editoarele moderne (vim, VS Code, etc.) folosesc liste dublu înlănțuite:

```c
typedef struct EditAction {
    char *content;
    int position;
    enum { INSERT, DELETE } type;
    struct EditAction *prev;
    struct EditAction *next;
} EditAction;

EditAction *current_state;  /* Pointer la starea curentă */
```

### 3. Gestiunea Memoriei în Nucleul Linux

Kernelul Linux folosește liste înlănțuite pentru gestionarea blocurilor libere de memorie:

```c
/* Din include/linux/list.h */
struct list_head {
    struct list_head *next, *prev;
};

/* Macro pentru a obține structura părinte */
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)
```

### 4. Implementarea HashMap cu Chaining

Rezolvarea coliziunilor în hash tables folosește frecvent liste înlănțuite:

```c
#define TABLE_SIZE 1024

typedef struct Entry {
    char *key;
    void *value;
    struct Entry *next;  /* Chain pentru coliziuni */
} Entry;

Entry *hash_table[TABLE_SIZE];
```

### 5. Browser History

Navigarea înapoi/înainte în browsere web:

```c
typedef struct PageNode {
    char url[2048];
    char title[256];
    struct PageNode *back;
    struct PageNode *forward;
} PageNode;

PageNode *current_page;  /* Pagina curentă */
```

---

## 💻 Exerciții de Laborator

### Exercițiul 1: Lista Simplu Înlănțuită de Studenți

**Obiectiv:** Implementarea unei liste simplu înlănțuite pentru gestionarea datelor studenților.

**Cerințe:**
1. Definirea structurii `Student` cu câmpurile: id (int), nume (char[50]), medie (float)
2. Implementarea funcției de creare a unui nod nou
3. Implementarea inserării la început, la sfârșit și la poziție specificată
4. Implementarea căutării după id și după nume
5. Implementarea ștergerii unui nod (după id)
6. Implementarea afișării complete a listei
7. Implementarea eliberării întregii liste
8. Gestiunea corectă a cazurilor limită (listă goală, element inexistent)

**Fișiere de intrare:** `data/studenti.txt`

### Exercițiul 2: Lista Dublu Înlănțuită - Playlist Muzical

**Obiectiv:** Implementarea unei liste dublu înlănțuite pentru simularea unui playlist muzical cu navigare bidirecțională.

**Cerințe:**
1. Definirea structurii `Melodie` cu: titlu (char[100]), artist (char[50]), durata_secunde (int)
2. Implementarea inserării sortate după titlu
3. Implementarea navigării: next_song(), prev_song(), go_to_start(), go_to_end()
4. Implementarea căutării după artist
5. Implementarea ștergerii melodiei curente
6. Implementarea modului shuffle (ordonare aleatorie)
7. Calcularea duratei totale a playlist-ului
8. Salvarea și încărcarea playlist-ului din/în fișier
9. Inversarea ordinii playlist-ului
10. Eliminarea duplicatelor

**Fișiere de intrare:** `data/playlist.txt`

---

## 🔧 Compilare și Execuție

```bash
# Construirea tuturor programelor
make

# Construirea unui program specific
make example1
make exercise1
make exercise2

# Execuția exemplului demonstrativ
make run

# Execuția testelor automate
make test

# Verificarea memoriei cu Valgrind
make valgrind

# Curățarea fișierelor generate
make clean

# Afișarea ajutorului
make help
```

**Opțiuni de compilare recomandate:**
```bash
gcc -Wall -Wextra -std=c11 -g -o program program.c
```

| Flag | Semnificație |
|------|--------------|
| `-Wall` | Activează toate warning-urile comune |
| `-Wextra` | Warning-uri suplimentare |
| `-std=c11` | Standard C11 |
| `-g` | Informații de debug pentru GDB/Valgrind |

---

## 📁 Structura Directorului

```
week-04-liste-inlantuite/
├── README.md                           # Acest fișier
├── Makefile                            # Automatizare build
│
├── slides/
│   ├── presentation-week04.html        # Prezentarea principală (35-40 slide-uri)
│   └── presentation-comparativ.html    # Comparație Pseudocod → C → Python
│
├── src/
│   ├── example1.c                      # Exemplu complet demonstrativ
│   ├── exercise1.c                     # Exercițiu: Lista de studenți
│   └── exercise2.c                     # Exercițiu: Playlist dublu înlănțuit
│
├── data/
│   ├── studenti.txt                    # Date pentru exercițiul 1
│   └── playlist.txt                    # Date pentru exercițiul 2
│
├── tests/
│   ├── test1_input.txt                 # Input test exercițiul 1
│   ├── test1_expected.txt              # Output așteptat exercițiul 1
│   ├── test2_input.txt                 # Input test exercițiul 2
│   └── test2_expected.txt              # Output așteptat exercițiul 2
│
├── teme/
│   ├── homework-requirements.md        # Cerințe temă (2 × 50 puncte)
│   └── homework-extended.md            # Provocări bonus (+10 puncte fiecare)
│
└── solution/
    ├── exercise1_sol.c                 # Soluție exercițiul 1
    ├── exercise2_sol.c                 # Soluție exercițiul 2
    ├── homework1_sol.c                 # Soluție tema 1
    └── homework2_sol.c                 # Soluție tema 2
```

---

## 📖 Lectură Recomandată

### Esențiale
- **Kernighan & Ritchie** - *The C Programming Language*, Capitolul 6: Structuri
- **Sedgewick & Wayne** - *Algorithms*, Secțiunea 1.3: Bags, Queues and Stacks
- **Cormen et al.** - *Introduction to Algorithms*, Capitolul 10: Elementary Data Structures

### Avansate
- **Knuth, D.E.** - *The Art of Computer Programming*, Vol. 1: Fundamental Algorithms
- **Loudon, K.** - *Mastering Algorithms with C*, Capitolul 5: Linked Lists
- **Love, R.** - *Linux Kernel Development*, Capitolul 6: Kernel Data Structures

### Resurse Online
- [Stanford CS Library - Linked List Basics](http://cslibrary.stanford.edu/103/)
- [GeeksforGeeks - Linked List Data Structure](https://www.geeksforgeeks.org/data-structures/linked-list/)
- [Visualgo - Linked List Visualization](https://visualgo.net/en/list)
- [Linux Kernel Linked List API](https://www.kernel.org/doc/html/latest/core-api/kernel-api.html)

---

## ✅ Lista de Autoevaluare

Înainte de a considera săptămâna completă, verifică dacă poți:

- [ ] Explicați diferența dintre alocarea statică și dinamică a memoriei
- [ ] Definiți o structură de nod și creați noduri cu `malloc()`
- [ ] Implementați inserarea la început în timp O(1)
- [ ] Implementați inserarea la sfârșit (cu și fără pointer tail)
- [ ] Implementați inserarea la o poziție arbitrară
- [ ] Traversați o listă și aplicați o funcție pe fiecare element
- [ ] Căutați un element după valoare sau criteriu
- [ ] Ștergeți un nod fără a produce memory leaks
- [ ] Eliberați întreaga listă corect cu `free()`
- [ ] Identificați și evitați erorile comune: NULL dereference, memory leaks, dangling pointers

---

## 💼 Pregătire pentru Interviu

### Întrebări Frecvente

1. **Care este diferența dintre array și linked list? Când ai folosi fiecare?**
   - Discută trade-off-urile: acces vs. inserare, cache locality, overhead de memorie

2. **Cum detectezi dacă o linked list are un ciclu?**
   - Algoritmul Floyd (Tortoise and Hare): doi pointeri, unul rapid, unul lent

3. **Cum găsești elementul din mijlocul unei liste într-o singură traversare?**
   - Tehnica slow/fast pointer: când fast ajunge la final, slow este la mijloc

4. **Cum inversezi o linked list in-place?**
   - Trei pointeri: prev, current, next; iterativ în O(n), O(1) spațiu

5. **Cum unești două liste sortate într-una singură sortată?**
   - Tehnica merge din merge sort; compară capetele, avansează în cea mai mică

### Provocări de Cod Frecvente

```c
/* Detectarea ciclului - Floyd's Algorithm */
int has_cycle(Node *head) {
    Node *slow = head, *fast = head;
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return 1;  /* Ciclu detectat */
    }
    return 0;  /* Fără ciclu */
}

/* Găsirea mijlocului */
Node *find_middle(Node *head) {
    Node *slow = head, *fast = head;
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;  /* slow este la mijloc */
}
```

---

## 🔗 Previzualizare Săptămâna Următoare

**Săptămâna 05: Stive (Stacks)**

Vom explora structura de date **LIFO** (Last In, First Out):
- Operații push și pop în O(1)
- Implementare cu array vs. linked list
- Aplicații: evaluarea expresiilor, paranteze echilibrate, undo
- Conversia notației infix ↔ postfix
- Algoritmul Shunting Yard

Listele înlănțuite pe care le învățați această săptămână vor fi fundamentul pentru implementarea stivelor în săptămâna următoare!

---

**Autor:** Curs ATP - Algoritmi și Tehnici de Programare  
**Universitate:** Academia de Studii Economice, București  
**Versiune:** 1.0 | Ianuarie 2025
