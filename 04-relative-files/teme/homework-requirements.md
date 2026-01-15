# Tema Săptămâna 04: Liste Înlănțuite

## 📋 Informații Generale

- **Termen limită:** Sfârșitul săptămânii 05
- **Punctaj:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu flagurile `-Wall -Wextra -std=c11`
- **Verificare memorie:** Valgrind (0 leaks = obligatoriu)

---

## 📝 Tema 1: Sistem de Gestiune Bibliotecă (50 puncte)

### Descriere

Implementați un sistem simplu de gestiune a cărților dintr-o bibliotecă folosind o listă simplu înlănțuită. Sistemul trebuie să permită adăugarea, căutarea, împrumutul și returnarea cărților.

### Structura Carte

```c
typedef struct Carte {
    int id;                    /* ID unic al cărții */
    char titlu[100];           /* Titlul cărții */
    char autor[50];            /* Numele autorului */
    int an_publicare;          /* Anul publicării */
    int este_imprumutata;      /* 0 = disponibilă, 1 = împrumutată */
    char imprumutat_de[50];    /* Numele cititorului (dacă e împrumutată) */
    struct Carte *next;
} Carte;
```

### Cerințe Funcționale

| Cerință | Puncte |
|---------|--------|
| 1. Funcția `adauga_carte()` - adaugă o carte nouă în bibliotecă | 8p |
| 2. Funcția `cauta_dupa_titlu()` - caută cărți după titlu (potrivire parțială) | 7p |
| 3. Funcția `cauta_dupa_autor()` - caută cărți după autor | 7p |
| 4. Funcția `imprumuta_carte()` - marchează o carte ca împrumutată | 8p |
| 5. Funcția `returneaza_carte()` - marchează o carte ca returnată | 7p |
| 6. Funcția `afiseaza_disponibile()` - afișează doar cărțile disponibile | 5p |
| 7. Funcția `sterge_carte()` - șterge o carte din sistem | 8p |

### Exemplu de Utilizare

```c
int main(void) {
    Carte *biblioteca = NULL;
    
    adauga_carte(&biblioteca, 1, "Ion", "Liviu Rebreanu", 1920);
    adauga_carte(&biblioteca, 2, "Enigma Otiliei", "George Calinescu", 1938);
    adauga_carte(&biblioteca, 3, "Maitreyi", "Mircea Eliade", 1933);
    
    imprumuta_carte(&biblioteca, 1, "Popescu Ion");
    
    printf("Cărți disponibile:\n");
    afiseaza_disponibile(biblioteca);
    
    returneaza_carte(&biblioteca, 1);
    
    free_biblioteca(&biblioteca);
    return 0;
}
```

### Fișier: `homework1_biblioteca.c`

---

## 📝 Tema 2: Editor de Text Simplificat (50 puncte)

### Descriere

Implementați un editor de text simplificat folosind o listă dublu înlănțuită, unde fiecare nod reprezintă o linie de text. Editorul trebuie să suporte operații de undo folosind o stivă de acțiuni.

### Structura Linie

```c
typedef struct Linie {
    int numar_linie;           /* Numărul liniei (1-indexed) */
    char text[256];            /* Conținutul liniei */
    struct Linie *prev;
    struct Linie *next;
} Linie;

typedef struct {
    Linie *head;
    Linie *tail;
    Linie *cursor;             /* Linia curentă */
    int total_linii;
} Document;
```

### Cerințe Funcționale

| Cerință | Puncte |
|---------|--------|
| 1. Funcția `insereaza_linie()` - inserează o linie nouă după cursor | 8p |
| 2. Funcția `sterge_linie()` - șterge linia curentă | 8p |
| 3. Funcția `modifica_linie()` - modifică textul liniei curente | 7p |
| 4. Funcția `muta_cursor()` - mută cursor-ul sus/jos cu n poziții | 7p |
| 5. Funcția `cauta_text()` - caută un text și mută cursor-ul acolo | 7p |
| 6. Funcția `afiseaza_document()` - afișează documentul cu numere de linie | 5p |
| 7. Funcția `renumeroteaza()` - renumerotează liniile după ștergere/inserare | 8p |

### Exemplu de Utilizare

```c
int main(void) {
    Document doc;
    document_init(&doc);
    
    insereaza_linie(&doc, "Prima linie a documentului.");
    insereaza_linie(&doc, "A doua linie cu text.");
    insereaza_linie(&doc, "Ultima linie.");
    
    muta_cursor(&doc, -1);  /* Sus cu o poziție */
    modifica_linie(&doc, "A doua linie MODIFICATĂ.");
    
    afiseaza_document(&doc);
    
    document_free(&doc);
    return 0;
}
```

### Fișier: `homework2_editor.c`

---

## 📊 Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Corectitudine funcțională | 40 |
| Utilizarea corectă a listelor înlănțuite | 25 |
| Gestiunea cazurilor limită (NULL, liste goale, etc.) | 15 |
| Calitatea codului (stil, comentarii, modularitate) | 10 |
| Compilare fără warning-uri | 10 |

### Penalizări

| Penalizare | Puncte |
|------------|--------|
| Compilare cu warning-uri | -10p |
| Memory leaks detectate de Valgrind | -20p |
| Crash pe input valid | -30p |
| Cod copiat (plagiat) | -50p (minim) |

---

## 📤 Instrucțiuni de Predare

1. Creați un director `tema04_NumePrenume`
2. Includeți fișierele:
   - `homework1_biblioteca.c`
   - `homework2_editor.c`
   - `README.txt` cu instrucțiuni de compilare și rulare
3. Arhivați directorul: `zip -r tema04_NumePrenume.zip tema04_NumePrenume/`
4. Încărcați arhiva pe platforma de cursuri

### Verificare înainte de predare

```bash
# Compilare
gcc -Wall -Wextra -std=c11 -g -o homework1 homework1_biblioteca.c
gcc -Wall -Wextra -std=c11 -g -o homework2 homework2_editor.c

# Verificare memorie
valgrind --leak-check=full ./homework1
valgrind --leak-check=full ./homework2
```

---

## 💡 Sfaturi

1. **Începeți cu structurile de date** - Definiți mai întâi structurile și asigurați-vă că le înțelegeți.

2. **Implementați funcțiile de bază mai întâi** - `create_node`, `free_list`, `print_list`.

3. **Testați incremental** - După fiecare funcție nouă, testați-o separat.

4. **Desenați pe hârtie** - Înainte de a implementa inserarea/ștergerea, desenați diagrame cu pointer-ii.

5. **Folosiți Valgrind frecvent** - Nu așteptați până la final pentru a verifica memory leaks.

6. **Tratați cazurile limită**:
   - Lista goală
   - Un singur element
   - Primul element
   - Ultimul element

---

## 📚 Resurse Utile

- Curs: Slide-urile săptămânii 04
- Laborator: `example1.c` - exemplu complet de referință
- Online: Stanford CS Library - Linked List Basics
- Tool: Visualgo.net pentru vizualizarea operațiilor

---

**Succes!** 🍀

*Echipa ATP - Academia de Studii Economice, București*
