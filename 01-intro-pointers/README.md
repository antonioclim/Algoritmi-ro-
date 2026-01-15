# Săptămâna 01: Pointeri la Funcții și Callbacks în C

## 🎯 Obiective de Învățare

La finalul acestei sesiuni de laborator, studenții vor putea:

1. **Cunoaște** sintaxa pentru declararea pointerilor la funcții în C
2. **Înțelege** de ce pointerii la funcții permit programare generică
3. **Aplica** `qsort()` și `bsearch()` cu funcții comparator personalizate
4. **Analiza** compromisurile între diferite implementări de comparatoare
5. **Evalua** când să folosești dispatch tables vs instrucțiuni switch
6. **Crea** soluții bazate pe callback pentru probleme din lumea reală

---

## 📜 Context Istoric

### Nașterea Limbajului C (1969-1973)

Dennis Ritchie a dezvoltat C la Bell Labs alături de sistemul de operare Unix. Geniul său a fost crearea unui limbaj care mapează direct la operațiile mașinii, rămânând în același timp lizibil și portabil. Pointerii la funcții în C oglindesc direct **instrucțiunile de salt indirect** din limbajul de asamblare—sintaxa `(*fptr)()` se compilează în încărcarea unei adrese într-un registru și executarea unui apel indirect.

### Figură Cheie: Dennis Ritchie (1941-2011)

- Creatorul limbajului de programare C
- Co-creator al Unix (cu Ken Thompson)
- Laureat al Premiului Turing (1983)
- Autor al „The C Programming Language" (K&R)

> *„C nu este un limbaj de nivel „foarte înalt", nici unul „mare", și nu este specializat pentru nicio zonă particulară de aplicație. Dar absența restricțiilor și generalitatea sa îl fac mai convenabil și mai eficient pentru multe sarcini decât limbajele presupus mai puternice."*
> — K&R, Prefața Primei Ediții

---

## 📚 Fundamente Teoretice

### 1. Ce este un Pointer la Funcție?

Un **pointer la funcție** este o variabilă care stochează adresa de memorie a unei funcții. La fel cum pointerii la date indică spre date în memorie, pointerii la funcții indică spre cod executabil.

```
┌─────────────────┐     ┌─────────────────┐
│ Segment Cod     │     │   Stack/Date    │
├─────────────────┤     ├─────────────────┤
│ 0x4005f0: add() │ ←── │ fptr: 0x4005f0  │
│ 0x400610: sub() │     │                 │
│ 0x400630: mul() │     │                 │
└─────────────────┘     └─────────────────┘
```

### 2. Sintaxa Declarației

```c
// Sintaxă de bază
tip_returnat (*nume_pointer)(tipuri_parametri);

// Exemple
int (*operatie_binara)(int, int);           // Primește 2 int, returnează int
void (*callback)(void);                      // Fără parametri, fără return
int (*comparator)(const void*, const void*); // Stil qsort
```

**Critic:** Parantezele din jurul `*nume_pointer` sunt obligatorii!

| Declarație | Semnificație |
|------------|--------------|
| `int (*fp)(int)` | Pointer la funcție care primește int, returnează int |
| `int *fp(int)` | Funcție care primește int, returnează pointer la int |

### 3. Pattern-ul Callback

Un **callback** este o funcție transmisă ca argument unei alte funcții, pentru a fi „apelată înapoi" mai târziu:

```
Codul Tău → Funcție Bibliotecă → Callback (Codul Tău)
```

Aceasta permite:
- **Algoritmi generici** (sortează orice tip de date)
- **Gestionarea evenimentelor** (GUI, rețea)
- **Inversarea controlului** (framework-uri)

### 4. qsort() și bsearch()

Din `<stdlib.h>`:

```c
void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *));

void *bsearch(const void *key, const void *base, size_t nmemb,
              size_t size, int (*compar)(const void *, const void *));
```

**Contractul Comparatorului (POSIX):**
- Returnează `< 0` dacă primul element vine înainte de al doilea
- Returnează `0` dacă elementele sunt egale
- Returnează `> 0` dacă primul element vine după al doilea

**Complexitate:**
| Funcție | Complexitate Timp | Complexitate Spațiu |
|---------|-------------------|---------------------|
| qsort() | O(n log n) mediu, O(n²) cel mai rău | O(log n) |
| bsearch() | O(log n) | O(1) |

### 5. Pattern Comparator Sigur

**Atenție:** Scăderea simplă poate face overflow cu întregi mari!

```c
// NESIGUR pentru valori mari
int cmp_nesigur(const void *a, const void *b) {
    return *(int*)a - *(int*)b;  // Poate face overflow!
}

// Pattern SIGUR
int cmp_sigur(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}
```

---

## 🏭 Aplicații Industriale

### GTK/GLib (Toolkit GUI Linux)
```c
g_signal_connect(buton, "clicked", G_CALLBACK(la_click_buton), date);
```

### Win32 API (Windows)
```c
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
```

### Gestionarea Semnalelor Unix
```c
signal(SIGINT, gestioneaza_intrerupere);
```

### Node.js / libuv
Întreaga buclă de evenimente Node.js este implementată în C folosind callback-uri cu pointeri la funcții pentru operații I/O asincrone.

---

## 💻 Exerciții de Laborator

### Exercițiul 1: Calculator cu Dispatch Table
Implementează un calculator folosind un array de pointeri la funcții.

**Cerințe:**
- Suportă operațiile: +, -, *, /, %
- Gestionează împărțirea la zero cu grație
- Folosește un dispatch table (nu instrucțiuni switch)

**Fișier:** `src/exercise1.c`

### Exercițiul 2: Bază de Date Studenți cu Sortare
Creează un sistem de gestiune studenți folosind qsort() și bsearch().

**Cerințe:**
- Definește o structură Student (id, nume, nota)
- Implementează comparatoare pentru sortare după fiecare câmp
- Caută studenți după nume folosind bsearch()
- Afișează top N studenți după notă

**Fișier:** `src/exercise2.c`

---

## 🔧 Compilare și Rulare

```bash
# Compilează toate sursele
make

# Rulează toate executabilele
make run

# Rulează testele automate
make test

# Verifică scurgeri de memorie
make valgrind

# Curăță artefactele de compilare
make clean

# Afișează ajutor
make help
```

---

## 📁 Structura Directorului

```
week-01-pointers-callbacks/
├── README.md                 # Acest fișier
├── Makefile                  # Automatizare compilare
├── slides/
│   ├── presentation-week01.html      # Slide-uri lecție principală
│   └── presentation-comparativ.html  # Comparație C vs Python
├── src/
│   ├── example1.c            # Exemplu complet funcțional
│   ├── exercise1.c           # Exercițiu calculator (TODO)
│   └── exercise2.c           # Exercițiu bază de date (TODO)
├── data/
│   ├── studenti.txt          # Date exemplu studenți
│   └── numere.txt            # Numere exemplu pentru sortare
├── tests/
│   ├── test1_input.txt       # Input test pentru exercițiul 1
│   ├── test1_expected.txt    # Output așteptat pentru exercițiul 1
│   ├── test2_input.txt       # Input test pentru exercițiul 2
│   └── test2_expected.txt    # Output așteptat pentru exercițiul 2
├── teme/
│   ├── teme-cerinte.md       # Cerințe temă
│   └── teme-extinse.md       # Provocări extinse
└── solution/
    ├── exercise1_sol.c       # Soluție pentru exercițiul 1
    ├── exercise2_sol.c       # Soluție pentru exercițiul 2
    ├── tema1_sol.c           # Soluție temă 1
    └── tema2_sol.c           # Soluție temă 2
```

---

## 📖 Lectură Recomandată

### Esențial
- **K&R** Capitol 5.11: Pointeri la Funcții
- **man qsort** - Documentație sistem
- **man bsearch** - Documentație căutare binară

### Avansat
- **Modern C** de Jens Gustedt - Capitol 11
- **CLRS** - Introduction to Algorithms (teoria sortării)
- **Expert C Programming** de Peter van der Linden

### Resurse Online
- [cppreference.com](https://en.cppreference.com/w/c) - Biblioteca standard C
- [cdecl.org](https://cdecl.org/) - Decodează declarații complexe C
- [godbolt.org](https://godbolt.org/) - Compiler Explorer

---

## ✅ Listă de Auto-Verificare

După completarea acestui laborator, verifică că poți:

- [ ] Declara un pointer la funcție cu sintaxă corectă
- [ ] Atribui o funcție unui pointer și o apela
- [ ] Explica diferența între `int (*f)(int)` și `int *f(int)`
- [ ] Scrie o funcție comparator pentru qsort()
- [ ] Folosi bsearch() pe un array sortat
- [ ] Implementa un dispatch table cu array de pointeri la funcții
- [ ] Explica de ce callback-urile permit programare generică
- [ ] Trece toate testele automate: `make test`

---

## 💼 Pregătire pentru Interviuri

Întrebări frecvente despre pointeri la funcții:

1. **Declară** un pointer la o funcție care primește doi int și returnează int
2. **Implementează** un dispatch table pentru un calculator
3. **Explică** `int (*fp)(int)` vs `int *fp(int)`
4. **Scrie** un comparator pentru sortarea structurilor după mai multe câmpuri
5. **Debug** de ce bsearch() returnează NULL (hint: este array-ul sortat?)

---

## 🔗 Previzualizare Săptămâna Viitoare

**Săptămâna 02: Fișiere Text în C**
- I/O pe fișiere cu stdio.h
- Strategii de buffering
- Tehnici de parsare text
- Pattern-uri de gestionare a erorilor

---

*Curs ATP | ASE-CSIE | Departamentul de Informatică Economică*
