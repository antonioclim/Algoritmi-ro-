# Provocări Extinse - Săptămâna 1

## 🚀 Provocări Avansate (Opționale)

Aceste exerciții sunt pentru studenții care doresc să aprofundeze conceptele și să obțină puncte bonus. Fiecare provocare rezolvată corect aduce **+10 puncte bonus** la nota finală.

---

## ⭐ Provocare 1: Sistem de Plugin-uri (Dificultate: Medie)

### Descriere

Implementați un sistem simplu de plugin-uri care permite înregistrarea dinamică de operații matematice.

### Cerințe

1. **Structura Plugin**:
   ```c
   typedef struct {
       char nume[32];
       char simbol;
       int (*operatie)(int, int);
       char descriere[100];
   } Plugin;
   ```

2. **Manager de Plugin-uri**:
   ```c
   typedef struct {
       Plugin pluginuri[20];
       int contor;
   } ManagerPluginuri;

   void manager_initializeaza(ManagerPluginuri *pm);
   int plugin_inregistreaza(ManagerPluginuri *pm, const Plugin *plugin);
   int plugin_dezinregistreaza(ManagerPluginuri *pm, char simbol);
   Plugin* plugin_gaseste(ManagerPluginuri *pm, char simbol);
   void plugin_listeaza(const ManagerPluginuri *pm);
   ```

3. **Demonstrație**:
   - Înregistrați 5+ operații
   - Afișați lista de plugin-uri disponibile
   - Executați operații folosind plugin-urile
   - Dezînregistrați un plugin și verificați că nu mai este disponibil

### Puncte Bonus: +10

---

## ⭐ Provocare 2: Sortare Multi-Criteriu (Dificultate: Medie-Ridicată)

### Descriere

Implementați un sistem de sortare care poate combina multiple criterii de sortare.

### Cerințe

1. **Structură pentru criteriu de sortare**:
   ```c
   typedef struct {
       int (*comparator)(const void*, const void*);
       int crescator;  // 1 = crescător, 0 = descrescător
   } CriteriuSortare;
   ```

2. **Sortare cu multiple criterii**:
   ```c
   // Sortează mai întâi după primul criteriu, apoi după al doilea pentru elemente egale, etc.
   void sortare_multipla(void *base, size_t nmemb, size_t size,
                         CriteriuSortare *criterii, int nr_criterii);
   ```

3. **Exemplu de utilizare**:
   - Sortați studenți mai întâi după an de studiu, apoi după medie (descrescător), apoi după nume

### Puncte Bonus: +10

---

## ⭐ Provocare 3: Mașină de Stări Finită (Dificultate: Ridicată)

### Descriere

Implementați un FSM (Finite State Machine) generic folosind tabele de dispatch.

### Cerințe

1. **Structuri de bază**:
   ```c
   typedef enum { STARE_A, STARE_B, STARE_C, NR_STARI } Stare;
   typedef enum { EVENIMENT_X, EVENIMENT_Y, EVENIMENT_Z, NR_EVENIMENTE } Eveniment;

   typedef Stare (*FunctieTranszitie)(void *context);

   typedef struct {
       FunctieTranszitie tranzitii[NR_STARI][NR_EVENIMENTE];
       void (*la_intrare[NR_STARI])(void *context);
       void (*la_iesire[NR_STARI])(void *context);
   } MasinaStari;
   ```

2. **Funcții FSM**:
   ```c
   void fsm_initializeaza(MasinaStari *fsm);
   void fsm_seteaza_tranzitie(MasinaStari *fsm, Stare din, Eveniment ev, FunctieTranszitie func);
   void fsm_seteaza_la_intrare(MasinaStari *fsm, Stare stare, void (*callback)(void*));
   void fsm_seteaza_la_iesire(MasinaStari *fsm, Stare stare, void (*callback)(void*));
   Stare fsm_proceseaza_eveniment(MasinaStari *fsm, Stare curenta, Eveniment ev, void *context);
   ```

3. **Aplicație practică**: Implementați un semafor de trafic sau un automat de vânzare.

### Puncte Bonus: +10

---

## ⭐ Provocare 4: Map/Filter/Reduce Generic (Dificultate: Medie)

### Descriere

Implementați funcțiile de ordin superior map, filter și reduce în C.

### Cerințe

1. **Map**:
   ```c
   void array_map(void *dest, const void *sursa, size_t n, size_t dim_elem,
                  void (*transforma)(void *elem_dest, const void *elem_sursa));
   ```

2. **Filter**:
   ```c
   size_t array_filter(void *dest, const void *sursa, size_t n, size_t dim_elem,
                       int (*predicat)(const void *elem));
   ```

3. **Reduce**:
   ```c
   void array_reduce(void *rezultat, const void *arr, size_t n, size_t dim_elem,
                     void (*reducator)(void *acumulator, const void *elem),
                     const void *initial);
   ```

4. **Demonstrație cu int și struct**:
   - Map: dublează fiecare element
   - Filter: păstrează doar elementele pozitive
   - Reduce: calculează suma/produsul

### Puncte Bonus: +10

---

## ⭐ Provocare 5: Sistem de Evenimente (Dificultate: Ridicată)

### Descriere

Implementați un sistem de evenimente similar cu cel din framework-urile GUI.

### Cerințe

1. **Structuri**:
   ```c
   typedef void (*HandlerEveniment)(void *emitator, void *date_eveniment, void *date_utilizator);

   typedef struct {
       char nume_eveniment[32];
       HandlerEveniment handler;
       void *date_utilizator;
   } Abonament;

   typedef struct {
       Abonament abonamente[100];
       int contor;
   } BusEvenimente;
   ```

2. **Funcții**:
   ```c
   void bus_initializeaza(BusEvenimente *bus);
   int eveniment_aboneaza(BusEvenimente *bus, const char *nume_eveniment,
                          HandlerEveniment handler, void *date_utilizator);
   int eveniment_dezaboneaza(BusEvenimente *bus, const char *nume_eveniment,
                             HandlerEveniment handler);
   void eveniment_emite(BusEvenimente *bus, const char *nume_eveniment,
                        void *emitator, void *date_eveniment);
   ```

3. **Demonstrație**:
   - Creați evenimente "click_buton", "apasare_tasta", "tic_timer"
   - Înregistrați multiple handler-e pentru fiecare eveniment
   - Emiteți evenimente și verificați că toate handler-ele sunt apelate

### Puncte Bonus: +10

---

## 📊 Sistem de Punctare Bonus

| Provocări Completate | Bonus Total |
|---------------------|-------------|
| 1 provocare | +10 puncte |
| 2 provocări | +20 puncte |
| 3 provocări | +30 puncte |
| 4 provocări | +40 puncte |
| Toate 5 | +50 puncte + Insignă "Master Callbacks" 🏆 |

---

## 📤 Predare

1. Fișierele să fie numite `bonus1_pluginuri.c`, `bonus2_sortare_multipla.c`, etc.
2. Fiecare fișier trebuie să compileze independent
3. Includeți comentarii explicative
4. Testați cu Valgrind pentru memory leaks

---

## 💡 Sfaturi

1. **Începeți cu provocarea care vi se pare cea mai interesantă**
2. **Folosiți typedef extensiv** pentru claritate
3. **Testați incremental** - nu scrieți tot codul și apoi testați
4. **Desenați diagrame** pentru FSM și sistemul de evenimente
5. **Citiți documentația** - man pages pentru funcții standard

---

*Provocările sunt opționale dar recompensate. Succes! 🎯*
