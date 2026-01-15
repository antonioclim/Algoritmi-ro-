/**
 * =============================================================================
 * EXERCIȚIUL 2: Playlist Muzical (Lista Dublu Înlănțuită)
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unei liste dublu înlănțuite pentru simularea unui
 *   playlist muzical cu navigare bidirecțională (înainte/înapoi).
 *
 * CERINȚE:
 *   1. Completați toate funcțiile marcate cu TODO
 *   2. Implementați navigarea în ambele direcții
 *   3. Verificați cu Valgrind că nu aveți memory leaks
 *   4. Programul trebuie să compileze fără warning-uri
 *
 * STRUCTURA MELODIE:
 *   - titlu: șir de caractere (max 100)
 *   - artist: șir de caractere (max 50)
 *   - durata_secunde: durata în secunde (int)
 *
 * EXEMPLU INPUT (data/playlist.txt):
 *   Bohemian_Rhapsody Queen 354
 *   Stairway_to_Heaven Led_Zeppelin 482
 *   Hotel_California Eagles 391
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -g -o exercise2 exercise2.c
 * UTILIZARE: ./exercise2 < data/playlist.txt
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* =============================================================================
 * DEFINIREA STRUCTURILOR
 * =============================================================================
 */

#define MAX_TITLU 100
#define MAX_ARTIST 50

/**
 * TODO 1: Definiți structura MelodieNode pentru lista dublu înlănțuită
 *
 * Structura trebuie să conțină:
 *   - titlu: char[MAX_TITLU] - titlul melodiei
 *   - artist: char[MAX_ARTIST] - numele artistului
 *   - durata_secunde: int - durata în secunde
 *   - prev: pointer către nodul anterior (MelodieNode)
 *   - next: pointer către nodul următor (MelodieNode)
 *
 * Hint: Pentru liste dublu înlănțuite avem nevoie de AMBII pointeri!
 */

/* CODUL TĂU AICI - Definiția structurii MelodieNode */
typedef struct MelodieNode {
    char titlu[MAX_TITLU];
    char artist[MAX_ARTIST];
    int durata_secunde;
    struct MelodieNode *prev;
    struct MelodieNode *next;
} MelodieNode;


/**
 * Structura pentru playlist-ul complet.
 * Conține pointeri către head, tail și melodia curentă.
 * (Această structură este deja definită)
 */
typedef struct {
    MelodieNode *head;      /* Prima melodie din playlist */
    MelodieNode *tail;      /* Ultima melodie din playlist */
    MelodieNode *current;   /* Melodia care se redă acum */
    int size;               /* Numărul total de melodii */
} Playlist;

/* =============================================================================
 * FUNCȚII DE INIȚIALIZARE ȘI CREARE
 * =============================================================================
 */

/**
 * Inițializează un playlist gol.
 * (Această funcție este deja implementată)
 */
void playlist_init(Playlist *pl) {
    pl->head = NULL;
    pl->tail = NULL;
    pl->current = NULL;
    pl->size = 0;
}

/**
 * TODO 2: Implementați funcția create_melodie
 *
 * Creează un nou nod MelodieNode cu datele specificate.
 *
 * @param titlu Titlul melodiei
 * @param artist Numele artistului
 * @param durata Durata în secunde
 * @return Pointer către noul nod sau NULL dacă alocarea eșuează
 *
 * Pași:
 *   1. Alocați memorie cu malloc()
 *   2. Verificați dacă alocarea a reușit
 *   3. Copiați datele (folosiți strncpy pentru siguranță)
 *   4. Setați AMBII pointeri (prev și next) la NULL
 *   5. Returnați noul nod
 */
MelodieNode *create_melodie(const char *titlu, const char *artist, int durata) {
    /* CODUL TĂU AICI */
    
    return NULL;  /* Înlocuiește cu implementarea ta */
}

/**
 * TODO 3: Implementați funcția playlist_free
 *
 * Eliberează toată memoria playlist-ului.
 *
 * @param pl Pointer către playlist
 *
 * Pași:
 *   1. Parcurgeți lista de la head la tail
 *   2. Eliberați fiecare nod
 *   3. Resetați toți pointerii la NULL și size la 0
 */
void playlist_free(Playlist *pl) {
    /* CODUL TĂU AICI */
    
}

/* =============================================================================
 * FUNCȚII DE INSERARE
 * =============================================================================
 */

/**
 * TODO 4: Implementați funcția playlist_add_end
 *
 * Adaugă o melodie la sfârșitul playlist-ului.
 * Complexitate: O(1) datorită pointer-ului tail!
 *
 * @param pl Pointer către playlist
 * @param titlu Titlul melodiei
 * @param artist Numele artistului
 * @param durata Durata în secunde
 *
 * Pași:
 *   1. Creați nodul nou
 *   2. Dacă playlist-ul e gol: head = tail = current = noul nod
 *   3. Altfel: 
 *      - new_node->prev = tail
 *      - tail->next = new_node
 *      - tail = new_node
 *   4. Incrementați size
 *
 * ATENȚIE: Actualizați AMBELE legături (prev ȘI next)!
 */
void playlist_add_end(Playlist *pl, const char *titlu, const char *artist, int durata) {
    /* CODUL TĂU AICI */
    
}

/**
 * TODO 5: Implementați funcția playlist_add_sorted
 *
 * Adaugă o melodie în playlist păstrând ordinea alfabetică după titlu.
 *
 * @param pl Pointer către playlist
 * @param titlu Titlul melodiei
 * @param artist Numele artistului
 * @param durata Durata în secunde
 *
 * Pași:
 *   1. Creați nodul nou
 *   2. Cazuri speciale: playlist gol, inserare la început
 *   3. Găsiți poziția corectă (strcmp pentru comparare)
 *   4. Inserați nodul actualizând TOATE legăturile
 *   5. Incrementați size
 *
 * Hint: strcmp(a, b) < 0 dacă a vine înainte de b alfabetic
 */
void playlist_add_sorted(Playlist *pl, const char *titlu, const char *artist, int durata) {
    /* CODUL TĂU AICI */
    
}

/* =============================================================================
 * FUNCȚII DE NAVIGARE
 * =============================================================================
 */

/**
 * TODO 6: Implementați funcția playlist_next
 *
 * Trece la următoarea melodie din playlist.
 *
 * @param pl Pointer către playlist
 * @return Pointer către noua melodie curentă sau NULL
 *
 * Comportament:
 *   - Dacă există next, current devine current->next
 *   - Dacă suntem la final (current->next == NULL), rămânem pe loc
 *   - Returnăm melodia curentă (sau NULL dacă playlist-ul e gol)
 */
MelodieNode *playlist_next(Playlist *pl) {
    /* CODUL TĂU AICI */
    
    return NULL;  /* Înlocuiește cu implementarea ta */
}

/**
 * TODO 7: Implementați funcția playlist_prev
 *
 * Trece la melodia anterioară din playlist.
 *
 * @param pl Pointer către playlist
 * @return Pointer către noua melodie curentă sau NULL
 *
 * Comportament:
 *   - Dacă există prev, current devine current->prev
 *   - Dacă suntem la început (current->prev == NULL), rămânem pe loc
 *   - Returnăm melodia curentă
 */
MelodieNode *playlist_prev(Playlist *pl) {
    /* CODUL TĂU AICI */
    
    return NULL;  /* Înlocuiește cu implementarea ta */
}

/**
 * TODO 8: Implementați funcția playlist_go_to_start
 *
 * Sare la prima melodie din playlist.
 *
 * @param pl Pointer către playlist
 * @return Pointer către prima melodie sau NULL dacă playlist-ul e gol
 */
MelodieNode *playlist_go_to_start(Playlist *pl) {
    /* CODUL TĂU AICI */
    
    return NULL;  /* Înlocuiește cu implementarea ta */
}

/**
 * TODO 9: Implementați funcția playlist_go_to_end
 *
 * Sare la ultima melodie din playlist.
 *
 * @param pl Pointer către playlist
 * @return Pointer către ultima melodie sau NULL dacă playlist-ul e gol
 */
MelodieNode *playlist_go_to_end(Playlist *pl) {
    /* CODUL TĂU AICI */
    
    return NULL;  /* Înlocuiește cu implementarea ta */
}

/* =============================================================================
 * FUNCȚII DE ȘTERGERE
 * =============================================================================
 */

/**
 * TODO 10: Implementați funcția playlist_remove_current
 *
 * Șterge melodia curentă din playlist.
 * După ștergere, current devine următoarea melodie (sau precedenta dacă
 * am șters ultima).
 *
 * @param pl Pointer către playlist
 * @return 1 dacă ștergerea a reușit, 0 dacă playlist-ul era gol
 *
 * Pași:
 *   1. Verificați dacă există o melodie curentă
 *   2. Salvați pointer-ul către nodul de șters
 *   3. Actualizați legăturile:
 *      - Dacă există prev: prev->next = current->next
 *      - Altfel: head = current->next
 *      - Dacă există next: next->prev = current->prev
 *      - Altfel: tail = current->prev
 *   4. Actualizați current (next dacă există, altfel prev)
 *   5. Eliberați memoria și decrementați size
 *
 * ATENȚIE: Multe cazuri speciale! Testați cu 1, 2 și mai multe melodii.
 */
int playlist_remove_current(Playlist *pl) {
    /* CODUL TĂU AICI */
    
    return 0;  /* Înlocuiește cu implementarea ta */
}

/* =============================================================================
 * FUNCȚII DE CĂUTARE ȘI STATISTICI
 * =============================================================================
 */

/**
 * Caută o melodie după artist.
 * (Această funcție este deja implementată)
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

/**
 * Calculează durata totală a playlist-ului în secunde.
 * (Această funcție este deja implementată)
 */
int playlist_total_duration(Playlist *pl) {
    int total = 0;
    MelodieNode *current = pl->head;
    
    while (current != NULL) {
        total += current->durata_secunde;
        current = current->next;
    }
    
    return total;
}

/**
 * Formatează durata în format MM:SS.
 * (Această funcție este deja implementată)
 */
void format_duration(int seconds, char *buffer, int buffer_size) {
    int minutes = seconds / 60;
    int secs = seconds % 60;
    snprintf(buffer, buffer_size, "%d:%02d", minutes, secs);
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

/**
 * Afișează întregul playlist.
 * (Această funcție este deja implementată)
 */
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
        
        /* Marchează melodia curentă */
        const char *marker = (node == pl->current) ? "▶" : " ";
        
        printf("║ %s %2d. %-25s %-20s %6s ║\n",
               marker, index, node->titlu, node->artist, duration_str);
        
        node = node->next;
        index++;
    }
    
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    
    /* Statistici */
    int total = playlist_total_duration(pl);
    format_duration(total, duration_str, sizeof(duration_str));
    printf("║ Total: %d melodii                          Durată: %8s ║\n",
           pl->size, duration_str);
    printf("╚════════════════════════════════════════════════════════════════╝\n");
}

/**
 * Afișează melodia curentă.
 * (Această funcție este deja implementată)
 */
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
    printf("║     EXERCIȚIUL 2: Playlist Muzical (Lista Dublu Înlănțuită)   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Citim melodiile din input */
    printf("Citire melodii (format: Titlu Artist Durata_secunde)...\n\n");
    
    while (scanf("%99s %49s %d", titlu, artist, &durata) == 3) {
        playlist_add_end(&pl, titlu, artist, durata);
    }
    
    /* Afișăm playlist-ul */
    playlist_print(&pl);
    
    /* Testăm navigarea */
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
    
    /* Testăm ștergerea */
    printf("\n═══ Testare Ștergere ═══\n\n");
    
    printf("→ Ștergem melodia curentă (ultima):\n");
    playlist_remove_current(&pl);
    playlist_print(&pl);
    
    printf("\n→ Go to start și ștergem prima melodie:\n");
    playlist_go_to_start(&pl);
    playlist_remove_current(&pl);
    playlist_print(&pl);
    
    /* Eliberăm memoria */
    playlist_free(&pl);
    printf("\n✓ Memoria a fost eliberată.\n");
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Implementați funcția playlist_shuffle() care amestecă aleatoriu
 *    ordinea melodiilor (Fisher-Yates shuffle adaptat pentru liste).
 *
 * 2. Implementați funcția playlist_reverse() care inversează playlist-ul.
 *
 * 3. Implementați funcția playlist_remove_duplicates() care elimină
 *    melodiile duplicate (după titlu).
 *
 * 4. Implementați funcția playlist_save() care salvează playlist-ul
 *    într-un fișier.
 *
 * 5. Adăugați suport pentru mod "repeat" (când ajunge la final,
 *    să continue de la început) și "repeat one" (repetă aceeași melodie).
 *
 * =============================================================================
 */
