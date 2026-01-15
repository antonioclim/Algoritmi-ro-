/**
 * =============================================================================
 * EXERCIȚIUL 2: Convertor Imagine BMP în Grayscale
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați un program care citește o imagine BMP în format RGB (24-bit)
 *   și o convertește în grayscale (8-bit cu paletă).
 *
 * CERINȚE:
 *   1. Citiți și parseați header-ele BMP (FileHeader și InfoHeader)
 *   2. Validați că imaginea este în format suportat (24-bit, necomprimat)
 *   3. Citiți datele pixelilor, gestionând padding-ul corect
 *   4. Convertiți fiecare pixel la grayscale folosind formula luminanței
 *   5. Scrieți imaginea rezultată în format 8-bit cu paletă grayscale
 *   6. Afișați informații despre procesare
 *
 * FORMULA CONVERSIE GRAYSCALE (ITU-R BT.601):
 *   Gray = 0.299 * R + 0.587 * G + 0.114 * B
 *
 * EXEMPLU UTILIZARE:
 *   ./exercise2 input.bmp output.bmp
 *
 * OUTPUT AȘTEPTAT:
 *   - Informații despre imaginea de intrare
 *   - Imaginea convertită salvată
 *   - Statistici despre procesare
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c -lm
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
 * STRUCTURI BMP
 * =============================================================================
 */

/**
 * TODO 1: Completați structura BMP File Header
 *
 * BMP File Header are exact 14 bytes și conține:
 *   - signature (uint16_t): 0x4D42 ("BM" în little-endian)
 *   - file_size (uint32_t): dimensiunea totală a fișierului
 *   - reserved1 (uint16_t): rezervat, trebuie să fie 0
 *   - reserved2 (uint16_t): rezervat, trebuie să fie 0
 *   - pixel_offset (uint32_t): offset de la început la datele pixelilor
 *
 * IMPORTANT: Utilizați #pragma pack(push, 1) pentru a dezactiva padding-ul!
 */

#pragma pack(push, 1)
typedef struct {
    /* YOUR CODE HERE */
    uint16_t signature;      /* "BM" = 0x4D42 */
    uint32_t file_size;      /* Dimensiune totală fișier */
    /* Completați restul câmpurilor... */
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t pixel_offset;
} BMPFileHeader;
#pragma pack(pop)

/**
 * TODO 2: Completați structura BMP Info Header (DIB Header)
 *
 * Info Header (BITMAPINFOHEADER) are 40 bytes și conține:
 *   - header_size (uint32_t): dimensiunea acestui header (40)
 *   - width (int32_t): lățimea imaginii în pixeli
 *   - height (int32_t): înălțimea imaginii (pozitiv = bottom-up)
 *   - planes (uint16_t): număr planuri de culoare (întotdeauna 1)
 *   - bits_per_pixel (uint16_t): biți per pixel (24 pentru RGB)
 *   - compression (uint32_t): tip compresie (0 = BI_RGB, necomprimat)
 *   - image_size (uint32_t): dimensiune date imagine (poate fi 0 pentru BI_RGB)
 *   - x_pixels_per_meter (int32_t): rezoluție orizontală
 *   - y_pixels_per_meter (int32_t): rezoluție verticală
 *   - colors_used (uint32_t): număr culori în paletă (0 = maxim)
 *   - colors_important (uint32_t): culori importante (0 = toate)
 */

#pragma pack(push, 1)
typedef struct {
    uint32_t header_size;
    int32_t  width;
    int32_t  height;
    /* YOUR CODE HERE - Completați restul câmpurilor */
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size;
    int32_t  x_pixels_per_meter;
    int32_t  y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
} BMPInfoHeader;
#pragma pack(pop)

/**
 * Structură pentru un pixel RGB (24-bit)
 * Notă: BMP stochează pixelii în ordine BGR!
 */
#pragma pack(push, 1)
typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} RGBPixel;
#pragma pack(pop)

/**
 * Structură pentru o intrare în paleta de culori
 */
#pragma pack(push, 1)
typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;  /* Întotdeauna 0 */
} RGBQuad;
#pragma pack(pop)

/**
 * Structură pentru a stoca întreaga imagine
 */
typedef struct {
    BMPFileHeader file_header;
    BMPInfoHeader info_header;
    uint8_t *pixel_data;     /* Date pixeli (raw) */
    int row_padding;         /* Bytes de padding per rând */
    size_t data_size;        /* Dimensiune date pixeli */
} BMPImage;

/* =============================================================================
 * PROTOTIPURI FUNCȚII
 * =============================================================================
 */

/* Funcții de citire/scriere */
int citeste_bmp(const char *filename, BMPImage *img);
int scrie_bmp_grayscale(const char *filename, const BMPImage *img_original, 
                        uint8_t *grayscale_data);

/* Funcții de procesare */
int converteste_grayscale(const BMPImage *img, uint8_t **grayscale_data);
uint8_t rgb_to_gray(uint8_t r, uint8_t g, uint8_t b);

/* Funcții utilitare */
void afiseaza_info_bmp(const BMPImage *img);
int calculeaza_padding(int width, int bits_per_pixel);
void elibereaza_bmp(BMPImage *img);

/* =============================================================================
 * IMPLEMENTARE FUNCȚII
 * =============================================================================
 */

/**
 * TODO 3: Implementați funcția calculeaza_padding
 *
 * În formatul BMP, fiecare rând de pixeli trebuie să fie aliniat la 4 bytes.
 * Formula: padding = (4 - (row_bytes % 4)) % 4
 *
 * @param width Lățimea imaginii în pixeli
 * @param bits_per_pixel Biți per pixel (24 pentru RGB, 8 pentru grayscale)
 * @return Numărul de bytes de padding necesari per rând
 *
 * Exemplu: Pentru o imagine de 3 pixeli lățime, 24-bit:
 *   row_bytes = 3 * 3 = 9 bytes
 *   padding = (4 - (9 % 4)) % 4 = (4 - 1) % 4 = 3 bytes
 */
int calculeaza_padding(int width, int bits_per_pixel) {
    /* YOUR CODE HERE */
    int bytes_per_pixel = bits_per_pixel / 8;
    int row_bytes = width * bytes_per_pixel;
    
    /* Calculați padding-ul */
    
    return 0;  /* Înlocuiți cu formula corectă */
}

/**
 * TODO 4: Implementați funcția rgb_to_gray
 *
 * Convertește un pixel RGB la grayscale folosind formula luminanței
 * (ITU-R BT.601, standard pentru televiziune).
 *
 * Formula: Gray = 0.299 * R + 0.587 * G + 0.114 * B
 *
 * @param r Componenta roșie (0-255)
 * @param g Componenta verde (0-255)
 * @param b Componenta albastră (0-255)
 * @return Valoarea grayscale (0-255)
 *
 * Hint: Utilizați rotunjire: (uint8_t)(0.299*r + 0.587*g + 0.114*b + 0.5)
 */
uint8_t rgb_to_gray(uint8_t r, uint8_t g, uint8_t b) {
    /* YOUR CODE HERE */
    return 0;  /* Înlocuiți cu implementarea corectă */
}

/**
 * TODO 5: Implementați funcția citeste_bmp
 *
 * Citește un fișier BMP și stochează datele în structura BMPImage.
 *
 * @param filename Calea către fișierul BMP
 * @param img Pointer către structura BMPImage (output)
 * @return 0 pentru succes, -1 pentru eroare
 *
 * Steps:
 *   1. Deschideți fișierul în modul "rb"
 *   2. Citiți BMPFileHeader (14 bytes)
 *   3. Validați signature-ul (trebuie să fie 0x4D42)
 *   4. Citiți BMPInfoHeader (40 bytes)
 *   5. Validați: bits_per_pixel == 24 și compression == 0
 *   6. Calculați padding-ul
 *   7. Alocați memorie pentru datele pixelilor
 *   8. Poziționați-vă la pixel_offset și citiți datele
 *   9. Închideți fișierul
 */
int citeste_bmp(const char *filename, BMPImage *img) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Eroare: Nu s-a putut deschide fișierul %s\n", filename);
        return -1;
    }
    
    /* TODO: Citiți file header */
    /* YOUR CODE HERE */
    if (fread(&img->file_header, sizeof(BMPFileHeader), 1, f) != 1) {
        fprintf(stderr, "Eroare: Nu s-a putut citi file header\n");
        fclose(f);
        return -1;
    }
    
    /* TODO 5a: Validați signature-ul */
    /* YOUR CODE HERE */
    /* Hint: if (img->file_header.signature != 0x4D42) { error... } */
    
    
    /* TODO: Citiți info header */
    /* YOUR CODE HERE */
    if (fread(&img->info_header, sizeof(BMPInfoHeader), 1, f) != 1) {
        fprintf(stderr, "Eroare: Nu s-a putut citi info header\n");
        fclose(f);
        return -1;
    }
    
    /* TODO 5b: Validați formatul (24-bit, necomprimat) */
    /* YOUR CODE HERE */
    /* Hint: Verificați bits_per_pixel și compression */
    
    
    /* TODO 5c: Calculați padding-ul */
    /* YOUR CODE HERE */
    img->row_padding = calculeaza_padding(img->info_header.width, 
                                          img->info_header.bits_per_pixel);
    
    /* TODO 5d: Calculați dimensiunea datelor și alocați memorie */
    /* YOUR CODE HERE */
    int row_size = img->info_header.width * 3 + img->row_padding;
    int height = abs(img->info_header.height);
    img->data_size = row_size * height;
    
    img->pixel_data = (uint8_t *)malloc(img->data_size);
    if (!img->pixel_data) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie\n");
        fclose(f);
        return -1;
    }
    
    /* TODO 5e: Poziționați-vă la datele pixelilor și citiți */
    /* YOUR CODE HERE */
    /* Hint: fseek(f, img->file_header.pixel_offset, SEEK_SET); */
    /* Apoi: fread(img->pixel_data, 1, img->data_size, f); */
    
    
    fclose(f);
    return 0;
}

/**
 * TODO 6: Implementați funcția converteste_grayscale
 *
 * Convertește datele pixelilor din RGB în grayscale.
 *
 * @param img Imaginea originală (RGB, 24-bit)
 * @param grayscale_data Pointer către buffer-ul rezultat (va fi alocat)
 * @return 0 pentru succes, -1 pentru eroare
 *
 * Steps:
 *   1. Calculați dimensiunile pentru imaginea grayscale (8-bit)
 *   2. Alocați buffer pentru datele grayscale
 *   3. Pentru fiecare rând (de jos în sus în BMP!):
 *      a. Pentru fiecare pixel din rând:
 *         - Citiți componentele B, G, R (ordinea în BMP!)
 *         - Convertiți la grayscale cu rgb_to_gray()
 *         - Stocați în buffer-ul de ieșire
 *      b. Gestionați padding-ul corespunzător
 *
 * ATENȚIE la ordinea pixelilor în BMP:
 *   - Pixelii sunt stocați BGR, nu RGB!
 *   - Rândurile sunt stocate de jos în sus (bottom-up) dacă height > 0
 */
int converteste_grayscale(const BMPImage *img, uint8_t **grayscale_data) {
    int width = img->info_header.width;
    int height = abs(img->info_header.height);
    
    /* Calculare padding pentru output (8-bit) */
    int output_padding = calculeaza_padding(width, 8);
    int output_row_size = width + output_padding;
    
    /* Alocare memorie pentru output */
    size_t output_size = output_row_size * height;
    *grayscale_data = (uint8_t *)malloc(output_size);
    if (!*grayscale_data) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie pentru output\n");
        return -1;
    }
    
    /* Inițializare cu 0 (pentru padding) */
    memset(*grayscale_data, 0, output_size);
    
    /* TODO 6: Implementați conversia */
    /* YOUR CODE HERE */
    
    /* Hint pentru iterare:
     * int input_row_size = width * 3 + img->row_padding;
     * 
     * for (int y = 0; y < height; y++) {
     *     for (int x = 0; x < width; x++) {
     *         // Calculați offset-ul în datele de intrare
     *         int input_offset = y * input_row_size + x * 3;
     *         
     *         // Citiți componentele (BMP stochează BGR!)
     *         uint8_t b = img->pixel_data[input_offset + 0];
     *         uint8_t g = img->pixel_data[input_offset + 1];
     *         uint8_t r = img->pixel_data[input_offset + 2];
     *         
     *         // Convertiți la grayscale
     *         uint8_t gray = rgb_to_gray(r, g, b);
     *         
     *         // Calculați offset-ul în datele de ieșire
     *         int output_offset = y * output_row_size + x;
     *         (*grayscale_data)[output_offset] = gray;
     *     }
     * }
     */
    
    return 0;
}

/**
 * TODO 7: Implementați funcția scrie_bmp_grayscale
 *
 * Scrie o imagine grayscale (8-bit) cu paletă.
 *
 * @param filename Calea pentru fișierul de ieșire
 * @param img_original Imaginea originală (pentru dimensiuni)
 * @param grayscale_data Datele grayscale de scris
 * @return 0 pentru succes, -1 pentru eroare
 *
 * Steps:
 *   1. Pregătiți file header-ul (actualizați dimensiunile)
 *   2. Pregătiți info header-ul (bits_per_pixel = 8, colors_used = 256)
 *   3. Creați paleta grayscale (256 intrări RGBQuad: 0,0,0 până la 255,255,255)
 *   4. Deschideți fișierul în modul "wb"
 *   5. Scrieți file header
 *   6. Scrieți info header
 *   7. Scrieți paleta (256 * 4 = 1024 bytes)
 *   8. Scrieți datele pixelilor
 *   9. Închideți fișierul
 */
int scrie_bmp_grayscale(const char *filename, const BMPImage *img_original, 
                        uint8_t *grayscale_data) {
    
    int width = img_original->info_header.width;
    int height = abs(img_original->info_header.height);
    int output_padding = calculeaza_padding(width, 8);
    int row_size = width + output_padding;
    
    /* Calculare dimensiuni fișier */
    uint32_t palette_size = 256 * sizeof(RGBQuad);  /* 1024 bytes */
    uint32_t pixel_data_size = row_size * height;
    uint32_t pixel_offset = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + palette_size;
    uint32_t file_size = pixel_offset + pixel_data_size;
    
    /* Pregătire file header */
    BMPFileHeader file_header;
    file_header.signature = 0x4D42;  /* "BM" */
    file_header.file_size = file_size;
    file_header.reserved1 = 0;
    file_header.reserved2 = 0;
    file_header.pixel_offset = pixel_offset;
    
    /* TODO 7a: Pregătiți info header */
    /* YOUR CODE HERE */
    BMPInfoHeader info_header;
    info_header.header_size = sizeof(BMPInfoHeader);
    info_header.width = width;
    info_header.height = img_original->info_header.height;  /* Păstrăm semnul */
    info_header.planes = 1;
    info_header.bits_per_pixel = 8;  /* Grayscale! */
    info_header.compression = 0;
    info_header.image_size = pixel_data_size;
    info_header.x_pixels_per_meter = img_original->info_header.x_pixels_per_meter;
    info_header.y_pixels_per_meter = img_original->info_header.y_pixels_per_meter;
    info_header.colors_used = 256;
    info_header.colors_important = 0;
    
    /* TODO 7b: Creați paleta grayscale */
    /* YOUR CODE HERE */
    RGBQuad palette[256];
    /* Hint: 
     * for (int i = 0; i < 256; i++) {
     *     palette[i].red = i;
     *     palette[i].green = i;
     *     palette[i].blue = i;
     *     palette[i].reserved = 0;
     * }
     */
    
    /* Deschidere fișier și scriere */
    FILE *f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Eroare: Nu s-a putut crea fișierul %s\n", filename);
        return -1;
    }
    
    /* TODO 7c: Scrieți toate componentele */
    /* YOUR CODE HERE */
    /* 
     * fwrite(&file_header, sizeof(BMPFileHeader), 1, f);
     * fwrite(&info_header, sizeof(BMPInfoHeader), 1, f);
     * fwrite(palette, sizeof(RGBQuad), 256, f);
     * fwrite(grayscale_data, 1, pixel_data_size, f);
     */
    
    fclose(f);
    printf("Imagine salvată: %s\n", filename);
    printf("Dimensiune fișier: %u bytes\n", file_size);
    
    return 0;
}

/**
 * Afișează informații despre o imagine BMP
 */
void afiseaza_info_bmp(const BMPImage *img) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                   INFORMAȚII IMAGINE BMP                      ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║  Dimensiune fișier:     %10u bytes                      ║\n", 
           img->file_header.file_size);
    printf("║  Offset date pixeli:    %10u bytes                      ║\n", 
           img->file_header.pixel_offset);
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║  Dimensiune header:     %10u bytes                      ║\n", 
           img->info_header.header_size);
    printf("║  Lățime:                %10d pixeli                     ║\n", 
           img->info_header.width);
    printf("║  Înălțime:              %10d pixeli                     ║\n", 
           img->info_header.height);
    printf("║  Biți per pixel:        %10u                            ║\n", 
           img->info_header.bits_per_pixel);
    printf("║  Compresie:             %10u (0=necomprimat)            ║\n", 
           img->info_header.compression);
    printf("║  Dimensiune imagine:    %10u bytes                      ║\n", 
           img->info_header.image_size);
    printf("║  Padding per rând:      %10d bytes                      ║\n", 
           img->row_padding);
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
}

/**
 * Eliberează memoria alocată pentru imagine
 */
void elibereaza_bmp(BMPImage *img) {
    if (img->pixel_data) {
        free(img->pixel_data);
        img->pixel_data = NULL;
    }
}

/* =============================================================================
 * FUNCȚIA MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           EXERCIȚIUL 2: CONVERTOR BMP GRAYSCALE               ║\n");
    printf("║           Săptămâna 03 - Fișiere Binare                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Verificare argumente */
    if (argc != 3) {
        printf("\nUtilizare: %s <input.bmp> <output.bmp>\n", argv[0]);
        printf("\nExemplu:\n");
        printf("  %s imagine.bmp imagine_gray.bmp\n", argv[0]);
        printf("\nNotă: Imaginea de intrare trebuie să fie în format BMP 24-bit.\n");
        
        /* Demonstrație cu fișier generat */
        printf("\n--- Mod demonstrativ (generare imagine test) ---\n");
        
        /* TODO 8: Generați o imagine test simplă */
        /* YOUR CODE HERE */
        /* Hint: Creați un gradient sau un pattern simplu */
        
        return 1;
    }
    
    const char *input_file = argv[1];
    const char *output_file = argv[2];
    
    /* Măsurare timp */
    clock_t start = clock();
    
    /* Citire imagine */
    printf("\nCitire imagine: %s\n", input_file);
    BMPImage img = {0};
    if (citeste_bmp(input_file, &img) != 0) {
        fprintf(stderr, "Eroare la citirea imaginii!\n");
        return 1;
    }
    
    /* Afișare informații */
    afiseaza_info_bmp(&img);
    
    /* Conversie grayscale */
    printf("\nConversie în grayscale...\n");
    uint8_t *grayscale_data = NULL;
    if (converteste_grayscale(&img, &grayscale_data) != 0) {
        fprintf(stderr, "Eroare la conversie!\n");
        elibereaza_bmp(&img);
        return 1;
    }
    
    /* Scriere rezultat */
    printf("\nScriere imagine: %s\n", output_file);
    if (scrie_bmp_grayscale(output_file, &img, grayscale_data) != 0) {
        fprintf(stderr, "Eroare la scriere!\n");
        free(grayscale_data);
        elibereaza_bmp(&img);
        return 1;
    }
    
    /* Măsurare timp final */
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    
    /* Statistici */
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                   STATISTICI PROCESARE                        ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║  Pixeli procesați:      %10d                            ║\n", 
           img.info_header.width * abs(img.info_header.height));
    printf("║  Timp procesare:        %10.3f secunde                   ║\n", 
           time_spent);
    printf("║  Viteză:                %10.0f pixeli/sec                ║\n", 
           (img.info_header.width * abs(img.info_header.height)) / time_spent);
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Eliberare memorie */
    free(grayscale_data);
    elibereaza_bmp(&img);
    
    printf("\nConversie finalizată cu succes!\n");
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opționale)
 * =============================================================================
 *
 * 1. Adăugați suport pentru imagini top-down (height negativ)
 *
 * 2. Implementați alte formule de conversie grayscale:
 *    - Average: (R + G + B) / 3
 *    - Lightness: (max(R,G,B) + min(R,G,B)) / 2
 *    - Desaturation: max(R,G,B)
 *
 * 3. Adăugați suport pentru imagini BMP 32-bit (RGBA)
 *
 * 4. Implementați un filtru de contrast sau brightness
 *
 * 5. Generați un histogram al nivelurilor de gri și afișați-l ASCII
 *
 * 6. Implementați detecție margini (Sobel edge detection) după conversie
 *
 * =============================================================================
 */
