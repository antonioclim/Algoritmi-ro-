/**
 * =============================================================================
 * SOLUȚIE - EXERCIȚIUL 2: Playlist Muzical (Lista Dublu Înlănțuită)
 * =============================================================================
 *
 * ATENȚIE: Acest fișier conține soluția completă.
 * Pentru instructori și pentru auto-verificare.
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLU 100
#define MAX_ARTIST 50

/* =============================================================================
 * DEFINIȚIA STRUCTURILOR
 * =============================================================================
 */

typedef struct MelodieNode {
    char titlu[MAX_TITLU];
    char artist[MAX_ARTIST];
    int durata_secunde;
    struct MelodieNode *prev;
    struct MelodieNode *next;
} MelodieNode;

typedef struct {
    MelodieNode *head;
    MelodieNode *tail;
    MelodieNode *current;
    int size;
} Playlist;

/* =============================================================================
 * FUNCȚII DE INIȚIALIZARE
 * =============================================================================
 */

void playlist_init(Playlist *pl) {
    pl->head = NULL;
    pl->tail = NULL;
    pl->current = NULL;
    pl->size = 0;
}

/* =============================================================================
 * SOLUȚIA TODO 2: create_melodie
 * =============================================================================
 */

MelodieNode *create_melodie(const char *titlu, const char *artist, int durata) {
    MelodieNode *new_node = (MelodieNode *)malloc(sizeof(MelodieNode));
    
    if (new_node == NULL) {
        fprintf(stderr, "Eroare: malloc() a eșuat în create_melodie()!\n");
        return NULL;
    }
    
    /* Copiem datele cu siguranță */
    strncpy(new_node->titlu, titlu, MAX_TITLU - 1);
    new_node->titlu[MAX_TITLU - 1] = '\0';
    
    strncpy(new_node->artist, artist, MAX_ARTIST - 1);
    new_node->artist[MAX_ARTIST - 1] = '\0';
    
    new_node->durata_secunde = durata;
    
    /* IMPORTANT pentru lista dublu înlănțuită: ambii pointeri la NULL */
    new_node->prev = NULL;
    new_node->next = NULL;
    
    return new_node;
}

/* =============================================================================
 * SOLUȚIA TODO 3: playlist_free
 * =============================================================================
 */

void playlist_free(Playlist *pl) {
    MelodieNode *current = pl->head;
    MelodieNode *next_node;
    
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    
    /* Resetăm structura playlist-ului */
    pl->head = NULL;
    pl->tail = NULL;
    pl->current = NULL;
    pl->size = 0;
}

/* =============================================================================
 * SOLUȚIA TODO 4: playlist_add_end
 * =============================================================================
 */

void playlist_add_end(Playlist *pl, const char *titlu, const char *artist, int durata) {
    MelodieNode *new_node = create_melodie(titlu, artist, durata);
    if (new_node == NULL) return;
    
    if (pl->head == NULL) {
        /* Playlist-ul era gol - prima melodie */
        pl->head = new_node;
        pl->tail = new_node;
        pl->current = new_node;
    } else {
        /* Adăugăm la sfârșit */
        new_node->prev = pl->tail;  /* Noul nod arată înapoi către fostul tail */
        pl->tail->next = new_node;  /* Fostul tail arată înainte către noul nod */
        pl->tail = new_node;        /* Actualizăm tail-ul */
    }
    
    pl->size++;
}

/* =============================================================================
 * SOLUȚIA TODO 5: playlist_add_sorted
 * =============================================================================
 */

void playlist_add_sorted(Playlist *pl, const char *titlu, const char *artist, int durata) {
    MelodieNode *new_node = create_melodie(titlu, artist, durata);
    if (new_node == NULL) return;
    
    /* Caz 1: Playlist gol */
    if (pl->head == NULL) {
        pl->head = new_node;
        pl->tail = new_node;
        pl->current = new_node;
        pl->size++;
        return;
    }
    
    /* Caz 2: Inserare la început (titlu vine primul alfabetic) */
    if (strcmp(titlu, pl->head->titlu) < 0) {
        new_node->next = pl->head;
        pl->head->prev = new_node;
        pl->head = new_node;
        pl->size++;
        return;
    }
    
    /* Caz 3: Căutăm poziția corectă */
    MelodieNode *current = pl->head;
    while (current->next != NULL && strcmp(current->next->titlu, titlu) < 0) {
        current = current->next;
    }
    
    /* Inserăm după current */
    new_node->next = current->next;
    new_node->prev = current;
    
    if (current->next != NULL) {
        current->next->prev = new_node;
    } else {
        /* Inserăm la sfârșit */
        pl->tail = new_node;
    }
    
    current->next = new_node;
    pl->size++;
}

/* =============================================================================
 * SOLUȚIA TODO 6: playlist_next
 * =============================================================================
 */

MelodieNode *playlist_next(Playlist *pl) {
    if (pl->current == NULL) {
        return NULL;
    }
    
    if (pl->current->next != NULL) {
        pl->current = pl->current->next;
    }
    /* Dacă suntem la final, rămânem pe loc */
    
    return pl->current;
}

/* =============================================================================
 * SOLUȚIA TODO 7: playlist_prev
 * =============================================================================
 */

MelodieNode *playlist_prev(Playlist *pl) {
    if (pl->current == NULL) {
        return NULL;
    }
    
    if (pl->current->prev != NULL) {
        pl->current = pl->current->prev;
    }
    /* Dacă suntem la început, rămânem pe loc */
    
    return pl->current;
}

/* =============================================================================
 * SOLUȚIA TODO 8: playlist_go_to_start
 * =============================================================================
 */

MelodieNode *playlist_go_to_start(Playlist *pl) {
    if (pl->head == NULL) {
        return NULL;
    }
    
    pl->current = pl->head;
    return pl->current;
}

/* =============================================================================
 * SOLUȚIA TODO 9: playlist_go_to_end
 * =============================================================================
 */

MelodieNode *playlist_go_to_end(Playlist *pl) {
    if (pl->tail == NULL) {
        return NULL;
    }
    
    pl->current = pl->tail;
    return pl->current;
}

/* =============================================================================
 * SOLUȚIA TODO 10: playlist_remove_current
 * =============================================================================
 */

int playlist_remove_current(Playlist *pl) {
    if (pl->current == NULL) {
        return 0;  /* Nu există melodie curentă */
    }
    
    MelodieNode *to_delete = pl->current;
    
    /* Actualizăm legăturile */
    if (to_delete->prev != NULL) {
        to_delete->prev->next = to_delete->next;
    } else {
        /* Ștergem head-ul */
        pl->head = to_delete->next;
    }
    
    if (to_delete->next != NULL) {
        to_delete->next->prev = to_delete->prev;
    } else {
        /* Ștergem tail-ul */
        pl->tail = to_delete->prev;
    }
    
    /* Actualizăm current */
    if (to_delete->next != NULL) {
        pl->current = to_delete->next;  /* Preferăm să mergem înainte */
    } else {
        pl->current = to_delete->prev;  /* Dacă era ultimul, mergem înapoi */
    }
    
    /* Eliberăm memoria și decrementăm size */
    free(to_delete);
    pl->size--;
    
    return 1;
}

/* =============================================================================
 * FUNCȚII AUXILIARE (NESCHIMBATE)
 * =============================================================================
 */

MelodieNode *playlist_find_by_artist(Playlist *pl, const char *artist) {
    MelodieNode *current = pl->head;
    
    while (current != NULL) {
        if (strcmp(current->artist, artist) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

int playlist_total_duration(Playlist *pl) {
    int total = 0;
    MelodieNode *current = pl->head;
    
    while (current != NULL) {
        total += current->durata_secunde;
        current = current->next;
    }
    
    return total;
}

void format_duration(int seconds, char *buffer, int buffer_size) {
    int minutes = seconds / 60;
    int secs = seconds % 60;
    snprintf(buffer, buffer_size, "%d:%02d", minutes, secs);
}

void playlist_print(Playlist *pl) {
    if (pl->head == NULL) {
        printf("Playlist-ul este gol.\n");
        return;
    }
    
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                        PLAYLIST                                ║\n");
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    
    MelodieNode *node = pl->head;
    int index = 1;
    char duration_str[16];
    
    while (node != NULL) {
        format_duration(node->durata_secunde, duration_str, sizeof(duration_str));
        
        const char *marker = (node == pl->current) ? "▶" : " ";
        
        printf("║ %s %2d. %-25s %-20s %6s ║\n",
               marker, index, node->titlu, node->artist, duration_str);
        
        node = node->next;
        index++;
    }
    
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    
    int total = playlist_total_duration(pl);
    format_duration(total, duration_str, sizeof(duration_str));
    printf("║ Total: %d melodii                          Durată: %8s ║\n",
           pl->size, duration_str);
    printf("╚════════════════════════════════════════════════════════════════╝\n");
}

void playlist_print_current(Playlist *pl) {
    if (pl->current == NULL) {
        printf("Nu există melodie curentă.\n");
        return;
    }
    
    char duration_str[16];
    format_duration(pl->current->durata_secunde, duration_str, sizeof(duration_str));
    
    printf("▶ Acum se redă: %s - %s [%s]\n",
           pl->current->artist, pl->current->titlu, duration_str);
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    Playlist pl;
    playlist_init(&pl);
    
    char titlu[MAX_TITLU];
    char artist[MAX_ARTIST];
    int durata;
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 2: Playlist - SOLUȚIE                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Citire melodii (format: Titlu Artist Durata_secunde)...\n\n");
    
    while (scanf("%99s %49s %d", titlu, artist, &durata) == 3) {
        playlist_add_end(&pl, titlu, artist, durata);
    }
    
    playlist_print(&pl);
    
    printf("\n═══ Testare Navigare ═══\n\n");
    
    printf("→ Go to start:\n  ");
    playlist_go_to_start(&pl);
    playlist_print_current(&pl);
    
    printf("\n→ Next:\n  ");
    playlist_next(&pl);
    playlist_print_current(&pl);
    
    printf("\n→ Next:\n  ");
    playlist_next(&pl);
    playlist_print_current(&pl);
    
    printf("\n→ Prev:\n  ");
    playlist_prev(&pl);
    playlist_print_current(&pl);
    
    printf("\n→ Go to end:\n  ");
    playlist_go_to_end(&pl);
    playlist_print_current(&pl);
    
    printf("\n═══ Testare Ștergere ═══\n\n");
    
    printf("→ Ștergem melodia curentă (ultima):\n");
    playlist_remove_current(&pl);
    playlist_print(&pl);
    
    printf("\n→ Go to start și ștergem prima melodie:\n");
    playlist_go_to_start(&pl);
    playlist_remove_current(&pl);
    playlist_print(&pl);
    
    playlist_free(&pl);
    printf("\n✓ Memoria a fost eliberată.\n");
    
    return 0;
}
