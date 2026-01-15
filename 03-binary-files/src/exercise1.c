/**
 * =============================================================================
 * EXERCIȚIUL 1: Gestiune Catalog Studenți (Fișiere Binare)
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați un sistem complet de gestiune a catalogului de studenți
 *   utilizând fișiere binare pentru persistența datelor.
 *
 * CERINȚE:
 *   1. Definiți structura Student cu câmpurile specificate
 *   2. Implementați funcțiile pentru operații CRUD
 *   3. Utilizați header cu magic number și versiune
 *   4. Implementați căutare binară după ID
 *   5. Gestionați corect toate erorile
 *   6. Afișați statistici despre operații
 *
 * EXEMPLU INPUT:
 *   1 (adaugă student)
 *   1001 Popescu Ion 1234A 9.5 8.0 10.0 7.5 9.0
 *   2 (afișează toți)
 *   3 (caută după ID) 1001
 *   4 (actualizează) 1001 ...
 *   5 (șterge) 1001
 *   0 (ieșire)
 *
 * OUTPUT AȘTEPTAT:
 *   Operații efectuate cu succes, statistici afișate
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c -lm
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
 * CONSTANTE
 * =============================================================================
 */

#define MAX_NUME        50
#define MAX_GRUPA       10
#define NR_NOTE         5
#define FILENAME        "catalog.bin"

/**
 * TODO 1: Definiți constante pentru magic number și versiune
 *
 * Magic number-ul este un identificator unic pentru formatul fișierului.
 * Convenție: utilizați 4 caractere ASCII convertite în uint32_t
 * Exemplu: "CTLG" = 0x47544C43
 *
 * Hint: Puteți calcula astfel: 'C' | ('T'<<8) | ('L'<<16) | ('G'<<24)
 */

/* YOUR CODE HERE */
#define MAGIC_NUMBER    0x00000000  /* Înlocuiți cu valoarea corectă */
#define FILE_VERSION    1

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * TODO 2: Definiți structura FileHeader
 *
 * Header-ul fișierului trebuie să conțină:
 *   - magic (uint32_t): pentru validarea formatului
 *   - version (uint32_t): versiunea formatului
 *   - record_count (uint32_t): numărul de studenți
 *   - created_at (uint64_t): timestamp creare
 *   - modified_at (uint64_t): timestamp ultima modificare
 *
 * Utilizați #pragma pack(push, 1) pentru a evita padding-ul!
 */

/* YOUR CODE HERE */
#pragma pack(push, 1)
typedef struct {
    /* Completați câmpurile header-ului */
    uint32_t placeholder;  /* Înlocuiți cu câmpurile corecte */
} FileHeader;
#pragma pack(pop)

/**
 * TODO 3: Definiți structura Student
 *
 * Câmpuri necesare:
 *   - id (uint32_t): identificator unic
 *   - nume (char[MAX_NUME]): numele complet
 *   - grupa (char[MAX_GRUPA]): codul grupei
 *   - note (float[NR_NOTE]): cele 5 note
 *   - medie (float): media calculată
 *   - activ (uint8_t): 1=activ, 0=șters
 *
 * Utilizați #pragma pack pentru consistență la serializare!
 */

/* YOUR CODE HERE */
#pragma pack(push, 1)
typedef struct {
    /* Completați câmpurile structurii Student */
    uint32_t id;
    /* ... restul câmpurilor ... */
} Student;
#pragma pack(pop)

/**
 * Statistici pentru operații
 */
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

static Statistici stats = {0, 0, 0, 0, 0, 0};

/* =============================================================================
 * PROTOTIPURI FUNCȚII
 * =============================================================================
 */

/* Funcții utilitare */
float calculeaza_medie(const float note[], int count);
void afiseaza_student(const Student *s);
void afiseaza_statistici(void);
void afiseaza_meniu(void);

/* Funcții pentru fișiere */
int creeaza_fisier_nou(const char *filename);
int deschide_fisier(const char *filename, FILE **f, FileHeader *header);
int salveaza_header(FILE *f, const FileHeader *header);

/* Operații CRUD */
int adauga_student(const char *filename);
int afiseaza_toti(const char *filename);
int cauta_student(const char *filename, uint32_t id, Student *rezultat);
int actualizeaza_student(const char *filename, uint32_t id);
int sterge_student(const char *filename, uint32_t id);

/* Căutare binară */
int cauta_binar(FILE *f, uint32_t nr_studenti, uint32_t id_cautat, Student *rezultat);

/* =============================================================================
 * IMPLEMENTARE FUNCȚII UTILITARE
 * =============================================================================
 */

/**
 * TODO 4: Implementați funcția calculeaza_medie
 *
 * @param note Array de note (float)
 * @param count Numărul de note
 * @return Media aritmetică a notelor
 *
 * Steps:
 *   1. Inițializați suma la 0
 *   2. Iterați prin toate notele și adunați-le
 *   3. Returnați suma / count
 */
float calculeaza_medie(const float note[], int count) {
    /* YOUR CODE HERE */
    return 0.0f;  /* Înlocuiți cu implementarea corectă */
}

/**
 * Afișează informațiile unui student
 */
void afiseaza_student(const Student *s) {
    if (!s) return;
    
    printf("\n┌─────────────────────────────────────────────────────────────┐\n");
    printf("│ ID: %-10u  Nume: %-35s│\n", s->id, s->nume);
    printf("│ Grupa: %-8s  Status: %-34s│\n", 
           s->grupa, s->activ ? "Activ" : "Inactiv");
    printf("│ Note: ");
    for (int i = 0; i < NR_NOTE; i++) {
        printf("%.1f ", s->note[i]);
    }
    printf("                                    │\n");
    printf("│ Media: %.2f                                                 │\n", s->medie);
    printf("└─────────────────────────────────────────────────────────────┘\n");
}

/**
 * Afișează statisticile operațiilor
 */
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

/**
 * Afișează meniul principal
 */
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
 * IMPLEMENTARE FUNCȚII PENTRU FIȘIERE
 * =============================================================================
 */

/**
 * TODO 5: Implementați funcția creeaza_fisier_nou
 *
 * @param filename Numele fișierului de creat
 * @return 0 pentru succes, -1 pentru eroare
 *
 * Steps:
 *   1. Deschideți fișierul în modul "wb" (write binary)
 *   2. Inițializați header-ul cu valorile corespunzătoare:
 *      - magic = MAGIC_NUMBER
 *      - version = FILE_VERSION
 *      - record_count = 0
 *      - created_at = timestamp curent (time(NULL))
 *      - modified_at = timestamp curent
 *   3. Scrieți header-ul în fișier cu fwrite()
 *   4. Închideți fișierul
 *   5. Returnați 0 pentru succes
 *
 * Hint: Verificați fiecare operație și returnați -1 în caz de eroare
 */
int creeaza_fisier_nou(const char *filename) {
    /* YOUR CODE HERE */
    
    return -1;  /* Înlocuiți cu implementarea corectă */
}

/**
 * TODO 6: Implementați funcția deschide_fisier
 *
 * @param filename Numele fișierului
 * @param f Pointer către FILE* (output)
 * @param header Pointer către FileHeader pentru a stoca header-ul citit
 * @return 0 pentru succes, -1 pentru eroare
 *
 * Steps:
 *   1. Încercați să deschideți fișierul în modul "rb+" (read/write binary)
 *   2. Dacă nu există, creați-l cu creeaza_fisier_nou() și redeschideți-l
 *   3. Citiți header-ul cu fread()
 *   4. Validați magic number-ul
 *   5. Stocați file pointer în *f
 *   6. Returnați 0 pentru succes
 */
int deschide_fisier(const char *filename, FILE **f, FileHeader *header) {
    /* YOUR CODE HERE */
    
    return -1;  /* Înlocuiți cu implementarea corectă */
}

/**
 * TODO 7: Implementați funcția salveaza_header
 *
 * @param f File pointer deschis
 * @param header Header-ul de salvat
 * @return 0 pentru succes, -1 pentru eroare
 *
 * Steps:
 *   1. Poziționați-vă la începutul fișierului cu fseek(f, 0, SEEK_SET)
 *   2. Scrieți header-ul cu fwrite()
 *   3. Apelați fflush() pentru a forța scrierea pe disc
 *   4. Returnați 0 pentru succes
 */
int salveaza_header(FILE *f, const FileHeader *header) {
    /* YOUR CODE HERE */
    
    return -1;  /* Înlocuiți cu implementarea corectă */
}

/* =============================================================================
 * IMPLEMENTARE OPERAȚII CRUD
 * =============================================================================
 */

/**
 * TODO 8: Implementați funcția adauga_student
 *
 * @param filename Numele fișierului
 * @return 0 pentru succes, -1 pentru eroare
 *
 * Steps:
 *   1. Deschideți fișierul
 *   2. Citiți datele studentului de la tastatură (scanf)
 *   3. Calculați media
 *   4. Setați activ = 1
 *   5. Poziționați-vă la sfârșitul fișierului (SEEK_END)
 *   6. Scrieți studentul cu fwrite()
 *   7. Actualizați record_count în header
 *   8. Actualizați modified_at
 *   9. Salvați header-ul
 *   10. Închideți fișierul
 *   11. Incrementați stats.adaugari
 */
int adauga_student(const char *filename) {
    FILE *f = NULL;
    FileHeader header;
    Student s;
    
    /* YOUR CODE HERE */
    
    /* Hint: Citiți datele astfel:
     * printf("ID: ");
     * scanf("%u", &s.id);
     * printf("Nume: ");
     * scanf(" %[^\n]", s.nume);  // citește până la newline
     * printf("Grupa: ");
     * scanf("%s", s.grupa);
     * printf("Note (5 valori): ");
     * for (int i = 0; i < NR_NOTE; i++) {
     *     scanf("%f", &s.note[i]);
     * }
     */
    
    return -1;  /* Înlocuiți cu implementarea corectă */
}

/**
 * TODO 9: Implementați funcția afiseaza_toti
 *
 * @param filename Numele fișierului
 * @return Numărul de studenți activi afișați
 *
 * Steps:
 *   1. Deschideți fișierul
 *   2. Iterați prin toate înregistrările (header.record_count)
 *   3. Pentru fiecare student activ, afișați-l
 *   4. Numărați studenții activi
 *   5. Închideți fișierul
 *   6. Incrementați stats.citiri
 */
int afiseaza_toti(const char *filename) {
    FILE *f = NULL;
    FileHeader header;
    Student s;
    int count_activi = 0;
    
    /* YOUR CODE HERE */
    
    return count_activi;
}

/**
 * TODO 10: Implementați funcția cauta_student
 *
 * @param filename Numele fișierului
 * @param id ID-ul studentului căutat
 * @param rezultat Pointer pentru stocarea studentului găsit (poate fi NULL)
 * @return 0 dacă găsit, -1 dacă nu
 *
 * Steps:
 *   1. Deschideți fișierul
 *   2. Iterați prin studenți și comparați ID-ul
 *   3. Dacă găsiți și rezultat != NULL, copiați datele
 *   4. Închideți fișierul
 *   5. Incrementați stats.cautari
 *
 * BONUS: Implementați căutare binară (presupunând fișierul sortat)
 */
int cauta_student(const char *filename, uint32_t id, Student *rezultat) {
    FILE *f = NULL;
    FileHeader header;
    
    /* YOUR CODE HERE */
    
    stats.cautari++;
    return -1;  /* Înlocuiți cu implementarea corectă */
}

/**
 * TODO 11: Implementați funcția actualizeaza_student
 *
 * @param filename Numele fișierului
 * @param id ID-ul studentului de actualizat
 * @return 0 pentru succes, -1 pentru eroare
 *
 * Steps:
 *   1. Găsiți studentul și salvați offset-ul
 *   2. Citiți noile date
 *   3. Poziționați-vă la offset-ul găsit
 *   4. Scrieți studentul actualizat
 *   5. Actualizați modified_at în header
 *   6. Incrementați stats.actualizari
 */
int actualizeaza_student(const char *filename, uint32_t id) {
    FILE *f = NULL;
    FileHeader header;
    Student s;
    long offset = -1;
    
    /* YOUR CODE HERE */
    
    return -1;  /* Înlocuiți cu implementarea corectă */
}

/**
 * TODO 12: Implementați funcția sterge_student (soft delete)
 *
 * @param filename Numele fișierului
 * @param id ID-ul studentului de șters
 * @return 0 pentru succes, -1 pentru eroare
 *
 * Steps:
 *   1. Găsiți studentul
 *   2. Setați activ = 0 (soft delete)
 *   3. Rescrieți studentul la poziția sa
 *   4. Actualizați modified_at
 *   5. Incrementați stats.stergeri
 *
 * Notă: NU ștergeți fizic - doar marcați ca inactiv!
 */
int sterge_student(const char *filename, uint32_t id) {
    FILE *f = NULL;
    FileHeader header;
    Student s;
    
    /* YOUR CODE HERE */
    
    return -1;  /* Înlocuiți cu implementarea corectă */
}

/* =============================================================================
 * FUNCȚII BONUS
 * =============================================================================
 */

/**
 * BONUS TODO: Implementați căutare binară
 *
 * Presupunem că fișierul este sortat după ID.
 * Complexitate: O(log n) vs O(n) pentru căutare liniară
 *
 * @param f File pointer poziționat după header
 * @param nr_studenti Numărul total de studenți
 * @param id_cautat ID-ul de găsit
 * @param rezultat Pointer pentru stocarea rezultatului
 * @return Offset-ul studentului sau -1 dacă nu s-a găsit
 */
int cauta_binar(FILE *f, uint32_t nr_studenti, uint32_t id_cautat, Student *rezultat) {
    /* YOUR CODE HERE */
    
    /* Hint:
     * long left = 0;
     * long right = nr_studenti - 1;
     * long header_size = sizeof(FileHeader);
     * long record_size = sizeof(Student);
     *
     * while (left <= right) {
     *     long mid = (left + right) / 2;
     *     long offset = header_size + mid * record_size;
     *     
     *     fseek(f, offset, SEEK_SET);
     *     fread(&s, sizeof(Student), 1, f);
     *     
     *     if (s.id == id_cautat && s.activ) {
     *         if (rezultat) *rezultat = s;
     *         return offset;
     *     } else if (s.id < id_cautat) {
     *         left = mid + 1;
     *     } else {
     *         right = mid - 1;
     *     }
     * }
     */
    
    return -1;
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
    printf("║           EXERCIȚIUL 1: CATALOG STUDENȚI                      ║\n");
    printf("║           Săptămâna 03 - Fișiere Binare                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Verificăm dacă fișierul există, altfel îl creăm */
    FILE *test = fopen(FILENAME, "rb");
    if (!test) {
        printf("\nCreare fișier nou: %s\n", FILENAME);
        if (creeaza_fisier_nou(FILENAME) < 0) {
            fprintf(stderr, "Eroare la crearea fișierului!\n");
            return 1;
        }
    } else {
        fclose(test);
    }
    
    do {
        afiseaza_meniu();
        
        if (scanf("%d", &optiune) != 1) {
            printf("Input invalid!\n");
            while (getchar() != '\n');  /* curățare buffer */
            continue;
        }
        
        switch (optiune) {
            case 1:
                if (adauga_student(FILENAME) == 0) {
                    printf("Student adăugat cu succes!\n");
                } else {
                    printf("Eroare la adăugare!\n");
                    stats.erori++;
                }
                break;
                
            case 2:
                {
                    int count = afiseaza_toti(FILENAME);
                    printf("\nTotal studenți activi: %d\n", count);
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
                    printf("Student actualizat cu succes!\n");
                } else {
                    printf("Eroare la actualizare!\n");
                    stats.erori++;
                }
                break;
                
            case 5:
                printf("Introduceți ID studentului de șters: ");
                scanf("%u", &id);
                if (sterge_student(FILENAME, id) == 0) {
                    printf("Student șters cu succes!\n");
                } else {
                    printf("Eroare la ștergere!\n");
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
    
    /* Afișare statistici finale */
    afiseaza_statistici();
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opționale)
 * =============================================================================
 *
 * 1. Implementați sortare a fișierului după ID pentru a permite căutare binară
 *
 * 2. Adăugați export în format CSV
 *
 * 3. Implementați compactare (eliminare fizică a înregistrărilor șterse)
 *
 * 4. Adăugați căutare după nume (substring match)
 *
 * 5. Implementați backup automat înainte de modificări
 *
 * =============================================================================
 */
