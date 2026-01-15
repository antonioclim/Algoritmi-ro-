# Săptămâna 03 - Teme: Fișiere Binare

## 📋 Informații Generale

- **Termen limită:** Sfârșitul săptămânii 04
- **Punctaj:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu `-Wall -Wextra -std=c11`

---

## 📝 Tema 1: Sistem de Gestiune Produse (50 puncte)

### Descriere

Implementați un sistem de gestiune a inventarului de produse pentru un magazin, utilizând fișiere binare pentru persistența datelor. Sistemul trebuie să permită adăugarea, căutarea, actualizarea și ștergerea produselor.

### Cerințe Funcționale

1. **Structura Produs** (5p)
   ```c
   typedef struct {
       uint32_t cod;           // Cod unic produs (ex: 100001)
       char denumire[60];      // Denumirea produsului
       char categorie[30];     // Categoria (Electronice, Alimente, etc.)
       float pret;             // Prețul unitar
       uint32_t stoc;          // Cantitate în stoc
       uint8_t activ;          // 1=activ, 0=șters
   } Produs;
   ```

2. **Header Fișier** (5p)
   - Magic number pentru validare format
   - Versiunea formatului
   - Număr total de produse
   - Timestamp creare și ultima modificare

3. **Operații CRUD** (20p)
   - **Adăugare produs** (5p): Verificare cod unic, calcul automat poziție
   - **Căutare produs** (5p): După cod SAU după denumire (substring)
   - **Actualizare produs** (5p): Modificare preț/stoc fără rescriere fișier
   - **Ștergere produs** (5p): Soft delete (marcare ca inactiv)

4. **Rapoarte** (10p)
   - Lista produselor cu stoc sub o limită dată
   - Valoarea totală a inventarului (sumă preț × stoc)
   - Produse pe categorii

5. **Gestionare Erori** (5p)
   - Verificare toate operațiile I/O
   - Mesaje de eroare descriptive
   - Revenire gracefully în caz de eroare

6. **Interfață Utilizator** (5p)
   - Meniu text interactiv
   - Afișare formatată a produselor
   - Confirmare operații distructive

### Exemplu Utilizare

```
╔═══════════════════════════════════════════════════════════════╗
║                SISTEM GESTIUNE INVENTAR                       ║
╠═══════════════════════════════════════════════════════════════╣
║  1. Adaugă produs                                             ║
║  2. Caută produs (cod)                                        ║
║  3. Caută produs (denumire)                                   ║
║  4. Actualizează produs                                       ║
║  5. Șterge produs                                             ║
║  6. Listează toate produsele                                  ║
║  7. Raport stoc scăzut                                        ║
║  8. Valoare totală inventar                                   ║
║  0. Ieșire                                                    ║
╚═══════════════════════════════════════════════════════════════╝
```

### Fișier: `homework1_inventar.c`

---

## 📝 Tema 2: Editor de Imagini BMP (50 puncte)

### Descriere

Implementați un editor simplu de imagini BMP care poate efectua mai multe operații de procesare pe imagini în format BMP 24-bit.

### Cerințe Funcționale

1. **Citire/Scriere BMP** (10p)
   - Parsare corectă header-e BMP
   - Validare format (24-bit, necomprimat)
   - Gestionare padding rânduri

2. **Conversie Grayscale** (10p)
   - Formula: `Gray = 0.299*R + 0.587*G + 0.114*B`
   - Output în format BMP 8-bit cu paletă

3. **Oglindire Imagine** (10p)
   - Oglindire orizontală (flip horizontal)
   - Oglindire verticală (flip vertical)

4. **Rotire Imagine** (10p)
   - Rotire 90° în sensul acelor de ceasornic
   - Rotire 90° în sens invers

5. **Ajustare Luminozitate** (5p)
   - Parametru: -100 (întunecare) până la +100 (luminare)
   - Clipping la 0-255

6. **Statistici Imagine** (5p)
   - Histogramă per canal (R, G, B)
   - Luminozitate medie
   - Contrast (deviație standard)

### Exemplu Utilizare

```bash
# Conversie grayscale
./homework2_bmp grayscale input.bmp output_gray.bmp

# Oglindire orizontală
./homework2_bmp flip_h input.bmp output_flip.bmp

# Rotire 90 grade
./homework2_bmp rotate_cw input.bmp output_rot.bmp

# Ajustare luminozitate (+30)
./homework2_bmp brightness input.bmp output_bright.bmp 30

# Afișare statistici
./homework2_bmp stats input.bmp
```

### Fișier: `homework2_bmp.c`

---

## 📊 Criterii de Evaluare

| Criteriu | Punctaj |
|----------|---------|
| Corectitudine funcțională | 40p |
| Utilizare corectă concepte fișiere binare | 25p |
| Gestionare cazuri limită | 15p |
| Calitate cod (structură, comentarii) | 10p |
| Compilare fără warning-uri | 10p |

### Penalizări

| Problemă | Penalizare |
|----------|------------|
| Warning-uri la compilare | -10p |
| Memory leaks (Valgrind) | -20p |
| Crash pe input valid | -30p |
| Cod copiat / plagiat | -50p până la -100p |

---

## 📤 Instrucțiuni Trimitere

1. Creați un director `Nume_Prenume_Grupa_S03`
2. Includeți fișierele sursă și un `Makefile`
3. Includeți un fișier `README.md` cu:
   - Instrucțiuni de compilare și rulare
   - Descriere scurtă a implementării
   - Probleme cunoscute (dacă există)
4. Arhivați ca `.zip` și încărcați pe platforma de curs

### Structură Arhivă

```
Popescu_Ion_1234A_S03/
├── Makefile
├── README.md
├── homework1_inventar.c
├── homework2_bmp.c
└── test_images/
    └── test.bmp
```

---

## 💡 Sfaturi

1. **Începeți devreme** - temele necesită timp pentru depanare
2. **Testați incremental** - verificați fiecare funcție separat
3. **Folosiți Valgrind** pentru detectarea memory leaks
4. **Citiți cu atenție** specificația format BMP
5. **Salvați backup** - nu vreți să pierdeți munca
6. **Utilizați hexdump** pentru a verifica conținutul fișierelor binare

---

## ❓ Întrebări Frecvente

**Q: Pot folosi alte biblioteci în afară de standard C?**
A: Nu. Utilizați doar biblioteca standard C (`stdio.h`, `stdlib.h`, `string.h`, `stdint.h`, `math.h`).

**Q: Trebuie să gestionez imagini BMP comprimate?**
A: Nu. Puteți presupune că imaginile sunt necomprimate (compression = 0).

**Q: Ce se întâmplă dacă fișierul BMP nu este valid?**
A: Programul trebuie să afișeze un mesaj de eroare descriptiv și să iasă cu cod non-zero.

---

*Material pentru cursul „Algoritmi și Tehnici de Programare" - ASE București*
