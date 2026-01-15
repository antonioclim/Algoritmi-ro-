/**
 * =============================================================================
 * SOLUȚIE TEMA 1: Procesator de Log-uri Apache/Nginx
 * =============================================================================
 *
 * ATENȚIE: Acest fișier conține soluția completă!
 * Destinat DOAR instructorilor pentru verificare și evaluare.
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_LINE 2048
#define MAX_IP 16
#define MAX_METHOD 10
#define MAX_URL 512
#define MAX_TIMESTAMP 64
#define MAX_ENTRIES 100000
#define TOP_N 10

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

typedef struct {
    char ip[MAX_IP];
    char timestamp[MAX_TIMESTAMP];
    char method[MAX_METHOD];
    char url[MAX_URL];
    int status;
    long bytes;
    int hour;  /* 0-23, extras din timestamp */
} LogEntry;

typedef struct {
    char ip[MAX_IP];
    int count;
} IPCount;

typedef struct {
    char url[MAX_URL];
    int count;
} URLCount;

typedef struct {
    int total_requests;
    int requests_per_hour[24];
    int status_2xx;
    int status_3xx;
    int status_4xx;
    int status_5xx;
    int get_count;
    int post_count;
    int put_count;
    int delete_count;
    int other_count;
    long total_bytes;
    IPCount top_ips[TOP_N];
    int top_ips_count;
    URLCount top_urls[TOP_N];
    int top_urls_count;
} Statistics;

/* =============================================================================
 * FUNCȚII DE PARSARE
 * =============================================================================
 */

/**
 * Extrage ora din timestamp format Apache
 * Format: [15/Jan/2024:10:30:45 +0200]
 */
int extrage_ora(const char *timestamp) {
    /* Căutăm ':' care separă ziua de oră */
    const char *p = strchr(timestamp, ':');
    if (p == NULL) return -1;
    
    int hour = 0;
    if (sscanf(p + 1, "%d", &hour) == 1) {
        return hour;
    }
    return -1;
}

/**
 * Parsează o linie de log Apache/Nginx
 * Format: IP - - [TIMESTAMP] "METHOD URL PROTOCOL" STATUS BYTES
 */
int parseaza_linie_log(const char *line, LogEntry *entry) {
    /* Pattern simplificat pentru parsing */
    char timestamp_buf[MAX_TIMESTAMP];
    char request[1024];
    
    /* Extragem IP-ul și timestamp-ul */
    int items = sscanf(line, "%15s %*s %*s [%63[^]]] \"%1023[^\"]\" %d %ld",
                       entry->ip, timestamp_buf, request, 
                       &entry->status, &entry->bytes);
    
    if (items < 4) {
        return 0;
    }
    
    /* Salvăm timestamp-ul */
    strncpy(entry->timestamp, timestamp_buf, MAX_TIMESTAMP - 1);
    entry->timestamp[MAX_TIMESTAMP - 1] = '\0';
    
    /* Extragem metoda și URL-ul din request */
    char protocol[32];
    items = sscanf(request, "%9s %511s %31s", 
                   entry->method, entry->url, protocol);
    
    if (items < 2) {
        strcpy(entry->method, "UNKNOWN");
        strcpy(entry->url, "/");
    }
    
    /* Extragem ora */
    entry->hour = extrage_ora(timestamp_buf);
    if (entry->hour < 0) entry->hour = 0;
    
    /* Dacă bytes e negativ (parsing eșuat), setăm la 0 */
    if (entry->bytes < 0) entry->bytes = 0;
    
    return 1;
}

/* =============================================================================
 * FUNCȚII DE STATISTICI
 * =============================================================================
 */

/**
 * Actualizează contorul pentru un IP
 */
void actualizeaza_ip_count(Statistics *stats, const char *ip, 
                           IPCount *ip_counts, int *ip_count) {
    /* Căutăm IP-ul în lista existentă */
    for (int i = 0; i < *ip_count; i++) {
        if (strcmp(ip_counts[i].ip, ip) == 0) {
            ip_counts[i].count++;
            return;
        }
    }
    
    /* Adăugăm IP nou */
    if (*ip_count < MAX_ENTRIES) {
        strncpy(ip_counts[*ip_count].ip, ip, MAX_IP - 1);
        ip_counts[*ip_count].ip[MAX_IP - 1] = '\0';
        ip_counts[*ip_count].count = 1;
        (*ip_count)++;
    }
    
    (void)stats;  /* Evităm warning */
}

/**
 * Actualizează contorul pentru un URL
 */
void actualizeaza_url_count(Statistics *stats, const char *url,
                            URLCount *url_counts, int *url_count) {
    /* Căutăm URL-ul în lista existentă */
    for (int i = 0; i < *url_count; i++) {
        if (strcmp(url_counts[i].url, url) == 0) {
            url_counts[i].count++;
            return;
        }
    }
    
    /* Adăugăm URL nou */
    if (*url_count < MAX_ENTRIES) {
        strncpy(url_counts[*url_count].url, url, MAX_URL - 1);
        url_counts[*url_count].url[MAX_URL - 1] = '\0';
        url_counts[*url_count].count = 1;
        (*url_count)++;
    }
    
    (void)stats;
}

/**
 * Funcție de comparare pentru sortare descrescătoare (IP)
 */
int compara_ip_count(const void *a, const void *b) {
    const IPCount *ia = (const IPCount *)a;
    const IPCount *ib = (const IPCount *)b;
    return ib->count - ia->count;
}

/**
 * Funcție de comparare pentru sortare descrescătoare (URL)
 */
int compara_url_count(const void *a, const void *b) {
    const URLCount *ua = (const URLCount *)a;
    const URLCount *ub = (const URLCount *)b;
    return ub->count - ua->count;
}

/**
 * Procesează fișierul de log și calculează statisticile
 */
int proceseaza_log(const char *filename, Statistics *stats) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Eroare la deschiderea '%s': %s\n", 
                filename, strerror(errno));
        return -1;
    }
    
    /* Inițializare statistici */
    memset(stats, 0, sizeof(Statistics));
    
    /* Array-uri temporare pentru contorizare */
    IPCount *ip_counts = malloc(MAX_ENTRIES * sizeof(IPCount));
    URLCount *url_counts = malloc(MAX_ENTRIES * sizeof(URLCount));
    
    if (ip_counts == NULL || url_counts == NULL) {
        fprintf(stderr, "Eroare la alocarea memoriei\n");
        fclose(fp);
        free(ip_counts);
        free(url_counts);
        return -1;
    }
    
    int ip_count = 0;
    int url_count = 0;
    
    char line[MAX_LINE];
    LogEntry entry;
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (parseaza_linie_log(line, &entry)) {
            stats->total_requests++;
            
            /* Request-uri per oră */
            if (entry.hour >= 0 && entry.hour < 24) {
                stats->requests_per_hour[entry.hour]++;
            }
            
            /* Coduri de status */
            if (entry.status >= 200 && entry.status < 300) {
                stats->status_2xx++;
            } else if (entry.status >= 300 && entry.status < 400) {
                stats->status_3xx++;
            } else if (entry.status >= 400 && entry.status < 500) {
                stats->status_4xx++;
            } else if (entry.status >= 500) {
                stats->status_5xx++;
            }
            
            /* Metode HTTP */
            if (strcmp(entry.method, "GET") == 0) {
                stats->get_count++;
            } else if (strcmp(entry.method, "POST") == 0) {
                stats->post_count++;
            } else if (strcmp(entry.method, "PUT") == 0) {
                stats->put_count++;
            } else if (strcmp(entry.method, "DELETE") == 0) {
                stats->delete_count++;
            } else {
                stats->other_count++;
            }
            
            /* Total bytes */
            stats->total_bytes += entry.bytes;
            
            /* Actualizare contoare */
            actualizeaza_ip_count(stats, entry.ip, ip_counts, &ip_count);
            actualizeaza_url_count(stats, entry.url, url_counts, &url_count);
        }
    }
    
    fclose(fp);
    
    /* Sortăm și extragem top N */
    qsort(ip_counts, ip_count, sizeof(IPCount), compara_ip_count);
    qsort(url_counts, url_count, sizeof(URLCount), compara_url_count);
    
    stats->top_ips_count = (ip_count < TOP_N) ? ip_count : TOP_N;
    for (int i = 0; i < stats->top_ips_count; i++) {
        stats->top_ips[i] = ip_counts[i];
    }
    
    stats->top_urls_count = (url_count < TOP_N) ? url_count : TOP_N;
    for (int i = 0; i < stats->top_urls_count; i++) {
        stats->top_urls[i] = url_counts[i];
    }
    
    free(ip_counts);
    free(url_counts);
    
    return 0;
}

/* =============================================================================
 * GENERARE RAPORT
 * =============================================================================
 */

/**
 * Generează raportul în fișier
 */
int genereaza_raport(const char *filename, const Statistics *stats) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Eroare la crearea raportului: %s\n", strerror(errno));
        return -1;
    }
    
    fprintf(fp, "╔═══════════════════════════════════════════════════════════════════════════╗\n");
    fprintf(fp, "║                    RAPORT ANALIZĂ LOG-URI WEB                             ║\n");
    fprintf(fp, "╠═══════════════════════════════════════════════════════════════════════════╣\n\n");
    
    /* Sumar general */
    fprintf(fp, "📊 SUMAR GENERAL\n");
    fprintf(fp, "═══════════════════════════════════════════════════════════════════════════\n");
    fprintf(fp, "  Total request-uri:     %d\n", stats->total_requests);
    fprintf(fp, "  Total bytes transferați: %.2f MB\n", stats->total_bytes / (1024.0 * 1024.0));
    fprintf(fp, "\n");
    
    /* Metode HTTP */
    fprintf(fp, "📈 DISTRIBUȚIE METODE HTTP\n");
    fprintf(fp, "═══════════════════════════════════════════════════════════════════════════\n");
    fprintf(fp, "  GET:     %6d (%5.1f%%)\n", stats->get_count, 
            stats->total_requests > 0 ? 100.0 * stats->get_count / stats->total_requests : 0);
    fprintf(fp, "  POST:    %6d (%5.1f%%)\n", stats->post_count,
            stats->total_requests > 0 ? 100.0 * stats->post_count / stats->total_requests : 0);
    fprintf(fp, "  PUT:     %6d (%5.1f%%)\n", stats->put_count,
            stats->total_requests > 0 ? 100.0 * stats->put_count / stats->total_requests : 0);
    fprintf(fp, "  DELETE:  %6d (%5.1f%%)\n", stats->delete_count,
            stats->total_requests > 0 ? 100.0 * stats->delete_count / stats->total_requests : 0);
    fprintf(fp, "  Altele:  %6d (%5.1f%%)\n", stats->other_count,
            stats->total_requests > 0 ? 100.0 * stats->other_count / stats->total_requests : 0);
    fprintf(fp, "\n");
    
    /* Coduri de status */
    fprintf(fp, "📋 DISTRIBUȚIE CODURI DE STATUS\n");
    fprintf(fp, "═══════════════════════════════════════════════════════════════════════════\n");
    fprintf(fp, "  2xx (Success):     %6d (%5.1f%%)\n", stats->status_2xx,
            stats->total_requests > 0 ? 100.0 * stats->status_2xx / stats->total_requests : 0);
    fprintf(fp, "  3xx (Redirect):    %6d (%5.1f%%)\n", stats->status_3xx,
            stats->total_requests > 0 ? 100.0 * stats->status_3xx / stats->total_requests : 0);
    fprintf(fp, "  4xx (Client Err):  %6d (%5.1f%%)\n", stats->status_4xx,
            stats->total_requests > 0 ? 100.0 * stats->status_4xx / stats->total_requests : 0);
    fprintf(fp, "  5xx (Server Err):  %6d (%5.1f%%)\n", stats->status_5xx,
            stats->total_requests > 0 ? 100.0 * stats->status_5xx / stats->total_requests : 0);
    fprintf(fp, "\n");
    
    /* Request-uri per oră */
    fprintf(fp, "⏰ REQUEST-URI PER ORĂ\n");
    fprintf(fp, "═══════════════════════════════════════════════════════════════════════════\n");
    for (int h = 0; h < 24; h++) {
        fprintf(fp, "  %02d:00 - %02d:59  │ %6d │ ", h, h, stats->requests_per_hour[h]);
        /* Histogramă simplă */
        int bars = (stats->total_requests > 0) ? 
                   (40 * stats->requests_per_hour[h] / stats->total_requests) : 0;
        if (bars == 0 && stats->requests_per_hour[h] > 0) bars = 1;
        for (int i = 0; i < bars; i++) fprintf(fp, "█");
        fprintf(fp, "\n");
    }
    fprintf(fp, "\n");
    
    /* Top IP-uri */
    fprintf(fp, "🔝 TOP %d IP-URI (după numărul de request-uri)\n", stats->top_ips_count);
    fprintf(fp, "═══════════════════════════════════════════════════════════════════════════\n");
    fprintf(fp, "  Loc │ IP Address      │ Request-uri │ Procent\n");
    fprintf(fp, "  ────┼─────────────────┼─────────────┼─────────\n");
    for (int i = 0; i < stats->top_ips_count; i++) {
        fprintf(fp, "  %3d │ %-15s │ %11d │ %6.2f%%\n", 
                i + 1, stats->top_ips[i].ip, stats->top_ips[i].count,
                stats->total_requests > 0 ? 
                100.0 * stats->top_ips[i].count / stats->total_requests : 0);
    }
    fprintf(fp, "\n");
    
    /* Top URL-uri */
    fprintf(fp, "🔝 TOP %d URL-URI (cele mai accesate)\n", stats->top_urls_count);
    fprintf(fp, "═══════════════════════════════════════════════════════════════════════════\n");
    fprintf(fp, "  Loc │ URL                                        │ Accesări\n");
    fprintf(fp, "  ────┼────────────────────────────────────────────┼──────────\n");
    for (int i = 0; i < stats->top_urls_count; i++) {
        char url_truncat[45];
        strncpy(url_truncat, stats->top_urls[i].url, 44);
        url_truncat[44] = '\0';
        fprintf(fp, "  %3d │ %-42s │ %8d\n", 
                i + 1, url_truncat, stats->top_urls[i].count);
    }
    
    fprintf(fp, "\n╚═══════════════════════════════════════════════════════════════════════════╝\n");
    fprintf(fp, "\nRaport generat automat - ATP Tema 1\n");
    
    fclose(fp);
    return 0;
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      SOLUȚIE TEMA 1: Procesator de Log-uri Web                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Verificare argumente */
    if (argc < 2) {
        /* Creem un fișier de log de test */
        printf("📝 Creare fișier de log de test...\n");
        FILE *fp = fopen("test_access.log", "w");
        if (fp) {
            fprintf(fp, "192.168.1.100 - - [15/Jan/2024:10:30:45 +0200] \"GET /index.html HTTP/1.1\" 200 2326\n");
            fprintf(fp, "192.168.1.101 - - [15/Jan/2024:10:30:46 +0200] \"POST /api/login HTTP/1.1\" 401 156\n");
            fprintf(fp, "10.0.0.50 - - [15/Jan/2024:10:30:47 +0200] \"GET /images/logo.png HTTP/1.1\" 200 45678\n");
            fprintf(fp, "192.168.1.100 - - [15/Jan/2024:11:15:22 +0200] \"GET /about.html HTTP/1.1\" 200 5432\n");
            fprintf(fp, "192.168.1.102 - - [15/Jan/2024:11:16:33 +0200] \"GET /contact HTTP/1.1\" 404 234\n");
            fprintf(fp, "10.0.0.50 - - [15/Jan/2024:12:00:00 +0200] \"POST /api/data HTTP/1.1\" 500 100\n");
            fprintf(fp, "192.168.1.100 - - [15/Jan/2024:14:30:00 +0200] \"GET /index.html HTTP/1.1\" 200 2326\n");
            fprintf(fp, "192.168.1.103 - - [15/Jan/2024:14:31:00 +0200] \"PUT /api/update HTTP/1.1\" 200 500\n");
            fprintf(fp, "192.168.1.100 - - [15/Jan/2024:15:00:00 +0200] \"DELETE /api/item/5 HTTP/1.1\" 204 0\n");
            fprintf(fp, "10.0.0.51 - - [15/Jan/2024:16:45:00 +0200] \"GET /style.css HTTP/1.1\" 304 0\n");
            fclose(fp);
            printf("✓ Fișier 'test_access.log' creat\n\n");
        }
    }
    
    const char *log_file = (argc > 1) ? argv[1] : "test_access.log";
    const char *report_file = (argc > 2) ? argv[2] : "log_report.txt";
    
    printf("📂 Procesare fișier '%s'...\n", log_file);
    
    Statistics stats;
    if (proceseaza_log(log_file, &stats) != 0) {
        return EXIT_FAILURE;
    }
    
    printf("✓ %d request-uri procesate\n\n", stats.total_requests);
    
    printf("📝 Generare raport '%s'...\n", report_file);
    if (genereaza_raport(report_file, &stats) != 0) {
        return EXIT_FAILURE;
    }
    printf("✓ Raport generat cu succes!\n\n");
    
    /* Afișare rezumat în consolă */
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  REZUMAT\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  Total request-uri: %d\n", stats.total_requests);
    printf("  Total bytes:       %.2f KB\n", stats.total_bytes / 1024.0);
    printf("  Status 2xx:        %d\n", stats.status_2xx);
    printf("  Status 4xx:        %d\n", stats.status_4xx);
    printf("  Status 5xx:        %d\n", stats.status_5xx);
    printf("═══════════════════════════════════════════════════════════════\n");
    
    printf("\n✅ Procesare completă! Verificați raportul: %s\n", report_file);
    
    /* Cleanup fișier de test */
    if (argc < 2) {
        remove("test_access.log");
    }
    
    return EXIT_SUCCESS;
}
