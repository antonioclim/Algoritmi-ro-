# Provocări Extinse - Săptămâna 04

## 🚀 Provocări Avansate (Opționale)

Fiecare provocare rezolvată corect: **+10 puncte bonus**

Punctele bonus se adaugă la nota temei (maxim 150 puncte total posibil).

---

## ⭐ Provocarea 1: Detectarea și Eliminarea Ciclurilor

**Dificultate:** Medie 🟡

### Descriere

Implementați o funcție care detectează dacă o listă înlănțuită conține un ciclu și, dacă da, elimină ciclul păstrând lista intactă.

### Cerințe

```c
/**
 * Detectează și elimină ciclul dintr-o listă înlănțuită.
 * 
 * @param head Pointer către capul listei
 * @return 1 dacă a existat și a fost eliminat un ciclu, 0 altfel
 *
 * Algoritm sugerat:
 *   1. Folosiți algoritmul Floyd (tortoise and hare) pentru detectare
 *   2. Dacă există ciclu, găsiți punctul de început al ciclului
 *   3. Găsiți ultimul nod din ciclu și setați-i next la NULL
 */
int detect_and_remove_cycle(Node **head);
```

### Exemple

```
Input:  1 -> 2 -> 3 -> 4 -> 5 -> 3 (ciclu la nodul 3)
Output: 1 -> 2 -> 3 -> 4 -> 5 -> NULL

Input:  1 -> 2 -> 3 -> 4 -> NULL (fără ciclu)
Output: 1 -> 2 -> 3 -> 4 -> NULL (nemodificată)
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 2: Merge K Liste Sortate

**Dificultate:** Medie-Grea 🟠

### Descriere

Implementați o funcție care unește K liste sortate într-o singură listă sortată.

### Cerințe

```c
/**
 * Unește K liste sortate într-o singură listă sortată.
 *
 * @param lists Array de pointeri către capetele celor K liste
 * @param k Numărul de liste
 * @return Pointer către capul listei unificate
 *
 * Constrângeri:
 *   - Toate listele de intrare sunt sortate crescător
 *   - Funcția NU trebuie să aloce noduri noi (reutilizează nodurile existente)
 *   - Complexitate dorită: O(N log K) unde N = numărul total de elemente
 */
Node *merge_k_lists(Node **lists, int k);
```

### Exemple

```
Input: 
  Lista 0: 1 -> 4 -> 7
  Lista 1: 2 -> 5 -> 8  
  Lista 2: 3 -> 6 -> 9

Output: 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> 9
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 3: LRU Cache cu Liste Dublu Înlănțuite

**Dificultate:** Grea 🔴

### Descriere

Implementați un cache LRU (Least Recently Used) folosind o listă dublu înlănțuită combinată cu un hash map simplu.

### Cerințe

```c
typedef struct CacheNode {
    int key;
    int value;
    struct CacheNode *prev;
    struct CacheNode *next;
} CacheNode;

typedef struct {
    int capacity;
    int size;
    CacheNode *head;      /* Most recently used */
    CacheNode *tail;      /* Least recently used */
    CacheNode **hash_map; /* Array pentru acces rapid */
    int hash_size;
} LRUCache;

/**
 * Creează un cache LRU cu capacitatea specificată.
 */
LRUCache *lru_create(int capacity);

/**
 * Obține valoarea asociată cheii sau -1 dacă nu există.
 * Mută elementul accesat la început (most recently used).
 */
int lru_get(LRUCache *cache, int key);

/**
 * Inserează sau actualizează o pereche cheie-valoare.
 * Dacă cache-ul e plin, elimină elementul LRU (tail).
 */
void lru_put(LRUCache *cache, int key, int value);

/**
 * Eliberează toate resursele cache-ului.
 */
void lru_free(LRUCache *cache);
```

### Exemplu de Utilizare

```c
LRUCache *cache = lru_create(3);  // Capacitate: 3 elemente

lru_put(cache, 1, 100);  // Cache: [(1,100)]
lru_put(cache, 2, 200);  // Cache: [(2,200), (1,100)]
lru_put(cache, 3, 300);  // Cache: [(3,300), (2,200), (1,100)]

lru_get(cache, 1);       // Returnează 100, Cache: [(1,100), (3,300), (2,200)]

lru_put(cache, 4, 400);  // Cache plin! Elimină (2,200)
                         // Cache: [(4,400), (1,100), (3,300)]

lru_get(cache, 2);       // Returnează -1 (a fost eliminat)

lru_free(cache);
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 4: Clonarea unei Liste cu Pointeri Aleatorii

**Dificultate:** Medie 🟡

### Descriere

Implementați clonarea profundă (deep copy) a unei liste înlănțuite unde fiecare nod are, pe lângă pointer-ul `next`, și un pointer `random` care poate pointa către orice alt nod din listă sau NULL.

### Cerințe

```c
typedef struct RandomNode {
    int data;
    struct RandomNode *next;
    struct RandomNode *random;  /* Pointer aleatoriu către orice nod */
} RandomNode;

/**
 * Creează o copie profundă a listei.
 * Pointer-ii random din copia nouă trebuie să pointeze
 * către nodurile corespunzătoare din lista nouă (nu cea veche!).
 *
 * @param head Pointer către capul listei originale
 * @return Pointer către capul noii liste (copia)
 *
 * Constrângere: Complexitate O(n) timp, O(1) spațiu auxiliar
 * (fără hash map sau array auxiliar)
 *
 * Hint: Tehnica "interleaving" - inserați nodurile noi între cele vechi
 */
RandomNode *clone_list_with_random(RandomNode *head);
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 5: Flatten Multilevel Doubly Linked List

**Dificultate:** Grea 🔴

### Descriere

Aveți o listă dublu înlănțuită pe mai multe niveluri: unele noduri pot avea un pointer `child` către o altă listă dublu înlănțuită. Implementați o funcție care "aplatizează" structura într-o singură listă.

### Cerințe

```c
typedef struct MultiNode {
    int data;
    struct MultiNode *prev;
    struct MultiNode *next;
    struct MultiNode *child;  /* Pointer către o sub-listă */
} MultiNode;

/**
 * Aplatizează lista multi-nivel într-o singură listă.
 * Sub-listele se inserează imediat după nodul părinte.
 *
 * @param head Pointer către capul listei
 * @return Pointer către capul listei aplatizate
 *
 * Exemplu:
 *   1 <-> 2 <-> 3 <-> 4
 *         |
 *         5 <-> 6
 *               |
 *               7
 *
 *   Devine: 1 <-> 2 <-> 5 <-> 6 <-> 7 <-> 3 <-> 4
 */
MultiNode *flatten_list(MultiNode *head);
```

### Puncte Bonus: +10

---

## 📊 Sistemul de Puncte Bonus

| Provocări Completate | Total Bonus | Badge |
|---------------------|-------------|-------|
| 1 | +10 puncte | 🥉 Starter |
| 2 | +20 puncte | 🥈 Intermediar |
| 3 | +30 puncte | 🥇 Avansat |
| 4 | +40 puncte | 💎 Expert |
| Toate 5 | +50 puncte | 🏆 **Linked List Master** |

---

## 📤 Instrucțiuni de Predare

1. Creați fișierele separate pentru fiecare provocare:
   - `bonus1_cycle.c`
   - `bonus2_merge_k.c`
   - `bonus3_lru.c`
   - `bonus4_clone.c`
   - `bonus5_flatten.c`

2. Includeți în fiecare fișier:
   - Funcția `main()` cu teste demonstrative
   - Comentarii explicative pentru algoritm
   - Verificare cu Valgrind

3. Adăugați la arhiva temei în subdirectorul `bonus/`

---

## 💡 Sfaturi pentru Provocări

1. **Provocarea 1 (Cicluri):**
   - Algoritmul Floyd: slow avansează cu 1, fast cu 2
   - După detectare, resetați unul la head și avansați ambii cu 1

2. **Provocarea 2 (Merge K):**
   - Varianta simplă: merge 2 câte 2 (dar O(NK))
   - Varianta optimă: min-heap pentru a selecta minimul în O(log K)

3. **Provocarea 3 (LRU):**
   - Lista dublu înlănțuită = ordinea de acces
   - Hash map = acces O(1) după cheie
   - La fiecare acces, mută nodul la început

4. **Provocarea 4 (Clone Random):**
   - Pasul 1: Inserați clone între originale (1->1'->2->2'->...)
   - Pasul 2: Setați pointer-ii random pentru clone
   - Pasul 3: Separați cele două liste

5. **Provocarea 5 (Flatten):**
   - Recursiv: aplatizați child-ul, apoi continuați cu next
   - Iterativ: folosiți o stivă pentru a salva next-ul

---

**Mult succes!** 🚀

*Provocările bonus sunt opționale dar vă ajută să stăpâniți cu adevărat structurile de date!*
