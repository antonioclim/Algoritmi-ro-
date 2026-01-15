# Săptămâna 03: Fișiere Binare în C

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, veți fi capabili să:

1. **Identificați** diferențele fundamentale dintre fișierele text și fișierele binare la nivel de reprezentare internă și cazuri de utilizare
2. **Explicați** mecanismul de funcționare al funcțiilor `fread()`, `fwrite()`, `fseek()` și `ftell()` în contextul manipulării fluxurilor de date
3. **Aplicați** tehnici de serializare și deserializare pentru structuri de date complexe în format binar
4. **Analizați** implicațiile endianness-ului și ale padding-ului structurilor asupra portabilității datelor binare
5. **Evaluați** compromisurile dintre fișiere text și binare în funcție de criterii precum performanță, portabilitate și depanabilitate
6. **Creați** sisteme complete de persistență a datelor utilizând fișiere binare cu suport pentru operații CRUD și indexare

---

## 📜 Context Istoric

Conceptul de fișier binar își are originile în primele sisteme de calcul din anii 1950, când memoria și spațiul de stocare erau resurse extrem de limitate și costisitoare. Spre deosebire de reprezentarea textuală, care necesită conversii succesive între formatul intern și cel uman-citibil, fișierele binare permit stocarea datelor în formatul nativ al procesorului.

În 1972, Dennis Ritchie și Ken Thompson au dezvoltat limbajul C la Bell Labs, introducând conceptul de „stream" (flux de date) ca abstracție uniformă pentru operațiile de intrare/ieșire. Funcțiile `fread()` și `fwrite()` au fost concepute pentru a permite transferul eficient de blocuri de date între memorie și dispozitivele de stocare, fără overhead-ul conversiei la format text.

Evoluția sistemelor de baze de date, de la primele sisteme ierarhice (IMS de la IBM, 1966) până la bazele de date relaționale moderne, s-a bazat fundamental pe capacitatea de a manipula eficient fișiere binare. Formatul binar rămâne esențial în domenii precum: sisteme de operare (fișiere executabile ELF/PE), multimedia (codec-uri audio/video), rețelistică (protocoale binare) și sisteme embedded cu resurse limitate.

### Figură Cheie: Dennis Ritchie (1941-2011)

Dennis MacAlistair Ritchie, cunoscut și ca „dmr", a fost un informatician american care a creat limbajul C și a co-dezvoltat sistemul de operare UNIX împreună cu Ken Thompson. Contribuțiile sale la teoria și practica sistemelor de operare, incluzând abstracția elegantă a operațiilor cu fișiere, au modelat fundamental informatica modernă.

> *„UNIX este simplu. Trebuie doar să fii un geniu ca să-i înțelegi simplitatea."*
> — Dennis Ritchie

---

## 📚 Fundamente Teoretice

### 1. Anatomia Fișierelor Binare vs. Text

Diferența fundamentală dintre cele două tipuri de fișiere constă în modul de reprezentare a datelor:

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    COMPARAȚIE: TEXT vs. BINAR                           │
├─────────────────────────────────────────────────────────────────────────┤
│  Numărul 12345 ca...                                                    │
│                                                                         │
│  FIȘIER TEXT (ASCII):                                                   │
│  ┌─────┬─────┬─────┬─────┬─────┐                                       │
│  │ '1' │ '2' │ '3' │ '4' │ '5' │  = 5 bytes                            │
│  │ 0x31│ 0x32│ 0x33│ 0x34│ 0x35│                                       │
│  └─────┴─────┴─────┴─────┴─────┘                                       │
│                                                                         │
│  FIȘIER BINAR (int pe 32 biți, little-endian):                         │
│  ┌─────┬─────┬─────┬─────┐                                             │
│  │ 0x39│ 0x30│ 0x00│ 0x00│  = 4 bytes (12345 = 0x00003039)            │
│  └─────┴─────┴─────┴─────┘                                             │
│                                                                         │
│  Economie de spațiu: ~20% pentru numere mici, mult mai mult            │
│  pentru numere mari sau structuri complexe                              │
└─────────────────────────────────────────────────────────────────────────┘
```

### 2. Funcții Standard pentru I/O Binar

Biblioteca standard C oferă un set de funcții specializate pentru manipularea fișierelor binare:

```c
// Citire din fișier binar
size_t fread(void *ptr, size_t size, size_t count, FILE *stream);

// Scriere în fișier binar  
size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);

// Poziționare în fișier
int fseek(FILE *stream, long offset, int whence);

// Obținerea poziției curente
long ftell(FILE *stream);

// Revenire la începutul fișierului
void rewind(FILE *stream);
```

**Parametrii funcției `fseek()`:**
- `SEEK_SET` (0): Offset de la începutul fișierului
- `SEEK_CUR` (1): Offset de la poziția curentă
- `SEEK_END` (2): Offset de la sfârșitul fișierului

### 3. Serializarea Structurilor de Date

Serializarea reprezintă procesul de conversie a structurilor de date din memorie într-un format liniar, potrivit pentru stocare sau transmisie. Deserializarea este operația inversă.

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    PROCESUL DE SERIALIZARE                              │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   MEMORIE (struct)              FIȘIER BINAR                           │
│   ┌─────────────────┐           ┌─────────────────┐                    │
│   │ id: 1001        │           │ E9 03 00 00     │ ← id (4 bytes)     │
│   │ nume: "Popescu" │  ──────►  │ 50 6F 70 65 73  │ ← nume            │
│   │ medie: 9.75     │           │ 63 75 00 ... 00 │   (50 bytes)      │
│   │ varsta: 21      │           │ 00 00 1C 42     │ ← medie (4 bytes) │
│   └─────────────────┘           │ 15 00 00 00     │ ← varsta (4 bytes)│
│                                 └─────────────────┘                    │
│                                                                         │
│   ATENȚIE: Ordinea byte-ilor depinde de endianness!                    │
│   Little-endian: LSB primul (Intel, AMD)                               │
│   Big-endian: MSB primul (rețea, SPARC, POWER)                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 4. Probleme de Portabilitate

Trei aspecte critice afectează portabilitatea fișierelor binare:

| Aspect | Problemă | Soluție |
|--------|----------|---------|
| **Endianness** | Ordinea byte-ilor diferă între arhitecturi | Convertire la network byte order cu `htonl()`/`ntohl()` |
| **Padding** | Compilatorul inserează bytes de aliniere | Utilizare `#pragma pack(1)` sau serializare câmp cu câmp |
| **Dimensiuni tipuri** | `int` poate fi 2, 4 sau 8 bytes | Utilizare tipuri cu dimensiune fixă: `int32_t`, `uint64_t` |

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    EXEMPLU PADDING STRUCTURĂ                            │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   struct Exemplu {          Layout în memorie (64-bit):                │
│       char a;               ┌───┬───┬───┬───┬───┬───┬───┬───┐          │
│       int b;                │ a │PAD│PAD│PAD│   b   │   b   │          │
│       char c;               │───┼───┼───┼───┼───┼───┼───┼───│          │
│   };                        │ c │PAD│PAD│PAD│PAD│PAD│PAD│PAD│          │
│                             └───┴───┴───┴───┴───┴───┴───┴───┘          │
│   sizeof(struct Exemplu) = 12 bytes (nu 6!)                            │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 🏭 Aplicații Industriale

### 1. Format BMP pentru Imagini

Formatul BMP (Bitmap) utilizează un header binar urmat de datele pixelilor:

```c
#pragma pack(push, 1)
typedef struct {
    uint16_t signature;      // 'BM' = 0x4D42
    uint32_t file_size;      // Dimensiunea totală a fișierului
    uint16_t reserved1;      // Rezervat (0)
    uint16_t reserved2;      // Rezervat (0)
    uint32_t pixel_offset;   // Offset către datele pixelilor
} BMPFileHeader;

typedef struct {
    uint32_t header_size;    // Dimensiunea acestui header (40)
    int32_t width;           // Lățimea imaginii în pixeli
    int32_t height;          // Înălțimea imaginii în pixeli
    uint16_t planes;         // Număr planuri de culoare (1)
    uint16_t bits_per_pixel; // Biți per pixel (24 pentru RGB)
    uint32_t compression;    // Tip compresie (0 = fără)
    uint32_t image_size;     // Dimensiunea datelor imagine
    // ... alte câmpuri
} BMPInfoHeader;
#pragma pack(pop)
```

### 2. Protocol de Rețea (Pachete Binare)

Protocoalele de rețea utilizează formate binare compacte pentru eficiență:

```c
#pragma pack(push, 1)
typedef struct {
    uint8_t version_ihl;     // Versiune (4 biți) + IHL (4 biți)
    uint8_t dscp_ecn;        // DSCP (6 biți) + ECN (2 biți)
    uint16_t total_length;   // Lungime totală (network byte order!)
    uint16_t identification; // ID pentru fragmentare
    uint16_t flags_fragment; // Flags + Fragment offset
    uint8_t ttl;             // Time To Live
    uint8_t protocol;        // Protocol încapsulat (TCP=6, UDP=17)
    uint16_t checksum;       // Sumă de control header
    uint32_t src_ip;         // Adresă IP sursă
    uint32_t dst_ip;         // Adresă IP destinație
} IPv4Header;
#pragma pack(pop)
```

### 3. Sistem de Gestiune Bază de Date Simplificată

```c
// Index pentru căutare rapidă
typedef struct {
    uint32_t key;            // Cheia de căutare
    uint32_t offset;         // Offset în fișierul de date
} IndexEntry;

// Funcție de căutare binară în fișier index
long find_record_offset(FILE *index_file, uint32_t search_key) {
    IndexEntry entry;
    long left = 0, right, mid;
    
    fseek(index_file, 0, SEEK_END);
    right = ftell(index_file) / sizeof(IndexEntry) - 1;
    
    while (left <= right) {
        mid = (left + right) / 2;
        fseek(index_file, mid * sizeof(IndexEntry), SEEK_SET);
        fread(&entry, sizeof(IndexEntry), 1, index_file);
        
        if (entry.key == search_key)
            return entry.offset;
        else if (entry.key < search_key)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;  // Nu s-a găsit
}
```

### 4. Serializare pentru Jocuri (Save Game)

```c
typedef struct {
    char player_name[32];
    uint32_t level;
    uint32_t experience;
    float health;
    float mana;
    uint32_t inventory_count;
    // Urmat de inventory_count structuri Item
} SaveGameHeader;

int save_game(const char *filename, const GameState *state) {
    FILE *f = fopen(filename, "wb");
    if (!f) return -1;
    
    // Magic number pentru verificare format
    uint32_t magic = 0x47414D45; // "GAME"
    fwrite(&magic, sizeof(magic), 1, f);
    
    // Versiunea formatului
    uint32_t version = 1;
    fwrite(&version, sizeof(version), 1, f);
    
    // Datele propriu-zise
    fwrite(&state->header, sizeof(SaveGameHeader), 1, f);
    fwrite(state->inventory, sizeof(Item), state->header.inventory_count, f);
    
    fclose(f);
    return 0;
}
```

### 5. WAV Audio File Format

```c
#pragma pack(push, 1)
typedef struct {
    char riff_id[4];         // "RIFF"
    uint32_t file_size;      // Dimensiune fișier - 8
    char wave_id[4];         // "WAVE"
    char fmt_id[4];          // "fmt "
    uint32_t fmt_size;       // 16 pentru PCM
    uint16_t audio_format;   // 1 pentru PCM
    uint16_t num_channels;   // 1=mono, 2=stereo
    uint32_t sample_rate;    // Ex: 44100
    uint32_t byte_rate;      // sample_rate * channels * bits/8
    uint16_t block_align;    // channels * bits/8
    uint16_t bits_per_sample;// 8, 16 sau 24
    char data_id[4];         // "data"
    uint32_t data_size;      // Dimensiune date audio
} WAVHeader;
#pragma pack(pop)
```

---

## 💻 Exerciții de Laborator

### Exercițiul 1: Gestiune Catalog Studenți (Binar)

**Cerințe:**
1. Definiți o structură `Student` cu câmpurile: id (int), nume (char[50]), grupa (char[10]), note[5] (float) și medie (float)
2. Implementați funcția `salveaza_studenti()` pentru scrierea unui array de studenți într-un fișier binar
3. Implementați funcția `incarca_studenti()` pentru citirea studenților din fișier
4. Implementați funcția `actualizeaza_student()` pentru modificarea unui student la o poziție specifică (fără rescrierea întregului fișier)
5. Implementați funcția `cauta_student_binar()` utilizând căutare binară după ID (presupunând fișierul sortat)
6. Adăugați verificare magic number și versiune la începutul fișierului
7. Tratați toate erorile potențiale (fișier inexistent, scriere eșuată etc.)
8. Afișați statistici despre operațiile efectuate

### Exercițiul 2: Convertor Imagine Grayscale

**Cerințe:**
1. Citiți un fișier BMP în format RGB (24 biți per pixel)
2. Parseați corect header-ele BMP și validați formatul
3. Convertiți fiecare pixel la grayscale folosind formula: `gray = 0.299*R + 0.587*G + 0.114*B`
4. Scrieți imaginea rezultată într-un nou fișier BMP (8 biți per pixel cu paletă grayscale)
5. Gestionați corect padding-ul rândurilor (aliniere la 4 bytes)
6. Afișați informații despre imagine: dimensiuni, dimensiune fișier, timp procesare

---

## 🔧 Compilare și Execuție

```bash
# Compilare toate exemplele
make all

# Compilare individuală cu debug symbols
make example1
make exercise1
make exercise2

# Rulare exemplu demonstrativ
make run-example

# Rulare exerciții
make run-ex1
make run-ex2

# Testare automată
make test

# Verificare memory leaks cu Valgrind
make valgrind

# Curățare fișiere generate
make clean

# Afișare help
make help
```

---

## 📁 Structura Directorului

```
week-03-fisiere-binare/
├── README.md                           # Acest fișier
├── Makefile                            # Automatizare compilare
│
├── slides/
│   ├── presentation-week03.html        # Prezentare principală (35-40 slide-uri)
│   └── presentation-comparativ.html    # Comparație Pseudocode/C/Python
│
├── src/
│   ├── example1.c                      # Exemplu complet demonstrativ
│   ├── exercise1.c                     # Exercițiu: Catalog studenți
│   └── exercise2.c                     # Exercițiu: Convertor BMP
│
├── data/
│   ├── studenti_initial.bin            # Date test pentru exercițiul 1
│   ├── test_image.bmp                  # Imagine test pentru exercițiul 2
│   └── sample_records.bin              # Înregistrări exemplu
│
├── tests/
│   ├── test1_input.txt                 # Input test exercițiu 1
│   ├── test1_expected.txt              # Output așteptat exercițiu 1
│   ├── test2_input.txt                 # Input test exercițiu 2
│   └── test2_expected.txt              # Output așteptat exercițiu 2
│
├── teme/
│   ├── homework-requirements.md        # Cerințe teme (2 × 50p)
│   └── homework-extended.md            # Provocări bonus (5 × 10p)
│
└── solution/
    ├── exercise1_sol.c                 # Soluție exercițiu 1
    ├── exercise2_sol.c                 # Soluție exercițiu 2
    ├── homework1_sol.c                 # Soluție temă 1
    └── homework2_sol.c                 # Soluție temă 2
```

---

## 📖 Lectură Recomandată

### Resurse Esențiale

1. **Kernighan, B. W. & Ritchie, D. M.** (1988). *The C Programming Language* (2nd ed.). Prentice Hall. **Capitolul 8: The UNIX System Interface**
2. **ISO/IEC 9899:2011** - Standardul C11, Secțiunea 7.21: Input/output <stdio.h>
3. **Stevens, W. R. & Rago, S. A.** (2013). *Advanced Programming in the UNIX Environment* (3rd ed.). Addison-Wesley. **Capitolele 3-5: File I/O**

### Resurse Avansate

4. **Bryant, R. E. & O'Hallaron, D. R.** (2015). *Computer Systems: A Programmer's Perspective* (3rd ed.). Pearson. **Capitolul 10: System-Level I/O**
5. **Drepper, U.** (2007). *What Every Programmer Should Know About Memory*. Red Hat, Inc.

### Resurse Online

- [GNU C Library Manual - I/O on Streams](https://www.gnu.org/software/libc/manual/html_node/I_002fO-on-Streams.html)
- [BMP File Format Specification](https://en.wikipedia.org/wiki/BMP_file_format)
- [Endianness Explained](https://betterexplained.com/articles/understanding-big-and-little-endian-byte-order/)

---

## ✅ Lista de Auto-Evaluare

La finalul acestei săptămâni, verificați că puteți:

- [ ] Explica diferențele conceptuale și practice dintre fișierele text și binare
- [ ] Utiliza corect funcțiile `fread()`, `fwrite()`, `fseek()` și `ftell()`
- [ ] Serializa și deserializa structuri de date complexe
- [ ] Gestiona problemele de endianness în contexte cross-platform
- [ ] Utiliza `#pragma pack` pentru a controla padding-ul structurilor
- [ ] Implementa operații CRUD pe fișiere binare cu acces direct
- [ ] Depana probleme de I/O binar utilizând `hexdump` sau utilitare similare
- [ ] Valida corectitudinea fișierelor binare utilizând magic numbers și checksums
- [ ] Optimiza performanța I/O prin buffering și citiri/scrieri în bloc
- [ ] Implementa un sistem simplu de indexare pentru acces rapid

---

## 💼 Pregătire pentru Interviuri

### Întrebări frecvente pe acest subiect:

1. **Care sunt avantajele și dezavantajele fișierelor binare față de cele text?**
   - *Indiciu: Discutați despre performanță, dimensiune, portabilitate și depanabilitate*

2. **Explicați diferența dintre endianness little-endian și big-endian. Cum afectează aceasta portabilitatea?**
   - *Indiciu: Dați exemplu concret cu un număr pe 4 bytes*

3. **Ce este padding-ul în structuri și de ce apare? Cum îl puteți controla?**
   - *Indiciu: Alinierea la granițe de memorie pentru performanță*

4. **Cum ați implementa un sistem de persistență pentru o structură de date complexă?**
   - *Indiciu: Magic number, versiune, serializare recursivă*

5. **Care este diferența dintre `fseek(f, 0, SEEK_END)` urmat de `ftell(f)` și utilizarea `fstat()`?**
   - *Indiciu: Portabilitate vs. funcționalitate sistem de operare*

---

## 🔗 Previzualizare Săptămâna Viitoare

**Săptămâna 04: Liste Înlănțuite**

În săptămâna următoare vom explora structurile de date dinamice, începând cu listele simplu și dublu înlănțuite. Veți învăța să:

- Alocați și eliberați memorie dinamic cu `malloc()` și `free()`
- Implementați operații de inserare, ștergere și căutare în liste
- Înțelegeți diferențele dintre array-uri și liste înlănțuite
- Combinați listele înlănțuite cu fișierele binare pentru persistența datelor

Cunoștințele despre fișiere binare din această săptămână vor fi esențiale pentru implementarea serializării listelor înlănțuite!

---

*Material elaborat pentru cursul „Algoritmi și Tehnici de Programare"*
*Academia de Studii Economice București - CSIE*
