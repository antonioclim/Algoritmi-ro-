# Tema Săptămâna 02: Fișiere Text

## 📋 Informații Generale

- **Termen limită:** Sfârșitul săptămânii 03
- **Punctaj:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu `-Wall -Wextra -std=c11`

---

## 📝 Tema 1: Procesator de Log-uri (50 puncte)

### Descriere

Implementați un program care procesează fișiere de log în format Apache/Nginx și generează statistici detaliate.

### Format Input (access.log)

```
192.168.1.100 - - [15/Jan/2024:10:30:45 +0200] "GET /index.html HTTP/1.1" 200 2326
192.168.1.101 - - [15/Jan/2024:10:30:46 +0200] "POST /api/login HTTP/1.1" 401 156
10.0.0.50 - - [15/Jan/2024:10:30:47 +0200] "GET /images/logo.png HTTP/1.1" 200 45678
```

### Cerințe

1. **(10p)** Citirea și parsarea fișierului de log
   - Extragerea adresei IP
   - Extragerea timestamp-ului
   - Extragerea metodei HTTP (GET, POST, PUT, DELETE)
   - Extragerea URL-ului
   - Extragerea codului de status
   - Extragerea dimensiunii răspunsului

2. **(10p)** Calcularea statisticilor
   - Total request-uri
   - Request-uri per metodă HTTP
   - Distribuția codurilor de status (2xx, 3xx, 4xx, 5xx)
   - Top 10 IP-uri după numărul de request-uri
   - Top 10 URL-uri cele mai accesate

3. **(10p)** Generarea raportului
   - Raport text formatat în fișier
   - Sumar pe oră (câte request-uri per oră)
   - Tabel cu statistici

4. **(10p)** Filtrare avansată
   - Filtrare după interval de timp
   - Filtrare după cod de status
   - Filtrare după IP sau subnet

5. **(10p)** Calitatea codului
   - Fără warning-uri la compilare
   - Comentarii explicative
   - Gestionare corectă a erorilor
   - Fără memory leaks (verificat cu Valgrind)

### Exemplu Utilizare

```bash
./log_analyzer access.log report.txt
./log_analyzer access.log report.txt --start "15/Jan/2024:10:00" --end "15/Jan/2024:12:00"
./log_analyzer access.log report.txt --status 4xx
```

### Fișier: `homework1_log_analyzer.c`

---

## 📝 Tema 2: Manager de Contacte (50 puncte)

### Descriere

Implementați o aplicație de gestionare a contactelor cu persistență în fișiere CSV, suport pentru căutare și export în multiple formate.

### Format Stocare (contacts.csv)

```csv
ID,Nume,Prenume,Telefon,Email,Categorie,DataAdaugare
1,Popescu,Ion,0721123456,ion.popescu@email.ro,Familie,2024-01-15
2,Ionescu,Maria,0731234567,maria.ionescu@email.ro,Prieteni,2024-01-14
```

### Cerințe

1. **(10p)** Operații CRUD
   - Adăugare contact nou
   - Citire/afișare contacte
   - Actualizare contact existent
   - Ștergere contact

2. **(10p)** Căutare și filtrare
   - Căutare după nume (parțial)
   - Căutare după număr de telefon
   - Filtrare după categorie
   - Căutare case-insensitive

3. **(10p)** Import/Export
   - Import din CSV
   - Export în CSV
   - Export în format vCard (.vcf)
   - Export în format text formatat

4. **(10p)** Validare date
   - Validare format telefon (0721234567)
   - Validare format email (regex simplificat)
   - Verificare duplicat la adăugare
   - ID unic generat automat

5. **(10p)** Interfață utilizator
   - Meniu interactiv în consolă
   - Mesaje de confirmare/eroare clare
   - Sortare după diferite câmpuri
   - Paginare pentru liste mari

### Exemplu Meniu

```
╔═══════════════════════════════════════╗
║     MANAGER DE CONTACTE               ║
╠═══════════════════════════════════════╣
║  1. Afișare toate contactele          ║
║  2. Adaugă contact nou                ║
║  3. Caută contact                     ║
║  4. Editează contact                  ║
║  5. Șterge contact                    ║
║  6. Export contacte                   ║
║  7. Import contacte                   ║
║  0. Ieșire                            ║
╚═══════════════════════════════════════╝
Alegere: _
```

### Fișier: `homework2_contacts.c`

---

## 📊 Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Funcționalitate corectă | 40 |
| Utilizare corectă a funcțiilor I/O | 25 |
| Gestionarea cazurilor limită | 15 |
| Calitatea codului | 10 |
| Fără warning-uri la compilare | 10 |

### Penalizări

- **-10p:** Warning-uri la compilare
- **-20p:** Memory leaks (detectate cu Valgrind)
- **-30p:** Crash pe input valid
- **-50p:** Plagiat (copiere de la colegi sau internet)

---

## 📤 Modalitate de Predare

1. Creați un director cu numele `Nume_Prenume_Grupa_Tema02`
2. Includeți fișierele:
   - `homework1_log_analyzer.c`
   - `homework2_contacts.c`
   - `README.txt` (instrucțiuni de compilare și utilizare)
   - Fișiere de test (opțional)
3. Arhivați directorul în format `.zip`
4. Încărcați pe platforma de cursuri

---

## 💡 Sfaturi și Recomandări

1. **Începeți devreme** - Nu lăsați tema pe ultima zi
2. **Testați incremental** - Verificați fiecare funcție separat
3. **Folosiți Valgrind** - Verificați memory leaks după fiecare modificare
4. **Citiți specificațiile** - Asigurați-vă că înțelegeți cerințele
5. **Comentați codul** - Ajută la debugging și evaluare
6. **Gestionați erorile** - Nu presupuneți că totul funcționează

---

## ❓ Întrebări Frecvente

**Q: Pot folosi biblioteci externe?**
A: Nu, doar biblioteca standard C (`stdio.h`, `stdlib.h`, `string.h`, etc.)

**Q: Ce se întâmplă dacă fișierul de intrare nu există?**
A: Programul trebuie să afișeze un mesaj de eroare clar și să se oprească.

**Q: Pot colabora cu colegii?**
A: Puteți discuta ideile generale, dar codul trebuie să fie propriu. Copierea este plagiat.

**Q: Cum verific memory leaks?**
A: Rulați: `valgrind --leak-check=full ./program`

---

*Tema creată pentru cursul ATP - Săptămâna 02*
