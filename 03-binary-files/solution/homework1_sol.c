/**
 * SOLUȚIE TEMA 1: Sistem Gestiune Produse
 * ATENȚIE: Destinat DOAR instructorilor!
 * Compilare: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define MAGIC 0x444F5250  /* "PROD" */
#define FILENAME "inventar.bin"

#pragma pack(push, 1)
typedef struct {
    uint32_t magic, version, record_count;
    uint64_t created_at, modified_at;
    uint8_t reserved[32];
} Header;

typedef struct {
    uint32_t cod;
    char denumire[60];
    char categorie[30];
    float pret;
    uint32_t stoc;
    uint8_t activ;
} Produs;
#pragma pack(pop)

int create_db(const char *fn) {
    FILE *f = fopen(fn, "wb");
    if (!f) return -1;
    Header h = {MAGIC, 1, 0, time(NULL), time(NULL)};
    fwrite(&h, sizeof(h), 1, f);
    fclose(f);
    return 0;
}

int add_product(const char *fn, Produs *p) {
    FILE *f = fopen(fn, "rb+");
    if (!f) { create_db(fn); f = fopen(fn, "rb+"); }
    Header h;
    fread(&h, sizeof(h), 1, f);
    fseek(f, 0, SEEK_END);
    p->activ = 1;
    fwrite(p, sizeof(Produs), 1, f);
    h.record_count++;
    h.modified_at = time(NULL);
    fseek(f, 0, SEEK_SET);
    fwrite(&h, sizeof(h), 1, f);
    fclose(f);
    return 0;
}

void list_products(const char *fn) {
    FILE *f = fopen(fn, "rb");
    if (!f) { printf("Fișier inexistent\n"); return; }
    Header h;
    fread(&h, sizeof(h), 1, f);
    printf("\n%-10s %-40s %-15s %10s %8s\n", "COD", "DENUMIRE", "CATEGORIE", "PREȚ", "STOC");
    printf("─────────────────────────────────────────────────────────────────────────────────\n");
    Produs p;
    float total = 0;
    for (uint32_t i = 0; i < h.record_count; i++) {
        fread(&p, sizeof(p), 1, f);
        if (p.activ) {
            printf("%-10u %-40s %-15s %10.2f %8u\n", 
                   p.cod, p.denumire, p.categorie, p.pret, p.stoc);
            total += p.pret * p.stoc;
        }
    }
    printf("─────────────────────────────────────────────────────────────────────────────────\n");
    printf("Valoare totală inventar: %.2f RON\n", total);
    fclose(f);
}

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║          SISTEM GESTIUNE INVENTAR - SOLUȚIE                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Demo: adăugare câteva produse */
    create_db(FILENAME);
    
    Produs produse[] = {
        {100001, "Laptop ASUS ROG", "Electronice", 4500.00, 15, 1},
        {100002, "Mouse Logitech MX", "Periferice", 350.00, 50, 1},
        {100003, "Monitor Dell 27\"", "Electronice", 1200.00, 20, 1},
        {100004, "Tastatură Mecanică", "Periferice", 450.00, 30, 1},
        {100005, "SSD Samsung 1TB", "Stocare", 550.00, 100, 1}
    };
    
    for (int i = 0; i < 5; i++) {
        add_product(FILENAME, &produse[i]);
        printf("Adăugat: %s\n", produse[i].denumire);
    }
    
    list_products(FILENAME);
    
    return 0;
}
