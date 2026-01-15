# Provocări Extinse - Săptămâna 03

## 🚀 Provocări Avansate (Opționale)

Fiecare provocare rezolvată corect: **+10 puncte bonus**

---

## ⭐ Provocarea 1: Index B-Tree Simplificat (Dificultate: Medie)

### Descriere

Implementați un sistem de indexare bazat pe o structură arborescentă simplificată pentru căutare rapidă în fișierul de produse din Tema 1.

### Cerințe

1. Creați un fișier index separat (`inventar.idx`)
2. Structura indexului:
   ```c
   typedef struct {
       uint32_t cod;           // Cheia de căutare
       uint32_t offset;        // Offset în fișierul de date
   } IndexEntry;
   ```
3. Mențineți indexul sortat după cod
4. Implementați căutare binară în index
5. Actualizați indexul la fiecare inserare/ștergere

### Puncte Bonus: +10

---

## ⭐ Provocarea 2: Compactare Fișier (Dificultate: Medie)

### Descriere

Implementați o funcție de compactare care elimină fizic înregistrările marcate ca șterse.

### Cerințe

1. Funcție `compact_file(const char *filename)`
2. Creați un fișier temporar cu doar înregistrările active
3. Reînlocuiți fișierul original cu cel compactat
4. Actualizați header-ul (record_count)
5. Gestionați erorile (revenire la original dacă eșuează)

### Algoritm Sugerat

```
ALGORITM CompactareFișier(fișier)
    1. Deschide fișier original
    2. Creează fișier temporar
    3. Copiază header-ul (cu count=0)
    4. PENTRU FIECARE înregistrare:
       DACĂ este activă:
           Scrie în fișierul temporar
           Incrementează count
    5. Actualizează header-ul temporar
    6. Închide ambele fișiere
    7. Șterge original, redenumește temporar
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 3: Filtru de Margini Sobel (Dificultate: Ridicată)

### Descriere

Implementați detectarea marginilor în imagini BMP folosind operatorul Sobel.

### Cerințe

1. Convertiți imaginea la grayscale (dacă nu este deja)
2. Aplicați kernel-urile Sobel:
   ```
   Gx = [-1  0  +1]    Gy = [-1 -2 -1]
        [-2  0  +2]         [ 0  0  0]
        [-1  0  +1]         [+1 +2 +1]
   ```
3. Calculați magnitudinea: `G = sqrt(Gx² + Gy²)`
4. Normalizați rezultatul la 0-255
5. Salvați imaginea cu margini detectate

### Formula per Pixel

```c
int gx = -p[y-1][x-1] + p[y-1][x+1]
        -2*p[y][x-1]  + 2*p[y][x+1]
        -p[y+1][x-1]  + p[y+1][x+1];

int gy = -p[y-1][x-1] - 2*p[y-1][x] - p[y-1][x+1]
        +p[y+1][x-1]  + 2*p[y+1][x] + p[y+1][x+1];

int g = (int)sqrt(gx*gx + gy*gy);
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 4: Format Binar Propriu cu Compresie RLE (Dificultate: Ridicată)

### Descriere

Implementați un format de imagine propriu cu compresie Run-Length Encoding.

### Cerințe

1. Definiți un format de fișier propriu:
   ```c
   // Header
   typedef struct {
       char magic[4];          // "RIMG"
       uint32_t version;       // 1
       uint32_t width;
       uint32_t height;
       uint8_t bits_per_pixel; // 8 sau 24
       uint8_t compression;    // 0=none, 1=RLE
       uint32_t data_size;     // Dimensiune date comprimate
   } RIMGHeader;
   ```
2. Implementați compresie RLE:
   - Pentru secvențe de pixeli identici: `[count][pixel]`
   - Count maxim: 127 (folosiți bitul 7 pentru escapare)
3. Funcții: `save_rimg()`, `load_rimg()`
4. Conversie BMP ↔ RIMG

### Exemplu RLE

```
Input:  AAAAAABBBBCCCCCCCCCCDDEE
Output: 6A 4B 10C 2D 2E
        (6×A, 4×B, 10×C, 2×D, 2×E)
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 5: Portabilitate Cross-Platform (Dificultate: Medie)

### Descriere

Faceți sistemul de gestiune produse complet portabil între sisteme little-endian și big-endian.

### Cerințe

1. Toate valorile multi-byte salvate în network byte order (big-endian)
2. Funcții de conversie pentru fiecare tip:
   ```c
   uint32_t to_file_u32(uint32_t host_val);
   uint32_t from_file_u32(uint32_t file_val);
   float to_file_float(float host_val);
   float from_file_float(float file_val);
   ```
3. Header-ul include un marker de endianness pentru detectare
4. Testați pe ambele tipuri de sisteme (sau simulați)

### Structură Header Extinsă

```c
typedef struct {
    uint32_t magic;           // 0x50524F44 ("PROD")
    uint8_t  endianness;      // 0x01 = LE, 0x02 = BE (al sistemului creator)
    uint8_t  version;
    uint16_t reserved;
    uint32_t record_count;
    // ... etc
} PortableHeader;
```

### Puncte Bonus: +10

---

## 📊 Sistem de Puncte Bonus

| Provocări Completate | Total Bonus |
|---------------------|-------------|
| 1 | +10 puncte |
| 2 | +20 puncte |
| 3 | +30 puncte |
| 4 | +40 puncte |
| Toate 5 | +50 puncte + Badge 🏆 "Binary Master" |

---

## 📤 Trimitere Provocări

Includeți provocările rezolvate în aceeași arhivă cu temele:

```
Popescu_Ion_1234A_S03/
├── homework1_inventar.c
├── homework2_bmp.c
├── bonus/
│   ├── challenge1_btree.c
│   ├── challenge2_compact.c
│   ├── challenge3_sobel.c
│   ├── challenge4_rle.c
│   └── challenge5_portable.c
└── README.md
```

---

## 💡 Sfaturi pentru Provocări

1. **Începeți cu provocările ușoare** (1, 2, 5)
2. **Testați exhaustiv** - provocările au mai multe cazuri limită
3. **Documentați codul** - explicați algoritmul în comentarii
4. **Folosiți Git** pentru a urmări progresul

---

*Material pentru cursul „Algoritmi și Tehnici de Programare" - ASE București*
