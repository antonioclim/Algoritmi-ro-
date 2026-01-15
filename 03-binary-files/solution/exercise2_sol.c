/**
 * SOLUȚIE EXERCIȚIUL 2: Convertor BMP Grayscale
 * ATENȚIE: Destinat DOAR instructorilor!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t signature;
    uint32_t file_size;
    uint16_t reserved1, reserved2;
    uint32_t pixel_offset;
} BMPFileHeader;

typedef struct {
    uint32_t header_size;
    int32_t width, height;
    uint16_t planes, bits_per_pixel;
    uint32_t compression, image_size;
    int32_t x_ppm, y_ppm;
    uint32_t colors_used, colors_important;
} BMPInfoHeader;

typedef struct {
    uint8_t blue, green, red, reserved;
} RGBQuad;
#pragma pack(pop)

typedef struct {
    BMPFileHeader file_header;
    BMPInfoHeader info_header;
    uint8_t *pixel_data;
    int row_padding;
    size_t data_size;
} BMPImage;

int calc_padding(int width, int bpp) {
    int row_bytes = width * (bpp / 8);
    return (4 - (row_bytes % 4)) % 4;
}

uint8_t rgb_to_gray(uint8_t r, uint8_t g, uint8_t b) {
    return (uint8_t)(0.299 * r + 0.587 * g + 0.114 * b + 0.5);
}

int read_bmp(const char *filename, BMPImage *img) {
    FILE *f = fopen(filename, "rb");
    if (!f) { perror("Eroare deschidere"); return -1; }
    
    fread(&img->file_header, sizeof(BMPFileHeader), 1, f);
    if (img->file_header.signature != 0x4D42) {
        fprintf(stderr, "Nu este fișier BMP valid\n");
        fclose(f); return -1;
    }
    
    fread(&img->info_header, sizeof(BMPInfoHeader), 1, f);
    if (img->info_header.bits_per_pixel != 24 || img->info_header.compression != 0) {
        fprintf(stderr, "Doar BMP 24-bit necomprimat suportat\n");
        fclose(f); return -1;
    }
    
    img->row_padding = calc_padding(img->info_header.width, 24);
    int row_size = img->info_header.width * 3 + img->row_padding;
    int height = abs(img->info_header.height);
    img->data_size = row_size * height;
    
    img->pixel_data = malloc(img->data_size);
    if (!img->pixel_data) { fclose(f); return -1; }
    
    fseek(f, img->file_header.pixel_offset, SEEK_SET);
    fread(img->pixel_data, 1, img->data_size, f);
    fclose(f);
    return 0;
}

int convert_grayscale(const BMPImage *img, uint8_t **out) {
    int width = img->info_header.width;
    int height = abs(img->info_header.height);
    int out_padding = calc_padding(width, 8);
    int out_row = width + out_padding;
    int in_row = width * 3 + img->row_padding;
    
    *out = calloc(out_row * height, 1);
    if (!*out) return -1;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int in_off = y * in_row + x * 3;
            uint8_t b = img->pixel_data[in_off];
            uint8_t g = img->pixel_data[in_off + 1];
            uint8_t r = img->pixel_data[in_off + 2];
            (*out)[y * out_row + x] = rgb_to_gray(r, g, b);
        }
    }
    return 0;
}

int write_grayscale(const char *filename, const BMPImage *orig, uint8_t *gray) {
    int w = orig->info_header.width, h = abs(orig->info_header.height);
    int pad = calc_padding(w, 8), row = w + pad;
    uint32_t pal_size = 256 * sizeof(RGBQuad);
    uint32_t pix_size = row * h;
    uint32_t pix_off = 14 + 40 + pal_size;
    
    BMPFileHeader fh = {0x4D42, pix_off + pix_size, 0, 0, pix_off};
    BMPInfoHeader ih = {40, w, orig->info_header.height, 1, 8, 0, pix_size,
                        orig->info_header.x_ppm, orig->info_header.y_ppm, 256, 0};
    
    RGBQuad palette[256];
    for (int i = 0; i < 256; i++)
        palette[i] = (RGBQuad){i, i, i, 0};
    
    FILE *f = fopen(filename, "wb");
    if (!f) return -1;
    fwrite(&fh, sizeof(fh), 1, f);
    fwrite(&ih, sizeof(ih), 1, f);
    fwrite(palette, sizeof(RGBQuad), 256, f);
    fwrite(gray, 1, pix_size, f);
    fclose(f);
    printf("Salvat: %s (%u bytes)\n", filename, fh.file_size);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Utilizare: %s <input.bmp> <output.bmp>\n", argv[0]);
        return 1;
    }
    
    BMPImage img = {0};
    if (read_bmp(argv[1], &img) != 0) return 1;
    
    printf("Imagine: %dx%d, %d-bit\n", img.info_header.width,
           abs(img.info_header.height), img.info_header.bits_per_pixel);
    
    uint8_t *gray = NULL;
    if (convert_grayscale(&img, &gray) != 0) { free(img.pixel_data); return 1; }
    
    write_grayscale(argv[2], &img, gray);
    
    free(gray);
    free(img.pixel_data);
    printf("Conversie completă!\n");
    return 0;
}
