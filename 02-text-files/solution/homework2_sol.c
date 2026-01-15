/**
 * =============================================================================
 * SOLUȚIE TEMA 2: Manager de Contacte
 * =============================================================================
 *
 * ATENȚIE: Acest fișier conține soluția completă!
 * Destinat DOAR instructorilor pentru verificare și evaluare.
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 *
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_CONTACTE 1000
#define MAX_NUME 50
#define MAX_TELEFON 20
#define MAX_EMAIL 100
#define MAX_CATEGORIE 30
#define MAX_LINE 512
#define CONTACTS_FILE "contacts.csv"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

typedef struct {
    int id;
    char nume[MAX_NUME];
    char prenume[MAX_NUME];
    char telefon[MAX_TELEFON];
    char email[MAX_EMAIL];
    char categorie[MAX_CATEGORIE];
    char data_adaugare[20];
} Contact;

typedef struct {
    Contact contacte[MAX_CONTACTE];
    int count;
    int next_id;
} ContactManager;

/* =============================================================================
 * FUNCȚII UTILITARE
 * =============================================================================
 */

/**
 * Elimină spațiile de la început și sfârșit
 */
char *trim(char *str) {
    if (str == NULL) return NULL;
    
    while (isspace((unsigned char)*str)) str++;
    if (*str == '\0') return str;
    
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    
    return str;
}

/**
 * Verifică formatul numărului de telefon (07XXXXXXXX)
 */
int valideaza_telefon(const char *telefon) {
    if (strlen(telefon) != 10) return 0;
    if (telefon[0] != '0') return 0;
    
    for (int i = 0; i < 10; i++) {
        if (!isdigit((unsigned char)telefon[i])) return 0;
    }
    
    return 1;
}

/**
 * Verifică formatul email-ului (simplificat)
 */
int valideaza_email(const char *email) {
    const char *at = strchr(email, '@');
    if (at == NULL) return 0;
    
    /* Trebuie să fie caractere înainte de @ */
    if (at == email) return 0;
    
    /* Trebuie să existe punct după @ */
    const char *dot = strchr(at, '.');
    if (dot == NULL || dot == at + 1) return 0;
    
    /* Trebuie să fie caractere după punct */
    if (strlen(dot) < 2) return 0;
    
    return 1;
}

/**
 * Obține data curentă în format YYYY-MM-DD
 */
void get_current_date(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d", tm_info);
}

/**
 * Verifică dacă un string conține alt string (case-insensitive)
 */
int contine_text(const char *haystack, const char *needle) {
    if (needle == NULL || *needle == '\0') return 1;
    
    char h_lower[MAX_LINE], n_lower[MAX_LINE];
    
    /* Copiem și convertim la lowercase */
    strncpy(h_lower, haystack, MAX_LINE - 1);
    h_lower[MAX_LINE - 1] = '\0';
    strncpy(n_lower, needle, MAX_LINE - 1);
    n_lower[MAX_LINE - 1] = '\0';
    
    for (char *p = h_lower; *p; p++) *p = tolower((unsigned char)*p);
    for (char *p = n_lower; *p; p++) *p = tolower((unsigned char)*p);
    
    return strstr(h_lower, n_lower) != NULL;
}

/* =============================================================================
 * FUNCȚII CRUD
 * =============================================================================
 */

/**
 * Inițializează manager-ul de contacte
 */
void init_manager(ContactManager *manager) {
    manager->count = 0;
    manager->next_id = 1;
}

/**
 * Găsește următorul ID disponibil
 */
int get_next_id(ContactManager *manager) {
    int max_id = 0;
    for (int i = 0; i < manager->count; i++) {
        if (manager->contacte[i].id > max_id) {
            max_id = manager->contacte[i].id;
        }
    }
    manager->next_id = max_id + 1;
    return manager->next_id;
}

/**
 * Adaugă un contact nou
 */
int adauga_contact(ContactManager *manager, const char *nume, const char *prenume,
                   const char *telefon, const char *email, const char *categorie) {
    if (manager->count >= MAX_CONTACTE) {
        fprintf(stderr, "Eroare: Limita de contacte atinsă!\n");
        return -1;
    }
    
    /* Validări */
    if (!valideaza_telefon(telefon)) {
        fprintf(stderr, "Eroare: Format telefon invalid! (format: 07XXXXXXXX)\n");
        return -1;
    }
    
    if (strlen(email) > 0 && !valideaza_email(email)) {
        fprintf(stderr, "Eroare: Format email invalid!\n");
        return -1;
    }
    
    /* Verificare duplicat (după telefon) */
    for (int i = 0; i < manager->count; i++) {
        if (strcmp(manager->contacte[i].telefon, telefon) == 0) {
            fprintf(stderr, "Eroare: Numărul de telefon există deja!\n");
            return -1;
        }
    }
    
    /* Adăugare contact */
    Contact *c = &manager->contacte[manager->count];
    c->id = get_next_id(manager);
    strncpy(c->nume, nume, MAX_NUME - 1);
    c->nume[MAX_NUME - 1] = '\0';
    strncpy(c->prenume, prenume, MAX_NUME - 1);
    c->prenume[MAX_NUME - 1] = '\0';
    strncpy(c->telefon, telefon, MAX_TELEFON - 1);
    c->telefon[MAX_TELEFON - 1] = '\0';
    strncpy(c->email, email, MAX_EMAIL - 1);
    c->email[MAX_EMAIL - 1] = '\0';
    strncpy(c->categorie, categorie, MAX_CATEGORIE - 1);
    c->categorie[MAX_CATEGORIE - 1] = '\0';
    get_current_date(c->data_adaugare, sizeof(c->data_adaugare));
    
    manager->count++;
    manager->next_id++;
    
    return c->id;
}

/**
 * Găsește un contact după ID
 */
Contact *gaseste_contact_id(ContactManager *manager, int id) {
    for (int i = 0; i < manager->count; i++) {
        if (manager->contacte[i].id == id) {
            return &manager->contacte[i];
        }
    }
    return NULL;
}

/**
 * Actualizează un contact
 */
int actualizeaza_contact(ContactManager *manager, int id, const char *nume,
                        const char *prenume, const char *telefon, 
                        const char *email, const char *categorie) {
    Contact *c = gaseste_contact_id(manager, id);
    if (c == NULL) {
        fprintf(stderr, "Eroare: Contact cu ID %d nu există!\n", id);
        return -1;
    }
    
    /* Validări */
    if (strlen(telefon) > 0 && !valideaza_telefon(telefon)) {
        fprintf(stderr, "Eroare: Format telefon invalid!\n");
        return -1;
    }
    
    if (strlen(email) > 0 && !valideaza_email(email)) {
        fprintf(stderr, "Eroare: Format email invalid!\n");
        return -1;
    }
    
    /* Actualizare (doar câmpurile ne-goale) */
    if (strlen(nume) > 0) {
        strncpy(c->nume, nume, MAX_NUME - 1);
        c->nume[MAX_NUME - 1] = '\0';
    }
    if (strlen(prenume) > 0) {
        strncpy(c->prenume, prenume, MAX_NUME - 1);
        c->prenume[MAX_NUME - 1] = '\0';
    }
    if (strlen(telefon) > 0) {
        strncpy(c->telefon, telefon, MAX_TELEFON - 1);
        c->telefon[MAX_TELEFON - 1] = '\0';
    }
    if (strlen(email) > 0) {
        strncpy(c->email, email, MAX_EMAIL - 1);
        c->email[MAX_EMAIL - 1] = '\0';
    }
    if (strlen(categorie) > 0) {
        strncpy(c->categorie, categorie, MAX_CATEGORIE - 1);
        c->categorie[MAX_CATEGORIE - 1] = '\0';
    }
    
    return 0;
}

/**
 * Șterge un contact
 */
int sterge_contact(ContactManager *manager, int id) {
    int found = -1;
    for (int i = 0; i < manager->count; i++) {
        if (manager->contacte[i].id == id) {
            found = i;
            break;
        }
    }
    
    if (found < 0) {
        fprintf(stderr, "Eroare: Contact cu ID %d nu există!\n", id);
        return -1;
    }
    
    /* Mutăm ultimul element în locul celui șters */
    if (found < manager->count - 1) {
        manager->contacte[found] = manager->contacte[manager->count - 1];
    }
    manager->count--;
    
    return 0;
}

/* =============================================================================
 * FUNCȚII DE CĂUTARE ȘI FILTRARE
 * =============================================================================
 */

/**
 * Caută contacte după nume (parțial, case-insensitive)
 */
void cauta_dupa_nume(const ContactManager *manager, const char *query) {
    printf("\n📋 Rezultate căutare pentru '%s':\n", query);
    printf("───────────────────────────────────────────────────────────────\n");
    
    int found = 0;
    for (int i = 0; i < manager->count; i++) {
        const Contact *c = &manager->contacte[i];
        if (contine_text(c->nume, query) || contine_text(c->prenume, query)) {
            printf("  ID: %d | %s %s | %s | %s\n", 
                   c->id, c->nume, c->prenume, c->telefon, c->categorie);
            found++;
        }
    }
    
    if (found == 0) {
        printf("  Nu s-au găsit contacte.\n");
    } else {
        printf("───────────────────────────────────────────────────────────────\n");
        printf("  Total: %d contact(e) găsite\n", found);
    }
}

/**
 * Filtrează contacte după categorie
 */
void filtreaza_dupa_categorie(const ContactManager *manager, const char *categorie) {
    printf("\n📋 Contacte din categoria '%s':\n", categorie);
    printf("───────────────────────────────────────────────────────────────\n");
    
    int found = 0;
    for (int i = 0; i < manager->count; i++) {
        const Contact *c = &manager->contacte[i];
        if (strcasecmp(c->categorie, categorie) == 0) {
            printf("  ID: %d | %s %s | %s | %s\n", 
                   c->id, c->nume, c->prenume, c->telefon, c->email);
            found++;
        }
    }
    
    if (found == 0) {
        printf("  Nu s-au găsit contacte în această categorie.\n");
    } else {
        printf("───────────────────────────────────────────────────────────────\n");
        printf("  Total: %d contact(e)\n", found);
    }
}

/* =============================================================================
 * FUNCȚII DE IMPORT/EXPORT
 * =============================================================================
 */

/**
 * Salvează contactele în fișier CSV
 */
int salveaza_csv(const ContactManager *manager, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Eroare la salvare: %s\n", strerror(errno));
        return -1;
    }
    
    /* Header */
    fprintf(fp, "ID,Nume,Prenume,Telefon,Email,Categorie,DataAdaugare\n");
    
    /* Date */
    for (int i = 0; i < manager->count; i++) {
        const Contact *c = &manager->contacte[i];
        fprintf(fp, "%d,%s,%s,%s,%s,%s,%s\n",
                c->id, c->nume, c->prenume, c->telefon,
                c->email, c->categorie, c->data_adaugare);
    }
    
    fclose(fp);
    return 0;
}

/**
 * Încarcă contactele din fișier CSV
 */
int incarca_csv(ContactManager *manager, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        /* Fișierul nu există - nu e eroare, doar nu avem date */
        return 0;
    }
    
    char line[MAX_LINE];
    int line_num = 0;
    
    /* Sărim header-ul */
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return 0;
    }
    
    while (fgets(line, sizeof(line), fp) != NULL && manager->count < MAX_CONTACTE) {
        line_num++;
        line[strcspn(line, "\r\n")] = '\0';
        
        Contact *c = &manager->contacte[manager->count];
        
        int items = sscanf(line, "%d,%49[^,],%49[^,],%19[^,],%99[^,],%29[^,],%19s",
                          &c->id, c->nume, c->prenume, c->telefon,
                          c->email, c->categorie, c->data_adaugare);
        
        if (items >= 6) {
            manager->count++;
            if (c->id >= manager->next_id) {
                manager->next_id = c->id + 1;
            }
        }
    }
    
    fclose(fp);
    return manager->count;
}

/**
 * Exportă în format vCard
 */
int exporta_vcard(const ContactManager *manager, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Eroare la export vCard: %s\n", strerror(errno));
        return -1;
    }
    
    for (int i = 0; i < manager->count; i++) {
        const Contact *c = &manager->contacte[i];
        fprintf(fp, "BEGIN:VCARD\n");
        fprintf(fp, "VERSION:3.0\n");
        fprintf(fp, "N:%s;%s;;;\n", c->nume, c->prenume);
        fprintf(fp, "FN:%s %s\n", c->prenume, c->nume);
        fprintf(fp, "TEL;TYPE=CELL:%s\n", c->telefon);
        if (strlen(c->email) > 0) {
            fprintf(fp, "EMAIL:%s\n", c->email);
        }
        fprintf(fp, "CATEGORIES:%s\n", c->categorie);
        fprintf(fp, "END:VCARD\n\n");
    }
    
    fclose(fp);
    return 0;
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

/**
 * Afișează toate contactele
 */
void afiseaza_contacte(const ContactManager *manager) {
    if (manager->count == 0) {
        printf("\n  📭 Nu există contacte.\n");
        return;
    }
    
    printf("\n");
    printf("┌─────┬──────────────────────┬────────────┬──────────────────────────┬────────────┐\n");
    printf("│ ID  │ Nume                 │ Telefon    │ Email                    │ Categorie  │\n");
    printf("├─────┼──────────────────────┼────────────┼──────────────────────────┼────────────┤\n");
    
    for (int i = 0; i < manager->count; i++) {
        const Contact *c = &manager->contacte[i];
        char nume_complet[MAX_NUME * 2];
        snprintf(nume_complet, sizeof(nume_complet), "%s %s", c->prenume, c->nume);
        
        printf("│ %3d │ %-20s │ %-10s │ %-24s │ %-10s │\n",
               c->id, nume_complet, c->telefon, c->email, c->categorie);
    }
    
    printf("└─────┴──────────────────────┴────────────┴──────────────────────────┴────────────┘\n");
    printf("  Total: %d contact(e)\n", manager->count);
}

/**
 * Afișează meniul principal
 */
void afiseaza_meniu(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════╗\n");
    printf("║       MANAGER DE CONTACTE             ║\n");
    printf("╠═══════════════════════════════════════╣\n");
    printf("║  1. Afișare toate contactele          ║\n");
    printf("║  2. Adaugă contact nou                ║\n");
    printf("║  3. Caută contact                     ║\n");
    printf("║  4. Editează contact                  ║\n");
    printf("║  5. Șterge contact                    ║\n");
    printf("║  6. Filtrează după categorie          ║\n");
    printf("║  7. Export contacte (vCard)           ║\n");
    printf("║  8. Salvează și ieșire                ║\n");
    printf("║  0. Ieșire fără salvare               ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    printf("Alegere: ");
}

/**
 * Citește o linie de la utilizator
 */
void citeste_linie(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) != NULL) {
        buffer[strcspn(buffer, "\r\n")] = '\0';
    }
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(void) {
    ContactManager manager;
    init_manager(&manager);
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║        SOLUȚIE TEMA 2: Manager de Contacte                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Încărcăm contactele existente */
    int loaded = incarca_csv(&manager, CONTACTS_FILE);
    if (loaded > 0) {
        printf("\n✓ %d contacte încărcate din '%s'\n", loaded, CONTACTS_FILE);
    }
    
    /* Adăugăm câteva contacte de test dacă nu există */
    if (manager.count == 0) {
        printf("\n📝 Adăugare contacte de test...\n");
        adauga_contact(&manager, "Popescu", "Ion", "0721123456", 
                      "ion.popescu@email.ro", "Familie");
        adauga_contact(&manager, "Ionescu", "Maria", "0731234567", 
                      "maria.ionescu@email.ro", "Prieteni");
        adauga_contact(&manager, "Georgescu", "Andrei", "0741345678", 
                      "andrei.g@email.ro", "Muncă");
        printf("✓ 3 contacte de test adăugate\n");
    }
    
    char input[MAX_LINE];
    int running = 1;
    
    while (running) {
        afiseaza_meniu();
        citeste_linie("", input, sizeof(input));
        
        int choice = atoi(input);
        
        switch (choice) {
            case 1:  /* Afișare contacte */
                afiseaza_contacte(&manager);
                break;
                
            case 2: {  /* Adaugă contact */
                char nume[MAX_NUME], prenume[MAX_NUME];
                char telefon[MAX_TELEFON], email[MAX_EMAIL], categorie[MAX_CATEGORIE];
                
                printf("\n📝 ADAUGĂ CONTACT NOU\n");
                printf("───────────────────────────────────────\n");
                citeste_linie("  Nume: ", nume, sizeof(nume));
                citeste_linie("  Prenume: ", prenume, sizeof(prenume));
                citeste_linie("  Telefon (07XXXXXXXX): ", telefon, sizeof(telefon));
                citeste_linie("  Email: ", email, sizeof(email));
                citeste_linie("  Categorie: ", categorie, sizeof(categorie));
                
                int id = adauga_contact(&manager, nume, prenume, telefon, email, categorie);
                if (id > 0) {
                    printf("\n✓ Contact adăugat cu ID: %d\n", id);
                }
                break;
            }
            
            case 3: {  /* Caută contact */
                char query[MAX_LINE];
                citeste_linie("\n🔍 Caută nume: ", query, sizeof(query));
                cauta_dupa_nume(&manager, query);
                break;
            }
            
            case 4: {  /* Editează contact */
                int id;
                citeste_linie("\n✏️  ID contact de editat: ", input, sizeof(input));
                id = atoi(input);
                
                Contact *c = gaseste_contact_id(&manager, id);
                if (c == NULL) {
                    printf("❌ Contact negăsit!\n");
                    break;
                }
                
                printf("\nContact curent: %s %s | %s\n", c->prenume, c->nume, c->telefon);
                printf("(Lasă gol pentru a păstra valoarea curentă)\n");
                
                char nume[MAX_NUME], prenume[MAX_NUME];
                char telefon[MAX_TELEFON], email[MAX_EMAIL], categorie[MAX_CATEGORIE];
                
                citeste_linie("  Nume nou: ", nume, sizeof(nume));
                citeste_linie("  Prenume nou: ", prenume, sizeof(prenume));
                citeste_linie("  Telefon nou: ", telefon, sizeof(telefon));
                citeste_linie("  Email nou: ", email, sizeof(email));
                citeste_linie("  Categorie nouă: ", categorie, sizeof(categorie));
                
                if (actualizeaza_contact(&manager, id, nume, prenume, telefon, email, categorie) == 0) {
                    printf("\n✓ Contact actualizat!\n");
                }
                break;
            }
            
            case 5: {  /* Șterge contact */
                int id;
                citeste_linie("\n🗑️  ID contact de șters: ", input, sizeof(input));
                id = atoi(input);
                
                citeste_linie("Confirmă ștergerea? (da/nu): ", input, sizeof(input));
                if (strcasecmp(input, "da") == 0) {
                    if (sterge_contact(&manager, id) == 0) {
                        printf("✓ Contact șters!\n");
                    }
                } else {
                    printf("Ștergere anulată.\n");
                }
                break;
            }
            
            case 6: {  /* Filtrare categorie */
                char categorie[MAX_CATEGORIE];
                citeste_linie("\n📁 Categorie: ", categorie, sizeof(categorie));
                filtreaza_dupa_categorie(&manager, categorie);
                break;
            }
            
            case 7: {  /* Export vCard */
                const char *vcf_file = "contacte_export.vcf";
                if (exporta_vcard(&manager, vcf_file) == 0) {
                    printf("\n✓ Contacte exportate în '%s'\n", vcf_file);
                }
                break;
            }
            
            case 8:  /* Salvare și ieșire */
                if (salveaza_csv(&manager, CONTACTS_FILE) == 0) {
                    printf("\n✓ Contacte salvate în '%s'\n", CONTACTS_FILE);
                }
                running = 0;
                break;
                
            case 0:  /* Ieșire */
                running = 0;
                break;
                
            default:
                printf("❌ Opțiune invalidă!\n");
        }
    }
    
    printf("\n👋 La revedere!\n");
    
    return EXIT_SUCCESS;
}
