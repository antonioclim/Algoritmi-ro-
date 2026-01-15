# Săptămâna 02: Fișiere Text în C

## 🎯 Obiective de Învățare

La finalul acestui laborator, studenții vor fi capabili să:

1. **Recunoască** și să descrie diferitele moduri de deschidere a fișierelor (`"r"`, `"w"`, `"a"`, `"r+"`, `"w+"`, `"a+"`) și implicațiile fiecăruia
2. **Înțeleagă** mecanismul de buffering în operațiile I/O și diferențele dintre stream-urile cu buffer și cele fără buffer
3. **Aplice** funcțiile standard pentru citirea și scrierea datelor în fișiere text (`fopen`, `fclose`, `fgets`, `fputs`, `fprintf`, `fscanf`)
4. **Analizeze** structura datelor din fișiere text complexe și să implementeze parsere robuste pentru formate variate (CSV, configurări, log-uri)
5. **Evalueze** diverse abordări de procesare a fișierelor și să aleagă strategia optimă în funcție de dimensiunea datelor și cerințele de performanță
6. **Creeze** aplicații complete care procesează seturi mari de date structurate, implementând validare, transformări și raportare

---

## 📜 Context Istoric

Conceptul de fișier ca abstracție pentru stocarea persistentă a datelor își are originile în primele sisteme de operare din anii 1950-1960. Sistemul UNIX, dezvoltat la Bell Labs începând cu 1969 de către Ken Thompson și Dennis Ritchie, a revoluționat modul în care programele interacționează cu fișierele prin introducerea filosofiei "totul este un fișier" (*everything is a file*).

Biblioteca standard de intrare/ieșire a limbajului C (`stdio.h`) a fost concepută de Dennis Ritchie și Mike Lesk în jurul anului 1973, odată cu dezvoltarea limbajului C pentru rescrierea nucleului UNIX. Abstracția `FILE*` și conceptul de stream au permis portabilitatea codului între sisteme diferite, ascunzând detaliile specifice fiecărui sistem de operare.

Formatul CSV (Comma-Separated Values) datează din epoca primelor computere mainframe din anii 1960-1970, când era folosit pentru transferul datelor între programe incompatibile. Standardizarea parțială a venit abia în 2005 prin RFC 4180, însă variațiile de implementare persistă și astăzi.

### Figură Cheie: Dennis Ritchie (1941-2011)

Dennis MacAlistair Ritchie, cunoscut și ca "dmr", a fost un informatician american care a creat limbajul de programare C și a co-dezvoltat sistemul de operare UNIX împreună cu Ken Thompson. A lucrat cea mai mare parte a carierei la Bell Labs, unde contribuțiile sale au pus bazele informaticii moderne.

> *"UNIX is very simple, it just needs a genius to understand its simplicity."*
> — Dennis Ritchie

Ritchie a primit numeroase distincții, inclusiv Premiul Turing (1983, împreună cu Ken Thompson) și Medalia Națională pentru Tehnologie (1998). Contribuțiile sale la biblioteca `stdio` rămân fundamentale pentru programarea sistemelor și astăzi.

---

## 📚 Fundamente Teoretice

### 1. Anatomia unui Stream în C

Un stream (flux de date) reprezintă o abstracție care permite programelor să interacționeze uniform cu diverse surse și destinații de date: fișiere pe disc, terminale, conexiuni de rețea sau dispozitive hardware.

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        STRUCTURA UNUI FILE STREAM                       │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   Program C                    Buffer                   Fișier pe Disc  │
│  ┌─────────┐               ┌──────────────┐            ┌─────────────┐  │
│  │         │  fprintf()    │              │  flush()   │             │  │
│  │ Codul   │ ────────────► │   Buffer     │ ─────────► │   date.txt  │  │
│  │ Sursă   │               │   (4KB)      │            │             │  │
│  │         │  fscanf()     │              │  read()    │             │  │
│  │         │ ◄──────────── │              │ ◄───────── │             │  │
│  └─────────┘               └──────────────┘            └─────────────┘  │
│                                                                         │
│  Structura FILE (simplificată):                                         │
│  ┌────────────────────────────────────────────────────────────────┐     │
│  │  int   fd;           // file descriptor (sistem de operare)   │     │
│  │  char* buffer;       // pointer la zona de buffer             │     │
│  │  int   bufsize;      // dimensiunea buffer-ului               │     │
│  │  char* ptr;          // poziția curentă în buffer             │     │
│  │  int   cnt;          // caractere rămase în buffer            │     │
│  │  int   flags;        // stare: EOF, eroare, mod de buffering  │     │
│  └────────────────────────────────────────────────────────────────┘     │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 2. Modurile de Deschidere a Fișierelor

| Mod | Descriere | Dacă fișierul există | Dacă nu există |
|-----|-----------|---------------------|----------------|
| `"r"` | Read only | Deschide pentru citire | Eroare (NULL) |
| `"w"` | Write only | Șterge conținutul | Creează nou |
| `"a"` | Append | Scrie la sfârșit | Creează nou |
| `"r+"` | Read + Write | Deschide pentru ambele | Eroare (NULL) |
| `"w+"` | Write + Read | Șterge conținutul | Creează nou |
| `"a+"` | Append + Read | Scrie la sfârșit, citește oriunde | Creează nou |

```c
/* Exemplu: Deschiderea sigură a unui fișier */
FILE *fp = fopen("date.txt", "r");
if (fp == NULL) {
    perror("Eroare la deschiderea fișierului");
    exit(EXIT_FAILURE);
}
/* ... operații cu fișierul ... */
fclose(fp);
```

### 3. Strategii de Buffering

Biblioteca `stdio` oferă trei strategii de buffering:

| Tip | Constantă | Comportament | Utilizare tipică |
|-----|-----------|--------------|------------------|
| Full buffering | `_IOFBF` | Flush când buffer-ul e plin | Fișiere pe disc |
| Line buffering | `_IOLBF` | Flush la newline | stdout când e terminal |
| No buffering | `_IONBF` | Fără buffer, scriere imediată | stderr |

```c
/* Configurarea buffer-ului pentru un stream */
char buffer[8192];
setvbuf(fp, buffer, _IOFBF, sizeof(buffer));

/* Forțarea scrierii buffer-ului pe disc */
fflush(fp);
```

### 4. Funcții Fundamentale pentru Citire

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    IERARHIA FUNCȚIILOR DE CITIRE                        │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   Nivel Înalt (formatat)                                                │
│   ├── fscanf()    ─── citire formatată din stream                       │
│   └── scanf()     ─── citire formatată din stdin                        │
│                                                                         │
│   Nivel Mediu (linie/string)                                            │
│   ├── fgets()     ─── citire linie (SIGUR, recomandat)                  │
│   ├── gets()      ─── DEPRECIAT! Buffer overflow vulnerabil             │
│   └── getline()   ─── extensie POSIX, alocare dinamică                  │
│                                                                         │
│   Nivel Scăzut (caracter)                                               │
│   ├── fgetc()     ─── citire caracter din stream                        │
│   ├── getc()      ─── macro, echivalent cu fgetc()                      │
│   └── getchar()   ─── citire caracter din stdin                         │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 5. Analiza Complexității Operațiilor I/O

| Operație | Complexitate Timp | Note |
|----------|-------------------|------|
| `fopen()` | O(1) | Apel sistem |
| `fclose()` | O(n) | n = date în buffer nescrise |
| `fgetc()` | O(1) amortizat | Un acces la disc per buffer |
| `fgets()` | O(k) | k = lungimea liniei |
| `fscanf()` | O(k) | k = lungimea input-ului |
| `fseek()` | O(1) | Doar modifică indicatorul |
| Parsare CSV completă | O(n × m) | n linii, m câmpuri per linie |

---

## 🏭 Aplicații Industriale

### 1. Procesarea Log-urilor (Unix/Linux)

```c
/* Parsarea unui fișier de log în format Apache */
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct {
    char ip[16];
    char timestamp[32];
    char method[8];
    char path[256];
    int status;
    long bytes;
} LogEntry;

int parse_log_line(const char *line, LogEntry *entry) {
    /* Format: 127.0.0.1 - - [10/Oct/2023:13:55:36] "GET /index.html HTTP/1.1" 200 2326 */
    return sscanf(line, "%15s %*s %*s [%31[^]]] \"%7s %255s %*[^\"]\" %d %ld",
                  entry->ip, entry->timestamp, entry->method, 
                  entry->path, &entry->status, &entry->bytes);
}
```

### 2. Fișiere de Configurare (INI Parser)

```c
/* Citirea configurațiilor în format INI */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char section[64];
    char key[64];
    char value[256];
} ConfigEntry;

int parse_ini_line(const char *line, ConfigEntry *entry) {
    /* Ignoră liniile goale și comentariile */
    while (isspace(*line)) line++;
    if (*line == '\0' || *line == ';' || *line == '#') 
        return 0;
    
    /* Secțiune: [SectionName] */
    if (*line == '[') {
        return sscanf(line, "[%63[^]]]", entry->section);
    }
    
    /* Pereche cheie=valoare */
    return sscanf(line, "%63[^=]=%255[^\n]", entry->key, entry->value);
}
```

### 3. Export Date pentru Raportare (CSV Writer)

```c
/* Generarea unui raport CSV din structuri de date */
void export_to_csv(const char *filename, Student students[], int count) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return;
    
    /* Header cu BOM pentru Excel (opțional) */
    fprintf(fp, "\xEF\xBB\xBF");  /* UTF-8 BOM */
    fprintf(fp, "ID,Nume,Prenume,Medie,Status\n");
    
    for (int i = 0; i < count; i++) {
        /* Escapare ghilimele în câmpuri */
        fprintf(fp, "%d,\"%s\",\"%s\",%.2f,%s\n",
                students[i].id,
                students[i].nume,
                students[i].prenume,
                students[i].medie,
                students[i].medie >= 5.0 ? "Promovat" : "Nepromovat");
    }
    
    fclose(fp);
}
```

### 4. Validare Input pentru Formulare Web (CGI)

```c
/* Parsarea datelor POST în format application/x-www-form-urlencoded */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void url_decode(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if (*src == '%' && (a = src[1]) && (b = src[2]) &&
            isxdigit(a) && isxdigit(b)) {
            *dst++ = (char)(16 * (isdigit(a) ? a - '0' : toupper(a) - 'A' + 10) +
                                 (isdigit(b) ? b - '0' : toupper(b) - 'A' + 10));
            src += 3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}
```

---

## 💻 Exerciții de Laborator

### Exercițiu 1: Analizator de Note Studenți

**Obiectiv**: Implementarea unui sistem complet de procesare a datelor studențești din fișiere CSV.

**Cerințe**:
1. Citirea datelor din fișierul `studgrades.csv` care conține: ID, Nume, Prenume, An, Grupă și 10 note
2. Calcularea mediei pentru fiecare student (ignorând notele de 0)
3. Generarea unui raport în fișierul `report.txt` cu:
   - Lista studenților ordonați după medie (descrescător)
   - Statistici: medie generală, număr promovați/nepromovați
   - Top 3 studenți cu cele mai mari medii
4. Tratarea erorilor: fișier inexistent, format invalid, câmpuri lipsă

**Structura de Date**:
```c
typedef struct {
    int id;
    char nume[50];
    char prenume[50];
    int an;
    int grupa;
    int note[10];
    double medie;
} Student;
```

### Exercițiu 2: Parser de Configurație Multi-Format

**Obiectiv**: Crearea unui parser robust pentru fișiere de configurație în diverse formate.

**Cerințe**:
1. Suport pentru trei formate de intrare:
   - Format simplu: `CHEIE=valoare`
   - Format INI: `[Secțiune]` urmat de perechi cheie-valoare
   - Format comentat: ignorare linii care încep cu `#` sau `;`
2. Validarea tipurilor de date: întreg, real, string, boolean
3. Suport pentru valori pe mai multe linii (continuare cu `\`)
4. Interpolarea variabilelor: `${ALTA_CHEIE}` înlocuit cu valoarea
5. Export în format JSON simplificat

**Exemplu Input**:
```ini
[Database]
host = localhost
port = 5432
# Comentariu
name = aplicatie_db

[Server]
address = ${Database.host}
timeout = 30
```

---

## 🔧 Compilare și Execuție

```bash
# Construirea tuturor executabilelor
make all

# Rularea exemplului demonstrativ
make run-example

# Rularea exercițiului 1
make run-ex1

# Rularea exercițiului 2  
make run-ex2

# Testare automată
make test

# Verificare memory leaks cu Valgrind
make valgrind

# Curățare fișiere generate
make clean

# Afișare ajutor
make help
```

---

## 📁 Structura Directorului

```
week-02-fisiere-text/
├── README.md                           # Acest fișier
├── Makefile                            # Automatizare compilare
│
├── slides/
│   ├── presentation-week02.html        # Prezentare principală (35+ slide-uri)
│   └── presentation-comparativ.html    # Comparație Pseudocod/C/Python
│
├── src/
│   ├── example1.c                      # Exemplu complet demonstrativ
│   ├── exercise1.c                     # Analizator note (cu TODO)
│   └── exercise2.c                     # Parser configurație (cu TODO)
│
├── data/
│   ├── studgrades.csv                  # Date studenți pentru Ex1
│   ├── config_sample.ini               # Configurație pentru Ex2
│   └── 3bgrades.csv                    # Date adiționale
│
├── tests/
│   ├── test1_input.txt                 # Input test Ex1
│   ├── test1_expected.txt              # Output așteptat Ex1
│   ├── test2_input.txt                 # Input test Ex2
│   └── test2_expected.txt              # Output așteptat Ex2
│
├── teme/
│   ├── homework-requirements.md        # 2 teme (50p fiecare)
│   └── homework-extended.md            # 5 provocări bonus (+10p fiecare)
│
└── solution/
    ├── exercise1_sol.c                 # Soluție completă Ex1
    ├── exercise2_sol.c                 # Soluție completă Ex2
    ├── homework1_sol.c                 # Soluție Tema 1
    └── homework2_sol.c                 # Soluție Tema 2
```

---

## 📖 Lectură Recomandată

### Resurse Esențiale
- **Kernighan & Ritchie** - *The C Programming Language*, Capitolul 7: Input and Output
- **Steve Summit** - *C Programming Notes*, Secțiunea despre stdio
- **GNU C Library Manual** - Capitolul 12: Input/Output on Streams

### Resurse Avansate
- **Advanced Programming in the UNIX Environment** (Stevens & Rago) - Capitolul 5: Standard I/O Library
- **RFC 4180** - Common Format and MIME Type for CSV Files
- **POSIX.1-2017** - Specificația completă pentru funcțiile de I/O

### Resurse Online
- [cppreference.com - File input/output](https://en.cppreference.com/w/c/io)
- [GNU Libc Manual - I/O on Streams](https://www.gnu.org/software/libc/manual/html_node/I_002fO-on-Streams.html)
- [Beej's Guide to C - File I/O](https://beej.us/guide/bgc/html/split/file-io.html)

---

## ✅ Lista de Auto-Evaluare

Înainte de a considera laboratorul finalizat, verifică că poți:

- [ ] Explica diferența dintre `fgets()` și `scanf()` pentru citirea liniilor
- [ ] Deschide un fișier în modul corect pentru o sarcină dată
- [ ] Verifica și trata erorile la operațiile cu fișiere
- [ ] Implementa un parser CSV care gestionează ghilimelele și virgulele în câmpuri
- [ ] Folosi `fseek()` și `ftell()` pentru navigarea în fișier
- [ ] Configura buffering-ul pentru optimizarea performanței
- [ ] Evita memory leaks prin închiderea corectă a fișierelor
- [ ] Scrie date structurate într-un format standardizat (CSV, INI)
- [ ] Depana probleme comune: EOF prematur, buffer overflow, format greșit
- [ ] Compila codul fără warnings cu `-Wall -Wextra`

---

## 💼 Pregătire pentru Interviuri

### Întrebări Frecvente

1. **Care este diferența dintre `fread()`/`fwrite()` și `fscanf()`/`fprintf()`?**
   
   *Răspuns așteptat*: `fread/fwrite` operează la nivel de bytes (raw binary), citind/scriind exact numărul specificat de bytes, fără interpretare. `fscanf/fprintf` sunt funcții formatate care interpretează datele conform specificatorilor de format (%d, %s, etc.) și gestionează conversii între reprezentarea text și cea internă.

2. **De ce `gets()` este considerat periculos?**
   
   *Răspuns așteptat*: `gets()` nu verifică lungimea buffer-ului destinație, permițând buffer overflow. Un input mai lung decât buffer-ul poate suprascrie memoria adiacentă, inclusiv adresa de return, permițând atacuri de tip stack smashing. Funcția a fost eliminată din standardul C11.

3. **Cum gestionezi un fișier CSV care conține virgule în câmpuri?**
   
   *Răspuns așteptat*: Câmpurile care conțin caractere speciale (virgulă, newline, ghilimele) trebuie încadrate în ghilimele duble. Ghilimelele din interiorul unui câmp se escapează prin dublare (""). Parser-ul trebuie să implementeze o mașină de stări pentru a distinge între delimitatori și caractere literale.

4. **Ce se întâmplă dacă nu apelezi `fclose()` pentru un fișier deschis?**
   
   *Răspuns așteptat*: Datele din buffer pot rămâne nescrise pe disc (data loss), file descriptor-ul rămâne ocupat (resurse sistem limitate), iar în cazuri extreme sistemul poate refuza deschiderea altor fișiere. La terminarea normală a programului, sistemul de operare eliberează resursele, dar un crash poate duce la pierderea datelor.

5. **Explică avantajele și dezavantajele diferitelor strategii de buffering.**
   
   *Răspuns așteptat*: Full buffering minimizează apelurile sistem și este optim pentru fișiere mari, dar poate întârzia vizibilitatea datelor. Line buffering oferă un compromis bun pentru output interactiv. No buffering garantează scrierea imediată dar are overhead maxim pentru operații multiple.

---

## 🔗 Previzualizare Săptămâna Următoare

**Săptămâna 3: Fișiere Binare**

Vom explora operațiile I/O la nivel binar: citirea și scrierea structurilor direct în/din fișiere, serialization/deserialization, formatul fișierelor binare și accesul direct (random access) folosind `fseek()` și `ftell()`. Vom implementa o bază de date simplă cu indexare.

Concepte cheie: `fread()`, `fwrite()`, endianness, padding, persistența structurilor de date.

---

*Acest material a fost creat pentru cursul "Algoritmi și Tehnici de Programare" (ATP), Academia de Studii Economice București, Facultatea CSIE.*
