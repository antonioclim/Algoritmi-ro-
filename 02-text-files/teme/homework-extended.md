# Provocări Extinse - Săptămâna 02

## 🚀 Provocări Avansate (Opțional)

Fiecare provocare rezolvată corect: **+10 puncte bonus**

---

## ⭐ Provocarea 1: Parser CSV Robust (Dificultate: Medie)

### Descriere

Implementați un parser CSV complet care respectă RFC 4180, cu suport pentru toate cazurile speciale.

### Cerințe

1. Suport pentru câmpuri încadrate în ghilimele duble
2. Gestionarea virgulelor în interiorul câmpurilor
3. Gestionarea caracterelor newline în câmpuri
4. Escaparea ghilimelelor prin dublare (`""`)
5. Detectarea automată a delimitatorului (virgulă, punct și virgulă, tab)

### Exemplu Input

```csv
"Nume, Prenume",Vârstă,Descriere
"Popescu, Ion",25,"A spus: ""Bună ziua""
și a plecat"
"Ionescu, Maria",30,"Text simplu"
```

### Interfață

```c
typedef struct {
    char ***data;      /* Array 2D de string-uri */
    int rows;
    int cols;
} CSVTable;

CSVTable *csv_parse(const char *filename);
void csv_free(CSVTable *table);
void csv_print(const CSVTable *table);
char *csv_get_cell(const CSVTable *table, int row, int col);
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 2: Sistem de Logging cu Rotație (Dificultate: Medie)

### Descriere

Implementați o bibliotecă de logging cu suport pentru niveluri, rotația fișierelor și format configurabil.

### Cerințe

1. Niveluri de logging: DEBUG, INFO, WARNING, ERROR, CRITICAL
2. Rotația fișierelor când se depășește o dimensiune maximă
3. Timestamp automat pentru fiecare mesaj
4. Format configurabil (JSON, text simplu, format personalizat)
5. Suport pentru logging în mai multe fișiere simultan

### Interfață

```c
typedef enum {
    LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_CRITICAL
} LogLevel;

typedef struct Logger Logger;

Logger *logger_create(const char *filename, LogLevel min_level);
void logger_set_max_size(Logger *log, long max_bytes);
void logger_set_format(Logger *log, const char *format);
void logger_log(Logger *log, LogLevel level, const char *fmt, ...);
void logger_destroy(Logger *log);

/* Macro-uri pentru utilizare ușoară */
#define LOG_DEBUG(logger, ...) logger_log(logger, LOG_DEBUG, __VA_ARGS__)
#define LOG_INFO(logger, ...) logger_log(logger, LOG_INFO, __VA_ARGS__)
#define LOG_ERROR(logger, ...) logger_log(logger, LOG_ERROR, __VA_ARGS__)
```

### Exemplu Utilizare

```c
Logger *log = logger_create("app.log", LOG_INFO);
logger_set_max_size(log, 1024 * 1024);  /* 1 MB */

LOG_INFO(log, "Aplicația a pornit");
LOG_ERROR(log, "Eroare la conectare: cod %d", err_code);

logger_destroy(log);
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 3: Diff pentru Fișiere Text (Dificultate: Ridicată)

### Descriere

Implementați un utilitar care compară două fișiere text și afișează diferențele, similar cu comanda Unix `diff`.

### Cerințe

1. Detectarea liniilor adăugate
2. Detectarea liniilor șterse
3. Detectarea liniilor modificate
4. Afișare în format unificat (unified diff)
5. Opțiune pentru ignorarea spațiilor albe

### Algoritm Sugerat

Longest Common Subsequence (LCS) pentru a găsi secvențele comune între cele două fișiere.

### Exemplu Output

```diff
--- file1.txt
+++ file2.txt
@@ -1,5 +1,5 @@
 Linia 1 - neschimbată
-Linia 2 - versiunea veche
+Linia 2 - versiunea nouă
 Linia 3 - neschimbată
-Linia 4 - ștearsă
 Linia 5 - neschimbată
+Linia 6 - adăugată
```

### Interfață

```c
typedef struct {
    int line_old;
    int line_new;
    char type;  /* '+' adăugat, '-' șters, ' ' neschimbat, '!' modificat */
    char *content;
} DiffLine;

typedef struct {
    DiffLine *lines;
    int count;
} DiffResult;

DiffResult *diff_files(const char *file1, const char *file2);
void diff_print_unified(const DiffResult *diff);
void diff_free(DiffResult *diff);
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 4: Template Engine (Dificultate: Ridicată)

### Descriere

Implementați un motor de template-uri simplu care poate genera text din șabloane cu variabile și structuri de control.

### Cerințe

1. Înlocuirea variabilelor: `{{variable}}`
2. Structuri condiționale: `{{if condition}}...{{endif}}`
3. Bucle: `{{for item in list}}...{{endfor}}`
4. Includerea altor fișiere: `{{include "file.txt"}}`
5. Filtre simple: `{{variable|upper}}`, `{{variable|lower}}`

### Exemplu Template

```html
Bună ziua, {{name}}!

{{if is_premium}}
Ești membru premium. Iată beneficiile tale:
{{for benefit in benefits}}
- {{benefit}}
{{endfor}}
{{endif}}

{{include "footer.txt"}}
```

### Interfață

```c
typedef struct {
    char *name;
    char *value;
} Variable;

typedef struct {
    Variable *vars;
    int count;
} Context;

Context *context_create(void);
void context_set(Context *ctx, const char *name, const char *value);
void context_set_list(Context *ctx, const char *name, char **values, int count);
char *template_render(const char *template_file, const Context *ctx);
void context_free(Context *ctx);
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 5: Bază de Date Text (Dificultate: Ridicată)

### Descriere

Implementați o bază de date simplă bazată pe fișiere text, cu suport pentru interogări de bază.

### Cerințe

1. Definirea schemei (tipuri de coloane)
2. Operații: INSERT, SELECT, UPDATE, DELETE
3. Condiții simple în WHERE (=, <, >, LIKE)
4. Indexare pentru căutare rapidă
5. Tranzacții atomice (rollback la eroare)

### Format Stocare

```
# students.db
@schema: id:int, name:string, grade:float
@index: id, name
1|Popescu Ion|9.5
2|Ionescu Maria|8.7
3|Georgescu Andrei|7.2
```

### Interfață

```c
typedef struct Database Database;
typedef struct QueryResult QueryResult;

Database *db_open(const char *filename);
void db_close(Database *db);

int db_insert(Database *db, const char *values);
QueryResult *db_select(Database *db, const char *columns, const char *where);
int db_update(Database *db, const char *set, const char *where);
int db_delete(Database *db, const char *where);

void result_print(const QueryResult *res);
void result_free(QueryResult *res);
```

### Exemplu Utilizare

```c
Database *db = db_open("students.db");

db_insert(db, "4, 'Dumitrescu Ana', 8.9");
QueryResult *res = db_select(db, "name, grade", "grade > 8.0");
result_print(res);

db_update(db, "grade = 9.0", "id = 3");
db_delete(db, "id = 1");

result_free(res);
db_close(db);
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
| Toate 5 | +50 puncte + Insigna "🏆 Text Files Master" |

---

## 📋 Criterii de Evaluare pentru Provocări

- **Funcționalitate completă:** 50%
- **Gestionarea erorilor:** 20%
- **Calitatea codului:** 15%
- **Documentație și comentarii:** 10%
- **Fără memory leaks:** 5%

---

## 💡 Recomandări

1. Alegeți provocarea care vă interesează cel mai mult
2. Începeți cu un prototip simplu și adăugați funcționalități treptat
3. Testați cu cazuri limită (fișiere goale, caractere speciale, etc.)
4. Documentați deciziile de design în comentarii
5. Cereți feedback înainte de versiunea finală

---

*Provocări create pentru cursul ATP - Săptămâna 02*
