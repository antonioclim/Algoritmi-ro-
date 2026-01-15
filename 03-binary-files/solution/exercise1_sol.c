/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 1: Gestiune Catalog Studenți
 * =============================================================================
 * 
 * ATENȚIE: Acest fișier este destinat DOAR instructorilor!
 * Nu distribuiți studenților înainte de termenul limită.
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c -lm
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_NUME        50
#define MAX_GRUPA       10
#define NR_NOTE         5
#define FILENAME        "catalog.bin"
#define MAGIC_NUMBER    0x47544C43  /* "CTLG" în little-endian */
#define FILE_VERSION    1

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

#pragma pack(push, 1)
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t record_count;
    uint64_t created_at;
    uint64_t modified_at;
    uint8_t  reserved[32];
} FileHeader;

typedef struct {
    uint32_t id;
    char     nume[MAX_NUME];
    char     grupa[MAX_GRUPA];
    float    note[NR_NOTE];
    float    medie;
    uint8_t  activ;
} Student;
#pragma pack(pop)

typedef struct {
    int adaugari;
    int citiri;
    int actualizari;
    int stergeri;
    int cautari;
    int erori;
} Statistici;

/* =============================================================================
 * VARIABILE GLOBALE
 * =============================================================================
 */

static Statistici stats = {0};

/* =============================================================================
 * FUNCȚII UTILITARE
 * =============================================================================
 */

float calculeaza_medie(const float note[], int count) {
    float suma = 0.0f;
    for (int i = 0; i < count; i++) {
        suma += note[i];
    }
    return suma / count;
}

void afiseaza_student(const Student *s) {
    if (!s) return;
    printf("\n┌─────────────────────────────────────────────────────────────┐\n");
    printf("│ ID: %-10u  Nume: %-35s│\n", s->id, s->nume);
    printf("│ Grupa: %-8s  Status: %-34s│\n", 
           s->grupa, s->activ ? "\033[32mActiv\033[0m" : "\033[31mInactiv\033[0m");
    printf("│ Note: ");
    for (int i = 0; i < NR_NOTE; i++) {
        printf("%.1f ", s->note[i]);
    }
    printf("                                    │\n");
    printf("│ Media: \033[33m%.2f\033[0m                                                 │\n", s->medie);
    printf("└─────────────────────────────────────────────────────────────┘\n");
}

void afiseaza_statistici(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    STATISTICI OPERAȚII                        ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║  Adăugări:     %-5d  │  Citiri:      %-5d                    ║\n", 
           stats.adaugari, stats.citiri);
    printf("║  Actualizări:  %-5d  │  Ștergeri:    %-5d                    ║\n", 
           stats.actualizari, stats.stergeri);
    printf("║  Căutări:      %-5d  │  Erori:       %-5d                    ║\n", 
           stats.cautari, stats.erori);
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
}

void afiseaza_meniu(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                   CATALOG STUDENȚI                            ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║  1. Adaugă student                                            ║\n");
    printf("║  2. Afișează toți studenții                                   ║\n");
    printf("║  3. Caută student după ID                                     ║\n");
    printf("║  4. Actualizează student                                      ║\n");
    printf("║  5. Șterge student                                            ║\n");
    printf("║  6. Afișează statistici                                       ║\n");
    printf("║  0. Ieșire                                                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("Opțiune: ");
}

/* =============================================================================
 * FUNCȚII PENTRU FIȘIERE
 * =============================================================================
 */

int creeaza_fisier_nou(const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        perror("Eroare la crearea fișierului");
        return -1;
    }
    
    FileHeader header = {
        .magic = MAGIC_NUMBER,
        .version = FILE_VERSION,
        .record_count = 0,
        .created_at = (uint64_t)time(NULL),
        .modified_at = (uint64_t)time(NULL)
    };
    memset(header.reserved, 0, sizeof(header.reserved));
    
    if (fwrite(&header, sizeof(FileHeader), 1, f) != 1) {
        perror("Eroare la scrierea header-ului");
        fclose(f);
        return -1;
    }
    
    fclose(f);
    return 0;
}

int deschide_fisier(const char *filename, FILE **f, FileHeader *header) {
    *f = fopen(filename, "rb+");
    if (!*f) {
        /* Fișierul nu există - îl creăm */
        if (creeaza_fisier_nou(filename) < 0) {
            return -1;
        }
        *f = fopen(filename, "rb+");
        if (!*f) {
            perror("Eroare la redeschiderea fișierului");
            return -1;
        }
    }
    
    if (fread(header, sizeof(FileHeader), 1, *f) != 1) {
        perror("Eroare la citirea header-ului");
        fclose(*f);
        return -1;
    }
    
    if (header->magic != MAGIC_NUMBER) {
        fprintf(stderr, "Eroare: Format fișier invalid (magic: 0x%08X)\n", header->magic);
        fclose(*f);
        return -1;
    }
    
    return 0;
}

int salveaza_header(FILE *f, const FileHeader *header) {
    if (fseek(f, 0, SEEK_SET) != 0) {
        perror("Eroare fseek");
        return -1;
    }
    
    if (fwrite(header, sizeof(FileHeader), 1, f) != 1) {
        perror("Eroare la scrierea header-ului");
        return -1;
    }
    
    fflush(f);
    return 0;
}

/* =============================================================================
 * OPERAȚII CRUD
 * =============================================================================
 */

int adauga_student(const char *filename) {
    FILE *f = NULL;
    FileHeader header;
    Student s;
    
    if (deschide_fisier(filename, &f, &header) < 0) {
        return -1;
    }
    
    /* Citire date de la utilizator */
    printf("ID: ");
    if (scanf("%u", &s.id) != 1) {
        fprintf(stderr, "ID invalid\n");
        fclose(f);
        return -1;
    }
    
    /* Verificare duplicat */
    fseek(f, sizeof(FileHeader), SEEK_SET);
    Student temp;
    for (uint32_t i = 0; i < header.record_count; i++) {
        if (fread(&temp, sizeof(Student), 1, f) != 1) break;
        if (temp.id == s.id && temp.activ) {
            fprintf(stderr, "Eroare: Student cu ID %u există deja!\n", s.id);
            fclose(f);
            return -1;
        }
    }
    
    printf("Nume: ");
    scanf(" %[^\n]", s.nume);
    
    printf("Grupa: ");
    scanf("%s", s.grupa);
    
    printf("Note (5 valori separate prin spațiu): ");
    for (int i = 0; i < NR_NOTE; i++) {
        if (scanf("%f", &s.note[i]) != 1) {
            fprintf(stderr, "Notă invalidă\n");
            fclose(f);
            return -1;
        }
    }
    
    s.medie = calculeaza_medie(s.note, NR_NOTE);
    s.activ = 1;
    
    /* Poziționare la sfârșit și scriere */
    fseek(f, 0, SEEK_END);
    if (fwrite(&s, sizeof(Student), 1, f) != 1) {
        perror("Eroare la scrierea studentului");
        fclose(f);
        return -1;
    }
    
    /* Actualizare header */
    header.record_count++;
    header.modified_at = (uint64_t)time(NULL);
    salveaza_header(f, &header);
    
    fclose(f);
    stats.adaugari++;
    return 0;
}

int afiseaza_toti(const char *filename) {
    FILE *f = NULL;
    FileHeader header;
    
    if (deschide_fisier(filename, &f, &header) < 0) {
        return -1;
    }
    
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("                    LISTA STUDENȚI                              \n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Total înregistrări: %u\n", header.record_count);
    
    fseek(f, sizeof(FileHeader), SEEK_SET);
    
    Student s;
    int count_activi = 0;
    
    for (uint32_t i = 0; i < header.record_count; i++) {
        if (fread(&s, sizeof(Student), 1, f) != 1) break;
        
        if (s.activ) {
            afiseaza_student(&s);
            count_activi++;
        }
    }
    
    fclose(f);
    stats.citiri++;
    return count_activi;
}

long gaseste_student_offset(FILE *f, FileHeader *header, uint32_t id) {
    fseek(f, sizeof(FileHeader), SEEK_SET);
    
    Student s;
    for (uint32_t i = 0; i < header->record_count; i++) {
        long offset = ftell(f);
        if (fread(&s, sizeof(Student), 1, f) != 1) break;
        
        if (s.id == id && s.activ) {
            return offset;
        }
    }
    
    return -1;
}

int cauta_student(const char *filename, uint32_t id, Student *rezultat) {
    FILE *f = NULL;
    FileHeader header;
    
    if (deschide_fisier(filename, &f, &header) < 0) {
        return -1;
    }
    
    long offset = gaseste_student_offset(f, &header, id);
    
    if (offset < 0) {
        fclose(f);
        stats.cautari++;
        return -1;
    }
    
    fseek(f, offset, SEEK_SET);
    fread(rezultat, sizeof(Student), 1, f);
    
    fclose(f);
    stats.cautari++;
    return 0;
}

int actualizeaza_student(const char *filename, uint32_t id) {
    FILE *f = NULL;
    FileHeader header;
    
    if (deschide_fisier(filename, &f, &header) < 0) {
        return -1;
    }
    
    long offset = gaseste_student_offset(f, &header, id);
    
    if (offset < 0) {
        fprintf(stderr, "Student cu ID %u nu a fost găsit.\n", id);
        fclose(f);
        return -1;
    }
    
    /* Citire student existent */
    fseek(f, offset, SEEK_SET);
    Student s;
    fread(&s, sizeof(Student), 1, f);
    
    printf("\nStudent curent:\n");
    afiseaza_student(&s);
    
    printf("\nIntroduceți noile note (5 valori): ");
    for (int i = 0; i < NR_NOTE; i++) {
        if (scanf("%f", &s.note[i]) != 1) {
            fprintf(stderr, "Notă invalidă\n");
            fclose(f);
            return -1;
        }
    }
    
    s.medie = calculeaza_medie(s.note, NR_NOTE);
    
    /* Rescriere la aceeași poziție */
    fseek(f, offset, SEEK_SET);
    fwrite(&s, sizeof(Student), 1, f);
    
    /* Actualizare timestamp în header */
    header.modified_at = (uint64_t)time(NULL);
    salveaza_header(f, &header);
    
    fclose(f);
    stats.actualizari++;
    return 0;
}

int sterge_student(const char *filename, uint32_t id) {
    FILE *f = NULL;
    FileHeader header;
    
    if (deschide_fisier(filename, &f, &header) < 0) {
        return -1;
    }
    
    long offset = gaseste_student_offset(f, &header, id);
    
    if (offset < 0) {
        fprintf(stderr, "Student cu ID %u nu a fost găsit.\n", id);
        fclose(f);
        return -1;
    }
    
    /* Citire, marcare ca șters, rescriere */
    fseek(f, offset, SEEK_SET);
    Student s;
    fread(&s, sizeof(Student), 1, f);
    
    s.activ = 0;  /* Soft delete */
    
    fseek(f, offset, SEEK_SET);
    fwrite(&s, sizeof(Student), 1, f);
    
    /* Actualizare timestamp */
    header.modified_at = (uint64_t)time(NULL);
    salveaza_header(f, &header);
    
    fclose(f);
    stats.stergeri++;
    return 0;
}

/* =============================================================================
 * FUNCȚIA MAIN
 * =============================================================================
 */

int main(void) {
    int optiune;
    uint32_t id;
    Student s;
    
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           SOLUȚIE EXERCIȚIUL 1: CATALOG STUDENȚI              ║\n");
    printf("║           Săptămâna 03 - Fișiere Binare                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Verificăm existența fișierului */
    FILE *test = fopen(FILENAME, "rb");
    if (!test) {
        printf("\nCreare fișier nou: %s\n", FILENAME);
        if (creeaza_fisier_nou(FILENAME) < 0) {
            return 1;
        }
    } else {
        fclose(test);
    }
    
    do {
        afiseaza_meniu();
        
        if (scanf("%d", &optiune) != 1) {
            printf("Input invalid!\n");
            while (getchar() != '\n');
            continue;
        }
        
        switch (optiune) {
            case 1:
                if (adauga_student(FILENAME) == 0) {
                    printf("\033[32m✓ Student adăugat cu succes!\033[0m\n");
                } else {
                    printf("\033[31m✗ Eroare la adăugare!\033[0m\n");
                    stats.erori++;
                }
                break;
                
            case 2:
                {
                    int count = afiseaza_toti(FILENAME);
                    if (count >= 0) {
                        printf("\nTotal studenți activi: %d\n", count);
                    }
                }
                break;
                
            case 3:
                printf("Introduceți ID: ");
                scanf("%u", &id);
                if (cauta_student(FILENAME, id, &s) == 0) {
                    printf("Student găsit:\n");
                    afiseaza_student(&s);
                } else {
                    printf("Studentul cu ID %u nu a fost găsit.\n", id);
                }
                break;
                
            case 4:
                printf("Introduceți ID studentului de actualizat: ");
                scanf("%u", &id);
                if (actualizeaza_student(FILENAME, id) == 0) {
                    printf("\033[32m✓ Student actualizat cu succes!\033[0m\n");
                } else {
                    stats.erori++;
                }
                break;
                
            case 5:
                printf("Introduceți ID studentului de șters: ");
                scanf("%u", &id);
                if (sterge_student(FILENAME, id) == 0) {
                    printf("\033[32m✓ Student șters cu succes!\033[0m\n");
                } else {
                    stats.erori++;
                }
                break;
                
            case 6:
                afiseaza_statistici();
                break;
                
            case 0:
                printf("\nLa revedere!\n");
                break;
                
            default:
                printf("Opțiune invalidă!\n");
        }
        
    } while (optiune != 0);
    
    afiseaza_statistici();
    
    return 0;
}
