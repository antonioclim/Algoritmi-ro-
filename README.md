# 🎓 Kit Educațional Alternativ: Algoritmi și Structuri de Date în C

<div align="center">

```
╔═══════════════════════════════════════════════════════════════════════════════╗
║                                                                               ║
║     ████████╗ ██████╗ ██╗  ██╗██╗████████╗    ███████╗██████╗ ██╗   ██╗      ║
║     ╚══██╔══╝██╔════╝ ██║  ██║██║╚══██╔══╝    ██╔════╝██╔══██╗██║   ██║      ║
║        ██║   ██║  ███╗███████║██║   ██║       █████╗  ██║  ██║██║   ██║      ║
║        ██║   ██║   ██║██╔══██║██║   ██║       ██╔══╝  ██║  ██║██║   ██║      ║
║        ██║   ╚██████╔╝██║  ██║██║   ██║       ███████╗██████╔╝╚██████╔╝      ║
║        ╚═╝    ╚═════╝ ╚═╝  ╚═╝╚═╝   ╚═╝       ╚══════╝╚═════╝  ╚═════╝       ║
║                                                                               ║
║              KIT EDUCAȚIONAL PENTRU ÎNVĂȚARE NON-FORMALĂ                      ║
║                     Limbajul C · Standard C11 · ANSI                          ║
║                                                                               ║
╚═══════════════════════════════════════════════════════════════════════════════╝
```

**Versiune:** 1.0.0 · **Data:** Ianuarie 2025  
**Limbaj:** C (C11) · **Linii de cod:** 250.000+ · **Fișiere:** 237  

[![License: Restrictivă](https://img.shields.io/badge/Licență-Restrictivă-red.svg)](#licență)
[![Language: C](https://img.shields.io/badge/Limbaj-C11-blue.svg)](#)
[![Platform: Linux/Windows](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-green.svg)](#)

</div>

---

## 📑 Cuprins

1. [Descriere Generală](#-descriere-generală)
2. [Structura Kit-ului](#-structura-kit-ului)
3. [Conținut pe Săptămâni](#-conținut-pe-săptămâni)
4. [Arhitectură și Design](#-arhitectură-și-design)
5. [Ghid de Utilizare](#-ghid-de-utilizare)
6. [Resurse Incluse](#-resurse-incluse)
7. [Statistici Tehnice](#-statistici-tehnice)
8. [Cerințe de Sistem](#-cerințe-de-sistem)
9. [Bibliografie](#-bibliografie)
10. [Licență](#-licență)

---

## 🎯 Descriere Generală

### Ce este acest Kit?

Acest kit educațional reprezintă o **colecție comprehensivă de materiale didactice** destinată învățării **algoritmilor fundamentali și structurilor de date** în limbajul C. Conceput pentru auto-studiu și învățare non-formală, kit-ul oferă o progresie pedagogică de la concepte fundamentale (pointeri, fișiere) către structuri avansate (arbori AVL, grafuri, tabele de dispersie).

### ⚠️ Notă Importantă

> **Acest kit este destinat învățării non-formale și complementare.**  
> Materialele sunt concepute ca resurse suplimentare pentru consolidarea cunoștințelor și nu înlocuiesc participarea la cursuri sau seminariile universitare oficiale.

### Filozofia Pedagogică

Kit-ul urmează o **progresie constructivistă**, în care fiecare săptămână construiește pe fundamentele stabilite anterior:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    TAXONOMIA BLOOM APLICATĂ                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Nivel 6: CREARE        ████████████████████████████  Proiecte integrate   │
│                          └── Săpt. 14: Recapitulare Avansată               │
│                                                                             │
│   Nivel 5: EVALUARE      ███████████████████████       Analiză comparativă  │
│                          └── Săpt. 12-13: Grafuri, Drumuri minime          │
│                                                                             │
│   Nivel 4: ANALIZĂ       ████████████████████         Trade-offs            │
│                          └── Săpt. 9-11: AVL, Heap, Hash                   │
│                                                                             │
│   Nivel 3: APLICARE      ███████████████              Implementare          │
│                          └── Săpt. 5-8: Stive, Cozi, Arbori               │
│                                                                             │
│   Nivel 2: ÎNȚELEGERE    ██████████                   Conceptualizare       │
│                          └── Săpt. 2-4: Fișiere, Liste                     │
│                                                                             │
│   Nivel 1: CUNOAȘTERE    █████                        Memorare              │
│                          └── Săpt. 1: Pointeri, Fundamente                 │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 📂 Structura Kit-ului

### Organizare Ierarhică

```
ATPro-Kit/
│
├── 📁 01-intro-pointers/           # Săptămâna 1: Pointeri la Funcții
├── 📁 02-text-files/               # Săptămâna 2: Fișiere Text
├── 📁 03-binary-files/             # Săptămâna 3: Fișiere Binare
├── 📁 04-relative-files/           # Săptămâna 4: Liste Înlănțuite
├── 📁 05-files-recap/              # Săptămâna 5: Stive (Stack)
├── 📁 06-recursion-divide-impera/  # Săptămâna 6: Cozi (Queues)
├── 📁 07-complexity-intro/         # Săptămâna 7: Arbori Binari
├── 📁 08-sorting-algorithms/       # Săptămâna 8: Arbori Binari de Căutare
├── 📁 09-greedy/                   # Săptămâna 9: Arbori AVL
├── 📁 10-backtracking/             # Săptămâna 10: Heap-uri
├── 📁 11-graphs-traversals/        # Săptămâna 11: Tabele de Dispersie
├── 📁 12-connectivity-dijkstra/    # Săptămâna 12: Grafuri - Fundamente
├── 📁 13-mst-kruskal-prim/         # Săptămâna 13: Drumuri Minime
├── 📁 14-review-exam/              # Săptămâna 14: Recapitulare
│
└── 📄 README.md                    # Acest fișier
```

### Structura Standard per Săptămână

Fiecare modul săptămânal respectă o **arhitectură uniformă**:

```
NN-topic-name/
│
├── 📄 README.md                    # Documentație completă (400-600 linii)
├── 📄 Makefile                     # Automatizare build (200+ linii)
│
├── 📁 slides/
│   ├── presentation-weekNN.html   # Prezentare principală (~2000 linii HTML)
│   └── presentation-comparativ.html # Comparație Pseudocod/C/Python
│
├── 📁 src/
│   ├── example1.c                 # Exemplu demonstrativ complet
│   ├── exercise1.c                # Exercițiu 1 (cu TODO-uri)
│   └── exercise2.c                # Exercițiu 2 (cu TODO-uri)
│
├── 📁 data/
│   └── [fișiere de test specifice]
│
├── 📁 tests/
│   ├── test1_input.txt            # Input pentru testare automată
│   ├── test1_expected.txt         # Output așteptat
│   ├── test2_input.txt
│   └── test2_expected.txt
│
├── 📁 teme/
│   ├── homework-requirements.md   # Cerințe temă (2 × 50 puncte)
│   └── homework-extended.md       # Provocări bonus
│
└── 📁 solution/
    ├── exercise1_sol.c            # Soluție exercițiu 1
    ├── exercise2_sol.c            # Soluție exercițiu 2
    ├── homework1_sol.c            # Soluție temă 1
    └── homework2_sol.c            # Soluție temă 2
```

---

## 📆 Conținut pe Săptămâni

### Vedere de Ansamblu

| Săpt. | Subiect | Concepte Cheie | Complexitate |
|:-----:|---------|----------------|:------------:|
| 01 | **Pointeri la Funcții** | Callbacks, qsort(), bsearch(), dispatch tables | ⭐⭐ |
| 02 | **Fișiere Text** | fopen/fclose, buffering, parsare CSV | ⭐⭐ |
| 03 | **Fișiere Binare** | fread/fwrite, serializare structuri | ⭐⭐⭐ |
| 04 | **Liste Înlănțuite** | Singly/Doubly linked, inserare, ștergere | ⭐⭐⭐ |
| 05 | **Stive (Stack)** | LIFO, push/pop, evaluare expresii | ⭐⭐⭐ |
| 06 | **Cozi (Queue)** | FIFO, circular buffer, BFS | ⭐⭐⭐ |
| 07 | **Arbori Binari** | Parcurgeri, înălțime, echilibrare | ⭐⭐⭐⭐ |
| 08 | **BST** | Inserare, căutare, ștergere | ⭐⭐⭐⭐ |
| 09 | **Arbori AVL** | Rotații LL/RR/LR/RL, echilibrare | ⭐⭐⭐⭐⭐ |
| 10 | **Heap-uri** | Min/Max heap, heapify, priority queue | ⭐⭐⭐⭐ |
| 11 | **Tabele Hash** | Funcții hash, coliziuni, chaining | ⭐⭐⭐⭐ |
| 12 | **Grafuri - Baze** | Reprezentări, BFS, DFS, conectivitate | ⭐⭐⭐⭐ |
| 13 | **Drumuri Minime** | Dijkstra, Bellman-Ford | ⭐⭐⭐⭐⭐ |
| 14 | **Recapitulare** | Integrare, optimizări, probleme complexe | ⭐⭐⭐⭐⭐ |

### Detaliere pe Săptămâni

#### 📌 Săptămâna 01: Pointeri la Funcții și Callbacks

**Obiective de învățare:**
- Declararea și utilizarea pointerilor la funcții
- Implementarea callback-urilor pentru algoritmi generici
- Utilizarea `qsort()` și `bsearch()` din biblioteca standard
- Crearea dispatch tables ca alternativă la switch

**Conținut practic:**
- Calculator cu array de pointeri la funcții
- Sistem de gestiune studenți cu sortări multiple

**Context istoric:** Dennis Ritchie și nașterea limbajului C la Bell Labs (1969-1973)

---

#### 📌 Săptămâna 02: Fișiere Text în C

**Obiective de învățare:**
- Operații de bază cu fișiere text (fopen, fclose, fread, fwrite)
- Strategii de buffering și implicații asupra performanței
- Parsarea fișierelor CSV și configurări INI
- Gestionarea robustă a erorilor I/O

**Conținut practic:**
- Procesor de configurări multi-format
- Analizator de date CSV cu agregări statistice

---

#### 📌 Săptămâna 03: Fișiere Binare în C

**Obiective de învățare:**
- Diferențele fundamentale între format text și binar
- Serializarea și deserializarea structurilor complexe
- Accesul aleator cu fseek() și ftell()
- Portabilitatea datelor și probleme de endianness

**Conținut practic:**
- Sistem de gestiune inventar cu persistență binară
- Convertor format text ↔ binar

---

#### 📌 Săptămâna 04: Liste Înlănțuite (Linked Lists)

**Obiective de învățare:**
- Implementarea listelor simplu și dublu înlănțuite
- Operații: inserare, ștergere, căutare, inversare
- Gestiunea corectă a memoriei dinamice
- Detectarea ciclurilor (algoritmul Floyd)

**Conținut practic:**
- Editor de playlist muzical
- Gestiune de procese cu liste dublu înlănțuite

**Context istoric:** John McCarthy și LISP (1958) - prima utilizare extensivă a listelor înlănțuite

---

#### 📌 Săptămâna 05: Stive (Stack)

**Obiective de învățare:**
- Principiul LIFO (Last In, First Out)
- Implementări: array static vs. listă înlănțuită
- Aplicații: validare paranteze, evaluare expresii
- Call stack-ul și recursivitatea

**Conținut practic:**
- Verificator de paranteze multiple
- Calculator pentru expresii în notație postfixă (RPN)

---

#### 📌 Săptămâna 06: Cozi (Queue)

**Obiective de învățare:**
- Principiul FIFO (First In, First Out)
- Circular buffer și aritmetică modulară
- Priority queues și aplicații
- BFS (Breadth-First Search) pe grafuri

**Conținut practic:**
- Sistem de procesare comenzi restaurant
- Task scheduler cu priorități multiple

**Context istoric:** Agner Krarup Erlang și teoria cozilor de așteptare (1909)

---

#### 📌 Săptămâna 07: Arbori Binari

**Obiective de învățare:**
- Terminologie: rădăcină, frunze, înălțime, adâncime
- Parcurgeri: preorder, inorder, postorder, level-order
- Construcția arborilor din parcurgeri
- Arbori binari completi și perfecți

**Conținut practic:**
- Vizualizator de arbori în terminal
- Constructor de arbori din expresii matematice

---

#### 📌 Săptămâna 08: Arbori Binari de Căutare (BST)

**Obiective de învățare:**
- Proprietatea BST și menținerea acesteia
- Operații: inserare, căutare, ștergere (cele 3 cazuri)
- Găsirea succesorului/predecesorului inorder
- Analiza complexității în cazuri medii și extreme

**Conținut practic:**
- Dicționar cu autocompletare
- Parser și evaluator de expresii cu arbore sintactic

---

#### 📌 Săptămâna 09: Arbori AVL

**Obiective de învățare:**
- Balance factor și condiția AVL
- Cele patru tipuri de rotații (LL, RR, LR, RL)
- Inserarea și ștergerea cu re-echilibrare
- Comparație AVL vs. Red-Black Trees

**Conținut practic:**
- Index de cuvinte cu arbore AVL
- Sistem de leaderboard cu ranking dinamic

**Context istoric:** Adelson-Velsky și Landis (1962) - primul arbore auto-echilibrat

---

#### 📌 Săptămâna 10: Heap-uri și Cozi cu Priorități

**Obiective de învățare:**
- Proprietatea heap (min-heap, max-heap)
- Reprezentarea implicită în array
- Operații: heapify, insert, extract, build-heap
- HeapSort și analiza complexității

**Conținut practic:**
- Sistem de urgențe medicale cu triaj
- Merger de K liste sortate folosind heap

---

#### 📌 Săptămâna 11: Tabele de Dispersie (Hash Tables)

**Obiective de învățare:**
- Funcții hash și proprietățile unei funcții hash bune
- Strategii de rezolvare a coliziunilor: chaining, open addressing
- Factorul de încărcare și rehashing
- Aplicații: numărare frecvențe, eliminare duplicate

**Conținut practic:**
- Dicționar român-englez cu căutare O(1)
- Detector de plagiat cu hash de n-grame

**Context istoric:** Hans Peter Luhn (1953) - inventatorul tabelelor hash

---

#### 📌 Săptămâna 12: Grafuri - Noțiuni Fundamentale

**Obiective de învățare:**
- Terminologie: vârf, muchie, grad, adiacență
- Reprezentări: matrice de adiacență, liste de adiacență
- Parcurgeri: BFS (coadă), DFS (stivă/recursiv)
- Componente conexe și detectarea ciclurilor

**Conținut practic:**
- Analizator de rețele sociale
- Detector de cicluri în dependențe de proiect

---

#### 📌 Săptămâna 13: Drumuri Minime în Grafuri

**Obiective de învățare:**
- Algoritmul Dijkstra pentru grafuri cu ponderi pozitive
- Algoritmul Bellman-Ford pentru ponderi negative
- Detectarea ciclurilor de cost negativ
- Optimizări cu priority queue

**Conținut practic:**
- GPS simplificat cu rutare optimă
- Analizator de arbitraj valutar

**Context istoric:** Edsger Dijkstra (1956) - algoritmul conceput într-o cafenea din Amsterdam

---

#### 📌 Săptămâna 14: Recapitulare și Integrare

**Obiective de învățare:**
- Sinteza structurilor de date și a complexităților
- Alegerea structurii optime pentru diferite scenarii
- Optimizări și trade-off-uri practice
- Pregătire pentru examene și interviuri

**Conținut practic:**
- Mini-proiect integrat: sistem de recomandare
- Simulare examen cu probleme combinate

---

## 🏗️ Arhitectură și Design

### Diagrama Dependențelor Conceptuale

```
                           ┌─────────────────────┐
                           │   14. Recapitulare  │
                           └──────────┬──────────┘
                                      │
              ┌───────────────────────┴───────────────────────┐
              │                                               │
   ┌──────────┴──────────┐                       ┌────────────┴───────────┐
   │  13. Drumuri Minime │                       │   Mai multe structuri  │
   │  (Dijkstra, B-F)    │                       │   combinate            │
   └──────────┬──────────┘                       └────────────────────────┘
              │
   ┌──────────┴──────────┐
   │  12. Grafuri Bază   │
   │  (BFS, DFS)         │
   └──────────┬──────────┘
              │
   ┌──────────┴──────────┐
   │  11. Hash Tables    │◄─────────────────────┐
   └──────────┬──────────┘                      │
              │                                 │
   ┌──────────┴──────────┐              ┌───────┴───────┐
   │  10. Heap-uri       │              │  Funcții Hash │
   └──────────┬──────────┘              └───────────────┘
              │
   ┌──────────┴──────────┐
   │  09. Arbori AVL     │
   │  (auto-echilibrare) │
   └──────────┬──────────┘
              │
   ┌──────────┴──────────┐
   │  08. BST            │
   │  (Binary Search)    │
   └──────────┬──────────┘
              │
   ┌──────────┴──────────┐
   │  07. Arbori Binari  │
   │  (parcurgeri)       │
   └──────────┬──────────┘
              │
      ┌───────┴───────┐
      │               │
┌─────┴─────┐   ┌─────┴─────┐
│ 05. Stive │   │ 06. Cozi  │
│   (LIFO)  │   │   (FIFO)  │
└─────┬─────┘   └─────┬─────┘
      │               │
      └───────┬───────┘
              │
   ┌──────────┴──────────┐
   │  04. Liste Înlănț.  │
   │  (noduri, pointeri) │
   └──────────┬──────────┘
              │
   ┌──────────┴──────────┐
   │  02-03. Fișiere     │
   │  (Text + Binar)     │
   └──────────┬──────────┘
              │
   ┌──────────┴──────────┐
   │  01. Pointeri       │
   │  (fundament)        │
   └─────────────────────┘
```

### Complexități - Tabel Sinteză

| Structură | Acces | Căutare | Inserare | Ștergere | Spațiu |
|-----------|:-----:|:-------:|:--------:|:--------:|:------:|
| **Array** | O(1) | O(n) | O(n) | O(n) | O(n) |
| **Lista Înlănțuită** | O(n) | O(n) | O(1)* | O(1)* | O(n) |
| **Stivă** | O(n) | O(n) | O(1) | O(1) | O(n) |
| **Coadă** | O(n) | O(n) | O(1) | O(1) | O(n) |
| **BST** | O(log n)† | O(log n)† | O(log n)† | O(log n)† | O(n) |
| **AVL** | O(log n) | O(log n) | O(log n) | O(log n) | O(n) |
| **Heap** | O(1)‡ | O(n) | O(log n) | O(log n) | O(n) |
| **Hash Table** | N/A | O(1)§ | O(1)§ | O(1)§ | O(n) |

*\* Cu pointer la poziție · † Cazul mediu, O(n) worst-case · ‡ Doar min/max · § Amortizat*

---

## 🛠️ Ghid de Utilizare

### Compilare și Rulare

Fiecare modul săptămânal include un **Makefile profesional** cu următoarele comenzi:

```bash
# Navigare în directorul săptămânii
cd NN-topic-name/

# Compilare toate sursele
make

# Rulare exemplu demonstrativ
make run

# Rulare exerciții individual
make run-ex1
make run-ex2

# Testare automată
make test

# Verificare memory leaks
make valgrind

# Curățare artefacte
make clean

# Afișare ajutor
make help
```

### Workflow Recomandat pentru Auto-Studiu

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                     FLUX DE ÎNVĂȚARE RECOMANDAT                              │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   1. PREGĂTIRE (30 min)                                                     │
│      ├── Citește README.md complet                                          │
│      ├── Vizualizează slides/presentation-weekNN.html                       │
│      └── Notează conceptele neclare                                         │
│                                                                             │
│   2. STUDIU EXEMPLU (45 min)                                                │
│      ├── Compilează și rulează: make && make run                            │
│      ├── Citește src/example1.c linie cu linie                              │
│      ├── Adaugă printf-uri pentru înțelegere                                │
│      └── Modifică și observă comportamentul                                 │
│                                                                             │
│   3. EXERCIȚII (90 min)                                                     │
│      ├── Deschide src/exercise1.c                                           │
│      ├── Completează secțiunile marcate TODO                                │
│      ├── Testează: make test                                                │
│      └── Repetă pentru exercise2.c                                          │
│                                                                             │
│   4. VERIFICARE (30 min)                                                    │
│      ├── Compară cu solution/exercise*_sol.c                                │
│      ├── Verifică memory leaks: make valgrind                               │
│      └── Încearcă variante alternative de implementare                      │
│                                                                             │
│   5. TEMĂ (2-3 ore)                                                         │
│      ├── Citește teme/homework-requirements.md                              │
│      ├── Implementează homework1 și homework2                               │
│      └── Auto-evaluează folosind criteriile din document                    │
│                                                                             │
│   6. EXTINDERE (opțional)                                                   │
│      ├── Citește teme/homework-extended.md                                  │
│      └── Încearcă provocările bonus                                         │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 📦 Resurse Incluse

### Per Modul Săptămânal

| Componentă | Descriere | Format | Dimensiune Medie |
|------------|-----------|--------|:----------------:|
| README.md | Documentație completă cu teorie, context istoric, exerciții | Markdown | ~450 linii |
| Makefile | Automatizare build cu target-uri colorate | Make | ~230 linii |
| Prezentare principală | Slide-uri interactive HTML standalone | HTML/CSS/JS | ~2000 linii |
| Prezentare comparativă | Pseudocod vs C vs Python | HTML | ~1200 linii |
| Exemplu demonstrativ | Cod C complet funcțional, comentat | C | ~800 linii |
| Exerciții (×2) | Cod schelet cu TODO-uri | C | ~400 linii/fiecare |
| Soluții (×4) | Implementări complete | C | ~300 linii/fiecare |
| Teste automate | Input/output pentru validare | Text | variabil |
| Cerințe temă | Specificații detaliate | Markdown | ~240 linii |
| Provocări extinse | Exerciții bonus avansate | Markdown | ~150 linii |

### Total Kit

| Metrică | Valoare |
|---------|--------:|
| Module săptămânale | 14 |
| Fișiere sursă C | 237 |
| Linii de cod C | 250.000+ |
| Prezentări HTML | 28 |
| Documentație Markdown | 14+ |
| Fișiere de test | 56 |

---

## 📊 Statistici Tehnice

### Distribuția Codului per Modul

```
Săptămâna  │ Linii Cod │ Vizualizare
───────────┼───────────┼──────────────────────────────────────────
01         │    1,928  │ ████████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
02         │    3,563  │ ███████████████░░░░░░░░░░░░░░░░░░░░░░░░░
03         │    3,023  │ ████████████░░░░░░░░░░░░░░░░░░░░░░░░░░░░
04         │    3,314  │ █████████████░░░░░░░░░░░░░░░░░░░░░░░░░░░
05         │    3,114  │ ████████████░░░░░░░░░░░░░░░░░░░░░░░░░░░░
06         │    3,503  │ ██████████████░░░░░░░░░░░░░░░░░░░░░░░░░░
07         │    3,593  │ ██████████████░░░░░░░░░░░░░░░░░░░░░░░░░░
08         │    3,738  │ ███████████████░░░░░░░░░░░░░░░░░░░░░░░░░
09         │    3,622  │ ██████████████░░░░░░░░░░░░░░░░░░░░░░░░░░
10         │    2,652  │ ██████████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
11         │    3,782  │ ███████████████░░░░░░░░░░░░░░░░░░░░░░░░░
12         │    4,019  │ ████████████████░░░░░░░░░░░░░░░░░░░░░░░░
13         │    4,565  │ ██████████████████░░░░░░░░░░░░░░░░░░░░░░
14         │    7,467  │ █████████████████████████████░░░░░░░░░░░
───────────┼───────────┼──────────────────────────────────────────
TOTAL      │  ~52,000  │ (doar src/ și solution/)
```

### Tehnologii Utilizate

| Categorie | Tehnologii |
|-----------|------------|
| **Limbaj** | C (Standard C11, ANSI) |
| **Compilator** | GCC 9+ cu flaguri: `-Wall -Wextra -pedantic -std=c11` |
| **Build System** | GNU Make |
| **Testare Memorie** | Valgrind |
| **Prezentări** | HTML5, CSS3, JavaScript (standalone, offline-capable) |
| **Documentație** | Markdown (GitHub Flavored) |

---

## 💻 Cerințe de Sistem

### Software Necesar

| Component | Versiune Minimă | Verificare |
|-----------|-----------------|------------|
| GCC | 9.0+ | `gcc --version` |
| GNU Make | 4.0+ | `make --version` |
| Valgrind | 3.15+ | `valgrind --version` |
| Browser modern | Chrome/Firefox/Edge actualizat | Pentru prezentări HTML |

### Instalare pe Ubuntu/Debian

```bash
sudo apt update
sudo apt install build-essential valgrind
```

### Instalare pe Windows (WSL2 recomandat)

```bash
# În WSL2 Ubuntu
sudo apt update
sudo apt install build-essential valgrind
```

### Instalare pe macOS

```bash
# Folosind Homebrew
xcode-select --install
brew install valgrind
```

---

## 📚 Bibliografie

### Cărți Fundamentale

1. **Cormen, T.H., Leiserson, C.E., Rivest, R.L., Stein, C.** (2022). *Introduction to Algorithms* (4th Edition). MIT Press.

2. **Sedgewick, R., Wayne, K.** (2011). *Algorithms* (4th Edition). Addison-Wesley.

3. **Kernighan, B.W., Ritchie, D.M.** (1988). *The C Programming Language* (2nd Edition). Prentice Hall.

4. **Knuth, D.E.** (1997). *The Art of Computer Programming, Vol. 1-3*. Addison-Wesley.

5. **Skiena, S.S.** (2020). *The Algorithm Design Manual* (3rd Edition). Springer.

### Resurse Online

- [Visualgo](https://visualgo.net/) - Vizualizări interactive pentru algoritmi
- [GeeksforGeeks](https://www.geeksforgeeks.org/) - Tutoriale și probleme
- [cppreference.com](https://en.cppreference.com/w/c) - Referință completă C
- [Compiler Explorer](https://godbolt.org/) - Analiză cod assembly

---

## ⚖️ Licență

### Termeni și Condiții de Utilizare

```
╔═══════════════════════════════════════════════════════════════════════════════╗
║                           LICENȚĂ RESTRICTIVĂ                                  ║
╠═══════════════════════════════════════════════════════════════════════════════╣
║                                                                               ║
║   © 2025 Antonio Clim. Toate drepturile rezervate.                            ║
║                                                                               ║
║   PERMIS:                                                                     ║
║   ✓ Utilizare personală pentru auto-studiu                                    ║
║   ✓ Vizualizare și rulare cod în scop educațional propriu                     ║
║   ✓ Modificări locale pentru experimentare personală                          ║
║                                                                               ║
║   INTERZIS (fără acord scris prealabil):                                      ║
║   ✗ Publicarea materialelor (online sau offline)                              ║
║   ✗ Utilizarea în cadrul activităților didactice formale                      ║
║   ✗ Predarea sau prezentarea materialelor către terți                         ║
║   ✗ Redistribuirea în orice formă                                             ║
║   ✗ Crearea de lucrări derivate pentru uz public                              ║
║   ✗ Utilizarea comercială de orice natură                                     ║
║                                                                               ║
║   Pentru solicitări de utilizare în scop didactic sau publicare,              ║
║   contactați autorul pentru obținerea acordului scris.                        ║
║                                                                               ║
╚═══════════════════════════════════════════════════════════════════════════════╝
```

---

## 👤 Autor

<div align="center">

**Asistent (pe perioadă determinată) ing. dr. Antonio Clim**

*Academia de Studii Economice din București*  
*Facultatea de Cibernetică, Statistică și Informatică Economică*  
*Departamentul de Informatică Economică*

---

*Kit educațional creat pentru învățare non-formală*  
*Ianuarie 2025*

</div>
