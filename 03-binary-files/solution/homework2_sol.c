/**
 * SOLUȚIE TEMA 2: Editor Imagini BMP
 * ATENȚIE: Destinat DOAR instructorilor!
 * Compilare: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c -lm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#pragma pack(push, 1)
typedef struct { uint16_t sig; uint32_t size; uint16_t r1, r2; uint32_t off; } BFH;
typedef struct { uint32_t sz; int32_t w, h; uint16_t pl, bpp; uint32_t cmp, isz; int32_t xp, yp; uint32_t cu, ci; } BIH;
#pragma pack(pop)

typedef struct { BFH fh; BIH ih; uint8_t *data; int pad; } IMG;

int calc_pad(int w, int bpp) { return (4 - (w * bpp / 8) % 4) % 4; }
uint8_t to_gray(uint8_t r, uint8_t g, uint8_t b) { return (uint8_t)(0.299*r + 0.587*g + 0.114*b); }

int read_img(const char *fn, IMG *img) {
    FILE *f = fopen(fn, "rb"); if (!f) return -1;
    fread(&img->fh, sizeof(BFH), 1, f);
    if (img->fh.sig != 0x4D42) { fclose(f); return -1; }
    fread(&img->ih, sizeof(BIH), 1, f);
    img->pad = calc_pad(img->ih.w, img->ih.bpp);
    int row = img->ih.w * 3 + img->pad;
    int h = abs(img->ih.h);
    img->data = malloc(row * h);
    fseek(f, img->fh.off, SEEK_SET);
    fread(img->data, 1, row * h, f);
    fclose(f);
    return 0;
}

void flip_h(IMG *img) {
    int row = img->ih.w * 3 + img->pad;
    int h = abs(img->ih.h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < img->ih.w / 2; x++) {
            int l = y * row + x * 3;
            int r = y * row + (img->ih.w - 1 - x) * 3;
            for (int c = 0; c < 3; c++) {
                uint8_t t = img->data[l + c];
                img->data[l + c] = img->data[r + c];
                img->data[r + c] = t;
            }
        }
    }
}

void flip_v(IMG *img) {
    int row = img->ih.w * 3 + img->pad;
    int h = abs(img->ih.h);
    uint8_t *tmp = malloc(row);
    for (int y = 0; y < h / 2; y++) {
        memcpy(tmp, img->data + y * row, row);
        memcpy(img->data + y * row, img->data + (h - 1 - y) * row, row);
        memcpy(img->data + (h - 1 - y) * row, tmp, row);
    }
    free(tmp);
}

void brightness(IMG *img, int delta) {
    int row = img->ih.w * 3 + img->pad;
    int h = abs(img->ih.h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < img->ih.w * 3; x++) {
            int v = img->data[y * row + x] + delta;
            img->data[y * row + x] = v < 0 ? 0 : (v > 255 ? 255 : v);
        }
    }
}

int write_img(const char *fn, IMG *img) {
    FILE *f = fopen(fn, "wb"); if (!f) return -1;
    int row = img->ih.w * 3 + img->pad;
    fwrite(&img->fh, sizeof(BFH), 1, f);
    fwrite(&img->ih, sizeof(BIH), 1, f);
    fseek(f, img->fh.off, SEEK_SET);
    fwrite(img->data, 1, row * abs(img->ih.h), f);
    fclose(f);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Utilizare: %s <operatie> <input.bmp> <output.bmp> [param]\n", argv[0]);
        printf("Operații: grayscale, flip_h, flip_v, brightness <delta>\n");
        return 1;
    }
    
    IMG img = {0};
    if (read_img(argv[2], &img) != 0) {
        fprintf(stderr, "Eroare citire %s\n", argv[2]);
        return 1;
    }
    
    if (strcmp(argv[1], "flip_h") == 0) flip_h(&img);
    else if (strcmp(argv[1], "flip_v") == 0) flip_v(&img);
    else if (strcmp(argv[1], "brightness") == 0 && argc > 4) brightness(&img, atoi(argv[4]));
    
    write_img(argv[3], &img);
    printf("Operație completă: %s -> %s\n", argv[2], argv[3]);
    
    free(img.data);
    return 0;
}
