/**
 * =============================================================================
 * SĂPTĂMÂNA 03: FIȘIERE BINARE
 * Exemplu Complet Demonstrativ
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Diferențele dintre fișiere text și binare
 *   2. Utilizarea funcțiilor fread() și fwrite()
 *   3. Navigare în fișiere cu fseek() și ftell()
 *   4. Serializarea și deserializarea structurilor
 *   5. Gestionarea problemelor de portabilitate (endianness, padding)
 *   6. Implementarea unui sistem CRUD simplu
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o example1 example1.c -lm
 * Utilizare: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

/* =============================================================================
 * CONSTANTE ȘI DEFINIȚII
 * =============================================================================
 */

#define MAX_NUME        50
#define MAX_STUDENTI    100
#define MAGIC_NUMBER    0x53545544  /* "STUD" în ASCII */
#define FILE_VERSION    1

/* Culori ANSI pentru output */
#define COLOR_RESET     "\033[0m"
#define COLOR_GREEN     "\033[0;32m"
#define COLOR_YELLOW    "\033[0;33m"
#define COLOR_CYAN      "\033[0;36m"
#define COLOR_RED       "\033[0;31m"
#define COLOR_BOLD      "\033[1m"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Header-ul fișierului binar
 * Conține informații meta despre format și conținut
 */
typedef struct {
    uint32_t magic;          /* Magic number pentru validare format */
    uint32_t version;        /* Versiunea formatului */
    uint32_t record_count;   /* Numărul de înregistrări */
    uint32_t record_size;    /* Dimensiunea unei înregistrări */
    uint64_t created_at;     /* Timestamp creare (Unix time) */
    uint64_t modified_at;    /* Timestamp ultima modificare */
    uint8_t  reserved[32];   /* Rezervat pentru extensii viitoare */
} FileHeader;

/**
 * Structura Student - folosită pentru demonstrație
 * Notă: Utilizăm #pragma pack pentru control explicit al layout-ului
 */
#pragma pack(push, 1)
typedef struct {
    uint32_t id;             /* ID unic student */
    char     nume[MAX_NUME]; /* Nume complet */
    char     grupa[10];      /* Grupa (ex: "1234A") */
    float    note[5];        /* Note la 5 examene */
    float    medie;          /* Media calculată */
    uint8_t  activ;          /* Flag: 1=activ, 0=șters */
} Student;
#pragma pack(pop)

/**
 * Context pentru operații cu fișierul binar
 */
typedef struct {
    FILE *file;
    char filename[256];
    FileHeader header;
    int is_open;
} DatabaseContext;

/* =============================================================================
 * PROTOTIPURI FUNCȚII
 * =============================================================================
 */

/* Funcții utilitare */
void print_separator(const char *title);
void print_bytes_hex(const void *data, size_t size, const char *label);
void print_student(const Student *s);
float calculeaza_medie(const float note[], int count);

/* Demonstrații PARTEA 1: Diferențe text vs. binar */
void demo_text_vs_binar(void);

/* Demonstrații PARTEA 2: fread și fwrite */
void demo_fread_fwrite(void);

/* Demonstrații PARTEA 3: fseek și ftell */
void demo_fseek_ftell(void);

/* Demonstrații PARTEA 4: Serializare structuri */
void demo_serializare(void);

/* Demonstrații PARTEA 5: Endianness și portabilitate */
void demo_endianness(void);

/* Demonstrații PARTEA 6: Sistem CRUD complet */
void demo_crud_system(void);

/* Funcții CRUD */
int db_create(DatabaseContext *ctx, const char *filename);
int db_open(DatabaseContext *ctx, const char *filename);
int db_close(DatabaseContext *ctx);
int db_add_student(DatabaseContext *ctx, const Student *student);
int db_get_student(DatabaseContext *ctx, uint32_t id, Student *student);
int db_update_student(DatabaseContext *ctx, uint32_t id, const Student *student);
int db_delete_student(DatabaseContext *ctx, uint32_t id);
int db_list_all(DatabaseContext *ctx);
long db_find_student_offset(DatabaseContext *ctx, uint32_t id);

/* =============================================================================
 * IMPLEMENTARE - FUNCȚII UTILITARE
 * =============================================================================
 */

/**
 * Afișează un separator cu titlu pentru organizarea output-ului
 */
void print_separator(const char *title) {
    printf("\n");
    printf(COLOR_CYAN "╔═══════════════════════════════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET "      " COLOR_BOLD "%s" COLOR_RESET, title);
    /* Padding pentru aliniere */
    int len = strlen(title);
    for (int i = len; i < 55; i++) printf(" ");
    printf(COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    printf("\n");
}

/**
 * Afișează date în format hexazecimal
 * Util pentru vizualizarea reprezentării binare
 */
void print_bytes_hex(const void *data, size_t size, const char *label) {
    const uint8_t *bytes = (const uint8_t *)data;
    printf(COLOR_YELLOW "%s:" COLOR_RESET " ", label);
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", bytes[i]);
        if ((i + 1) % 16 == 0 && i + 1 < size) {
            printf("\n         ");
            for (size_t j = 0; j < strlen(label); j++) printf(" ");
        }
    }
    printf("\n");
}

/**
 * Afișează informațiile unui student
 */
void print_student(const Student *s) {
    printf("  ┌─────────────────────────────────────────────────────────────┐\n");
    printf("  │ " COLOR_BOLD "ID:" COLOR_RESET " %-10u  " COLOR_BOLD "Nume:" COLOR_RESET " %-30s │\n", 
           s->id, s->nume);
    printf("  │ " COLOR_BOLD "Grupa:" COLOR_RESET " %-8s  " COLOR_BOLD "Status:" COLOR_RESET " %-30s │\n", 
           s->grupa, s->activ ? COLOR_GREEN "Activ" COLOR_RESET : COLOR_RED "Inactiv" COLOR_RESET);
    printf("  │ " COLOR_BOLD "Note:" COLOR_RESET " %.1f, %.1f, %.1f, %.1f, %.1f                           │\n",
           s->note[0], s->note[1], s->note[2], s->note[3], s->note[4]);
    printf("  │ " COLOR_BOLD "Media:" COLOR_RESET " " COLOR_GREEN "%.2f" COLOR_RESET "                                                │\n", 
           s->medie);
    printf("  └─────────────────────────────────────────────────────────────┘\n");
}

/**
 * Calculează media aritmetică a notelor
 */
float calculeaza_medie(const float note[], int count) {
    float suma = 0.0f;
    for (int i = 0; i < count; i++) {
        suma += note[i];
    }
    return suma / count;
}

/* =============================================================================
 * PARTEA 1: DIFERENȚE TEXT VS. BINAR
 * =============================================================================
 */

void demo_text_vs_binar(void) {
    print_separator("PARTEA 1: TEXT vs. BINAR");
    
    int numar = 12345;
    float valoare = 3.14159f;
    
    /* Scriere în format TEXT */
    printf(COLOR_CYAN "1. Scriere în format TEXT:" COLOR_RESET "\n");
    FILE *f_text = fopen("demo_text.txt", "w");
    if (f_text) {
        fprintf(f_text, "%d\n%f\n", numar, valoare);
        fclose(f_text);
        printf("   → Fișier creat: demo_text.txt\n");
    }
    
    /* Scriere în format BINAR */
    printf(COLOR_CYAN "\n2. Scriere în format BINAR:" COLOR_RESET "\n");
    FILE *f_bin = fopen("demo_binar.bin", "wb");
    if (f_bin) {
        fwrite(&numar, sizeof(int), 1, f_bin);
        fwrite(&valoare, sizeof(float), 1, f_bin);
        fclose(f_bin);
        printf("   → Fișier creat: demo_binar.bin\n");
    }
    
    /* Comparare dimensiuni */
    printf(COLOR_CYAN "\n3. Comparare dimensiuni:" COLOR_RESET "\n");
    
    f_text = fopen("demo_text.txt", "rb");
    f_bin = fopen("demo_binar.bin", "rb");
    
    if (f_text && f_bin) {
        fseek(f_text, 0, SEEK_END);
        fseek(f_bin, 0, SEEK_END);
        
        long size_text = ftell(f_text);
        long size_bin = ftell(f_bin);
        
        printf("   Fișier text:  %ld bytes\n", size_text);
        printf("   Fișier binar: %ld bytes\n", size_bin);
        printf("   Economie: %.1f%%\n", 
               (1.0 - (double)size_bin / size_text) * 100);
        
        fclose(f_text);
        fclose(f_bin);
    }
    
    /* Vizualizare conținut binar */
    printf(COLOR_CYAN "\n4. Reprezentare în memorie:" COLOR_RESET "\n");
    print_bytes_hex(&numar, sizeof(numar), "int 12345  ");
    print_bytes_hex(&valoare, sizeof(valoare), "float 3.14 ");
    
    /* Citire înapoi */
    printf(COLOR_CYAN "\n5. Citire din fișierul binar:" COLOR_RESET "\n");
    f_bin = fopen("demo_binar.bin", "rb");
    if (f_bin) {
        int nr_citit;
        float val_citita;
        
        fread(&nr_citit, sizeof(int), 1, f_bin);
        fread(&val_citita, sizeof(float), 1, f_bin);
        fclose(f_bin);
        
        printf("   Număr citit:  %d\n", nr_citit);
        printf("   Valoare citită: %f\n", val_citita);
    }
    
    /* Cleanup */
    remove("demo_text.txt");
    remove("demo_binar.bin");
}

/* =============================================================================
 * PARTEA 2: FUNCȚIILE fread() ȘI fwrite()
 * =============================================================================
 */

void demo_fread_fwrite(void) {
    print_separator("PARTEA 2: fread() și fwrite()");
    
    /* Array de numere pentru demonstrație */
    int numere[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    int count = sizeof(numere) / sizeof(numere[0]);
    
    printf(COLOR_CYAN "1. Scriere array cu fwrite():" COLOR_RESET "\n");
    printf("   Array original: ");
    for (int i = 0; i < count; i++) {
        printf("%d ", numere[i]);
    }
    printf("\n");
    
    FILE *f = fopen("demo_array.bin", "wb");
    if (f) {
        /* Sintaxa: fwrite(sursa, dim_element, nr_elemente, fisier) */
        size_t written = fwrite(numere, sizeof(int), count, f);
        printf("   Elemente scrise: %zu\n", written);
        printf("   Bytes scriși: %zu\n", written * sizeof(int));
        fclose(f);
    }
    
    printf(COLOR_CYAN "\n2. Citire array cu fread():" COLOR_RESET "\n");
    int numere_citite[10] = {0};
    
    f = fopen("demo_array.bin", "rb");
    if (f) {
        /* Sintaxa: fread(destinatie, dim_element, nr_elemente, fisier) */
        size_t read = fread(numere_citite, sizeof(int), count, f);
        printf("   Elemente citite: %zu\n", read);
        printf("   Array citit: ");
        for (int i = 0; i < count; i++) {
            printf("%d ", numere_citite[i]);
        }
        printf("\n");
        fclose(f);
    }
    
    printf(COLOR_CYAN "\n3. Citire parțială:" COLOR_RESET "\n");
    f = fopen("demo_array.bin", "rb");
    if (f) {
        int partial[3];
        /* Citim doar 3 elemente */
        size_t read = fread(partial, sizeof(int), 3, f);
        printf("   Primele %zu elemente: ", read);
        for (size_t i = 0; i < read; i++) {
            printf("%d ", partial[i]);
        }
        printf("\n");
        
        /* Citim următoarele 2 */
        read = fread(partial, sizeof(int), 2, f);
        printf("   Următoarele %zu elemente: ", read);
        for (size_t i = 0; i < read; i++) {
            printf("%d ", partial[i]);
        }
        printf("\n");
        fclose(f);
    }
    
    printf(COLOR_CYAN "\n4. Verificare return value:" COLOR_RESET "\n");
    printf("   fread() returnează numărul de elemente citite\n");
    printf("   Dacă return < count cerut → EOF sau eroare\n");
    printf("   Verificați întotdeauna valoarea returnată!\n");
    
    /* Cleanup */
    remove("demo_array.bin");
}

/* =============================================================================
 * PARTEA 3: FUNCȚIILE fseek() ȘI ftell()
 * =============================================================================
 */

void demo_fseek_ftell(void) {
    print_separator("PARTEA 3: fseek() și ftell()");
    
    /* Creăm un fișier cu 10 înregistrări */
    printf(COLOR_CYAN "1. Creare fișier cu 10 înregistrări:" COLOR_RESET "\n");
    
    FILE *f = fopen("demo_seek.bin", "wb");
    if (f) {
        for (int i = 0; i < 10; i++) {
            int val = (i + 1) * 100;  /* 100, 200, ..., 1000 */
            fwrite(&val, sizeof(int), 1, f);
            printf("   [%d] = %d (offset: %ld)\n", i, val, (long)(i * sizeof(int)));
        }
        fclose(f);
    }
    
    printf(COLOR_CYAN "\n2. Navigare cu fseek():" COLOR_RESET "\n");
    f = fopen("demo_seek.bin", "rb");
    if (f) {
        int val;
        
        /* SEEK_SET - de la început */
        printf("\n   " COLOR_YELLOW "SEEK_SET" COLOR_RESET " - poziționare de la început:\n");
        fseek(f, 3 * sizeof(int), SEEK_SET);  /* Element 3 */
        fread(&val, sizeof(int), 1, f);
        printf("   fseek(f, 3*sizeof(int), SEEK_SET) → val = %d\n", val);
        
        /* SEEK_CUR - de la poziția curentă */
        printf("\n   " COLOR_YELLOW "SEEK_CUR" COLOR_RESET " - poziționare relativă:\n");
        /* Suntem acum la elementul 4, mergem înapoi 2 poziții */
        fseek(f, -2 * sizeof(int), SEEK_CUR);
        fread(&val, sizeof(int), 1, f);
        printf("   fseek(f, -2*sizeof(int), SEEK_CUR) → val = %d\n", val);
        
        /* SEEK_END - de la sfârșit */
        printf("\n   " COLOR_YELLOW "SEEK_END" COLOR_RESET " - poziționare de la sfârșit:\n");
        fseek(f, -1 * sizeof(int), SEEK_END);  /* Ultimul element */
        fread(&val, sizeof(int), 1, f);
        printf("   fseek(f, -sizeof(int), SEEK_END) → val = %d\n", val);
        
        fclose(f);
    }
    
    printf(COLOR_CYAN "\n3. Determinare dimensiune fișier cu fseek/ftell:" COLOR_RESET "\n");
    f = fopen("demo_seek.bin", "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);  /* Revenire la început */
        
        printf("   Dimensiune fișier: %ld bytes\n", size);
        printf("   Număr elemente: %ld\n", size / sizeof(int));
        fclose(f);
    }
    
    printf(COLOR_CYAN "\n4. Acces aleatoriu (random access):" COLOR_RESET "\n");
    printf("   Avantajul fișierelor binare: O(1) access la orice element!\n");
    printf("   Formula: offset = index * sizeof(element)\n");
    
    f = fopen("demo_seek.bin", "rb");
    if (f) {
        int indices[] = {7, 2, 9, 0, 5};
        printf("   Citire în ordine: 7, 2, 9, 0, 5\n");
        printf("   Valori: ");
        for (int i = 0; i < 5; i++) {
            int val;
            fseek(f, indices[i] * sizeof(int), SEEK_SET);
            fread(&val, sizeof(int), 1, f);
            printf("%d ", val);
        }
        printf("\n");
        fclose(f);
    }
    
    /* Cleanup */
    remove("demo_seek.bin");
}

/* =============================================================================
 * PARTEA 4: SERIALIZARE STRUCTURI
 * =============================================================================
 */

void demo_serializare(void) {
    print_separator("PARTEA 4: SERIALIZARE STRUCTURI");
    
    /* Structură normală (cu padding) */
    struct NormalStruct {
        char a;      /* 1 byte */
        int b;       /* 4 bytes */
        char c;      /* 1 byte */
    };
    
    /* Structură packed (fără padding) */
    #pragma pack(push, 1)
    struct PackedStruct {
        char a;      /* 1 byte */
        int b;       /* 4 bytes */
        char c;      /* 1 byte */
    };
    #pragma pack(pop)
    
    printf(COLOR_CYAN "1. Efectul padding-ului:" COLOR_RESET "\n");
    printf("   sizeof(struct NormalStruct): %zu bytes\n", sizeof(struct NormalStruct));
    printf("   sizeof(struct PackedStruct): %zu bytes\n", sizeof(struct PackedStruct));
    printf("   Diferență: %zu bytes de padding!\n", 
           sizeof(struct NormalStruct) - sizeof(struct PackedStruct));
    
    /* Demonstrație vizuală */
    printf(COLOR_CYAN "\n2. Layout în memorie:" COLOR_RESET "\n");
    printf("   NormalStruct:  [a][P][P][P][ b  ][ b  ][c][P][P][P]\n");
    printf("   PackedStruct:  [a][ b  ][ b  ][c]\n");
    printf("   P = byte de padding\n");
    
    /* Serializare Student */
    printf(COLOR_CYAN "\n3. Serializare structură Student:" COLOR_RESET "\n");
    printf("   sizeof(Student): %zu bytes\n", sizeof(Student));
    
    Student s = {
        .id = 1001,
        .nume = "Popescu Ion Alexandru",
        .grupa = "1234A",
        .note = {9.5f, 8.0f, 10.0f, 7.5f, 9.0f},
        .medie = 0.0f,
        .activ = 1
    };
    s.medie = calculeaza_medie(s.note, 5);
    
    printf(COLOR_CYAN "\n4. Scriere și citire structură:" COLOR_RESET "\n");
    
    FILE *f = fopen("demo_student.bin", "wb");
    if (f) {
        fwrite(&s, sizeof(Student), 1, f);
        printf("   Student salvat în fișier.\n");
        fclose(f);
    }
    
    /* Citire înapoi */
    Student s_citit = {0};
    f = fopen("demo_student.bin", "rb");
    if (f) {
        fread(&s_citit, sizeof(Student), 1, f);
        printf("   Student citit din fișier:\n");
        print_student(&s_citit);
        fclose(f);
    }
    
    /* Vizualizare bytes */
    printf(COLOR_CYAN "\n5. Primii 20 bytes din structură:" COLOR_RESET "\n");
    print_bytes_hex(&s, 20, "Student ");
    
    /* Cleanup */
    remove("demo_student.bin");
}

/* =============================================================================
 * PARTEA 5: ENDIANNESS ȘI PORTABILITATE
 * =============================================================================
 */

void demo_endianness(void) {
    print_separator("PARTEA 5: ENDIANNESS");
    
    /* Detectare endianness sistem */
    printf(COLOR_CYAN "1. Detectare endianness:" COLOR_RESET "\n");
    
    uint32_t test = 0x01020304;
    uint8_t *bytes = (uint8_t *)&test;
    
    if (bytes[0] == 0x04) {
        printf("   Sistemul dvs. este " COLOR_GREEN "Little-Endian" COLOR_RESET "\n");
        printf("   (Intel/AMD x86, x64, ARM în mod implicit)\n");
    } else if (bytes[0] == 0x01) {
        printf("   Sistemul dvs. este " COLOR_YELLOW "Big-Endian" COLOR_RESET "\n");
        printf("   (SPARC, PowerPC, rețea)\n");
    }
    
    printf(COLOR_CYAN "\n2. Reprezentarea numărului 0x01020304:" COLOR_RESET "\n");
    print_bytes_hex(&test, sizeof(test), "Memory  ");
    printf("\n   Little-Endian: 04 03 02 01 (LSB primul)\n");
    printf("   Big-Endian:    01 02 03 04 (MSB primul)\n");
    
    printf(COLOR_CYAN "\n3. Funcții de conversie (POSIX):" COLOR_RESET "\n");
    printf("   htonl() - Host TO Network Long (32-bit)\n");
    printf("   htons() - Host TO Network Short (16-bit)\n");
    printf("   ntohl() - Network TO Host Long\n");
    printf("   ntohs() - Network TO Host Short\n");
    
    printf(COLOR_CYAN "\n4. Exemplu conversie manuală:" COLOR_RESET "\n");
    
    uint32_t original = 0x12345678;
    printf("   Original: 0x%08X\n", original);
    print_bytes_hex(&original, sizeof(original), "Bytes   ");
    
    /* Swap manual pentru demonstrație */
    uint32_t swapped = ((original & 0xFF000000) >> 24) |
                       ((original & 0x00FF0000) >> 8)  |
                       ((original & 0x0000FF00) << 8)  |
                       ((original & 0x000000FF) << 24);
    
    printf("\n   Swapped:  0x%08X\n", swapped);
    print_bytes_hex(&swapped, sizeof(swapped), "Bytes   ");
    
    printf(COLOR_CYAN "\n5. Recomandări pentru portabilitate:" COLOR_RESET "\n");
    printf("   • Folosiți tipuri cu dimensiune fixă (int32_t, uint64_t)\n");
    printf("   • Convertiți la network byte order înainte de scriere\n");
    printf("   • Documentați formatul fișierului (endianness folosit)\n");
    printf("   • Sau: serializați câmp cu câmp în format text/JSON\n");
}

/* =============================================================================
 * PARTEA 6: SISTEM CRUD COMPLET
 * =============================================================================
 */

/**
 * Creează o nouă bază de date
 */
int db_create(DatabaseContext *ctx, const char *filename) {
    ctx->file = fopen(filename, "wb+");
    if (!ctx->file) {
        fprintf(stderr, COLOR_RED "Eroare: Nu s-a putut crea fișierul %s\n" COLOR_RESET, 
                filename);
        return -1;
    }
    
    strncpy(ctx->filename, filename, sizeof(ctx->filename) - 1);
    
    /* Inițializare header */
    ctx->header.magic = MAGIC_NUMBER;
    ctx->header.version = FILE_VERSION;
    ctx->header.record_count = 0;
    ctx->header.record_size = sizeof(Student);
    ctx->header.created_at = (uint64_t)time(NULL);
    ctx->header.modified_at = ctx->header.created_at;
    memset(ctx->header.reserved, 0, sizeof(ctx->header.reserved));
    
    /* Scriere header */
    fwrite(&ctx->header, sizeof(FileHeader), 1, ctx->file);
    fflush(ctx->file);
    
    ctx->is_open = 1;
    return 0;
}

/**
 * Deschide o bază de date existentă
 */
int db_open(DatabaseContext *ctx, const char *filename) {
    ctx->file = fopen(filename, "rb+");
    if (!ctx->file) {
        fprintf(stderr, COLOR_RED "Eroare: Nu s-a putut deschide fișierul %s\n" COLOR_RESET, 
                filename);
        return -1;
    }
    
    strncpy(ctx->filename, filename, sizeof(ctx->filename) - 1);
    
    /* Citire și validare header */
    if (fread(&ctx->header, sizeof(FileHeader), 1, ctx->file) != 1) {
        fprintf(stderr, COLOR_RED "Eroare: Header invalid\n" COLOR_RESET);
        fclose(ctx->file);
        return -1;
    }
    
    if (ctx->header.magic != MAGIC_NUMBER) {
        fprintf(stderr, COLOR_RED "Eroare: Magic number invalid (0x%08X != 0x%08X)\n" COLOR_RESET,
                ctx->header.magic, MAGIC_NUMBER);
        fclose(ctx->file);
        return -1;
    }
    
    if (ctx->header.version != FILE_VERSION) {
        fprintf(stderr, COLOR_YELLOW "Avertisment: Versiune diferită (%u != %u)\n" COLOR_RESET,
                ctx->header.version, FILE_VERSION);
    }
    
    ctx->is_open = 1;
    return 0;
}

/**
 * Închide baza de date
 */
int db_close(DatabaseContext *ctx) {
    if (!ctx->is_open) return -1;
    
    /* Actualizare header înainte de închidere */
    ctx->header.modified_at = (uint64_t)time(NULL);
    fseek(ctx->file, 0, SEEK_SET);
    fwrite(&ctx->header, sizeof(FileHeader), 1, ctx->file);
    
    fclose(ctx->file);
    ctx->is_open = 0;
    return 0;
}

/**
 * Găsește offset-ul unui student după ID
 */
long db_find_student_offset(DatabaseContext *ctx, uint32_t id) {
    if (!ctx->is_open) return -1;
    
    fseek(ctx->file, sizeof(FileHeader), SEEK_SET);
    
    Student s;
    for (uint32_t i = 0; i < ctx->header.record_count; i++) {
        long offset = ftell(ctx->file);
        if (fread(&s, sizeof(Student), 1, ctx->file) != 1) break;
        
        if (s.id == id && s.activ) {
            return offset;
        }
    }
    
    return -1;  /* Nu s-a găsit */
}

/**
 * Adaugă un student nou
 */
int db_add_student(DatabaseContext *ctx, const Student *student) {
    if (!ctx->is_open) return -1;
    
    /* Verificare duplicat */
    if (db_find_student_offset(ctx, student->id) >= 0) {
        fprintf(stderr, COLOR_YELLOW "Avertisment: Student cu ID %u există deja\n" COLOR_RESET,
                student->id);
        return -1;
    }
    
    /* Poziționare la sfârșit */
    fseek(ctx->file, 0, SEEK_END);
    
    /* Scriere student */
    if (fwrite(student, sizeof(Student), 1, ctx->file) != 1) {
        fprintf(stderr, COLOR_RED "Eroare la scriere student\n" COLOR_RESET);
        return -1;
    }
    
    ctx->header.record_count++;
    fflush(ctx->file);
    
    return 0;
}

/**
 * Citește un student după ID
 */
int db_get_student(DatabaseContext *ctx, uint32_t id, Student *student) {
    if (!ctx->is_open) return -1;
    
    long offset = db_find_student_offset(ctx, id);
    if (offset < 0) {
        return -1;  /* Nu s-a găsit */
    }
    
    fseek(ctx->file, offset, SEEK_SET);
    fread(student, sizeof(Student), 1, ctx->file);
    
    return 0;
}

/**
 * Actualizează un student
 */
int db_update_student(DatabaseContext *ctx, uint32_t id, const Student *student) {
    if (!ctx->is_open) return -1;
    
    long offset = db_find_student_offset(ctx, id);
    if (offset < 0) {
        fprintf(stderr, COLOR_YELLOW "Student cu ID %u nu a fost găsit\n" COLOR_RESET, id);
        return -1;
    }
    
    /* Scriere la poziția găsită */
    fseek(ctx->file, offset, SEEK_SET);
    fwrite(student, sizeof(Student), 1, ctx->file);
    fflush(ctx->file);
    
    return 0;
}

/**
 * Șterge un student (soft delete)
 */
int db_delete_student(DatabaseContext *ctx, uint32_t id) {
    if (!ctx->is_open) return -1;
    
    long offset = db_find_student_offset(ctx, id);
    if (offset < 0) {
        fprintf(stderr, COLOR_YELLOW "Student cu ID %u nu a fost găsit\n" COLOR_RESET, id);
        return -1;
    }
    
    /* Marcăm ca inactiv */
    Student s;
    fseek(ctx->file, offset, SEEK_SET);
    fread(&s, sizeof(Student), 1, ctx->file);
    
    s.activ = 0;
    
    fseek(ctx->file, offset, SEEK_SET);
    fwrite(&s, sizeof(Student), 1, ctx->file);
    fflush(ctx->file);
    
    return 0;
}

/**
 * Listează toți studenții activi
 */
int db_list_all(DatabaseContext *ctx) {
    if (!ctx->is_open) return -1;
    
    printf("\n");
    printf(COLOR_CYAN "  Total înregistrări: %u\n" COLOR_RESET, ctx->header.record_count);
    printf("\n");
    
    fseek(ctx->file, sizeof(FileHeader), SEEK_SET);
    
    Student s;
    int count_active = 0;
    
    for (uint32_t i = 0; i < ctx->header.record_count; i++) {
        if (fread(&s, sizeof(Student), 1, ctx->file) != 1) break;
        
        if (s.activ) {
            print_student(&s);
            count_active++;
        }
    }
    
    printf("\n  Studenți activi: %d\n", count_active);
    
    return count_active;
}

void demo_crud_system(void) {
    print_separator("PARTEA 6: SISTEM CRUD COMPLET");
    
    DatabaseContext db = {0};
    const char *db_file = "studenti.db";
    
    /* 1. Creare bază de date */
    printf(COLOR_CYAN "1. Creare bază de date nouă:" COLOR_RESET "\n");
    if (db_create(&db, db_file) == 0) {
        printf(COLOR_GREEN "   ✓ Bază de date creată: %s\n" COLOR_RESET, db_file);
        printf("   Header size: %zu bytes\n", sizeof(FileHeader));
        printf("   Record size: %zu bytes\n", sizeof(Student));
    }
    
    /* 2. Adăugare studenți (CREATE) */
    printf(COLOR_CYAN "\n2. Adăugare studenți (CREATE):" COLOR_RESET "\n");
    
    Student studenti[] = {
        {1001, "Popescu Ion", "1234A", {9.5, 8.0, 10.0, 7.5, 9.0}, 0, 1},
        {1002, "Ionescu Maria", "1234A", {10.0, 9.5, 9.0, 10.0, 9.5}, 0, 1},
        {1003, "Georgescu Ana", "1234B", {7.0, 8.0, 7.5, 8.5, 8.0}, 0, 1},
        {1004, "Dumitrescu Vlad", "1234B", {6.5, 7.0, 8.0, 6.0, 7.5}, 0, 1},
        {1005, "Constantinescu Elena", "1235A", {9.0, 9.5, 9.0, 8.5, 10.0}, 0, 1}
    };
    
    for (int i = 0; i < 5; i++) {
        studenti[i].medie = calculeaza_medie(studenti[i].note, 5);
        if (db_add_student(&db, &studenti[i]) == 0) {
            printf("   + Adăugat: %s (ID: %u, Media: %.2f)\n", 
                   studenti[i].nume, studenti[i].id, studenti[i].medie);
        }
    }
    
    /* 3. Citire student (READ) */
    printf(COLOR_CYAN "\n3. Citire student după ID (READ):" COLOR_RESET "\n");
    Student s_citit;
    if (db_get_student(&db, 1002, &s_citit) == 0) {
        printf("   Student găsit:\n");
        print_student(&s_citit);
    }
    
    /* 4. Actualizare student (UPDATE) */
    printf(COLOR_CYAN "\n4. Actualizare student (UPDATE):" COLOR_RESET "\n");
    s_citit.note[0] = 10.0f;
    s_citit.medie = calculeaza_medie(s_citit.note, 5);
    if (db_update_student(&db, 1002, &s_citit) == 0) {
        printf("   ✓ Student 1002 actualizat (nota[0] = 10.0)\n");
        printf("   Nouă medie: %.2f\n", s_citit.medie);
    }
    
    /* 5. Ștergere student (DELETE) */
    printf(COLOR_CYAN "\n5. Ștergere student (DELETE):" COLOR_RESET "\n");
    if (db_delete_student(&db, 1004) == 0) {
        printf("   ✓ Student 1004 marcat ca șters (soft delete)\n");
    }
    
    /* 6. Listare toți studenții */
    printf(COLOR_CYAN "\n6. Listare studenți activi:" COLOR_RESET "\n");
    db_list_all(&db);
    
    /* 7. Închidere și redeschidere */
    printf(COLOR_CYAN "\n7. Test persistență (închidere și redeschidere):" COLOR_RESET "\n");
    db_close(&db);
    printf("   ✓ Bază de date închisă\n");
    
    DatabaseContext db2 = {0};
    if (db_open(&db2, db_file) == 0) {
        printf("   ✓ Bază de date redeschisă\n");
        printf("   Magic: 0x%08X\n", db2.header.magic);
        printf("   Version: %u\n", db2.header.version);
        printf("   Records: %u\n", db2.header.record_count);
        
        db_close(&db2);
    }
    
    /* Cleanup */
    remove(db_file);
    printf("\n   Fișierul de test a fost șters.\n");
}

/* =============================================================================
 * FUNCȚIA MAIN
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf(COLOR_CYAN "╔═══════════════════════════════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET "     " COLOR_BOLD "SĂPTĂMÂNA 03: FIȘIERE BINARE" COLOR_RESET "                              " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET "     Exemplu Complet Demonstrativ                              " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET "     Algoritmi și Tehnici de Programare                        " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    
    /* Rulare toate demonstrațiile */
    demo_text_vs_binar();
    demo_fread_fwrite();
    demo_fseek_ftell();
    demo_serializare();
    demo_endianness();
    demo_crud_system();
    
    printf("\n");
    printf(COLOR_GREEN "╔═══════════════════════════════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_GREEN "║" COLOR_RESET "     " COLOR_BOLD "✓ Toate demonstrațiile finalizate cu succes!" COLOR_RESET "               " COLOR_GREEN "║\n" COLOR_RESET);
    printf(COLOR_GREEN "╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    printf("\n");
    
    return 0;
}

/* =============================================================================
 * SFARSIT FISIER
 * =============================================================================
 */
