#define _POSIX_C_SOURCE 200809L
#include "sh_image.h"
#include "khash.h"
#include <math.h>
#include <unistd.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdlib.h>

uint32_t color_map[] = { 0x000000, 0x00005F, 0x000080, 0x000087, 0x0000AF, 0x0000D7, 0x0000FF, 0x005F5F, 0x005F87, 0x005FAF, 0x005FD7, 0x005FFF, 0x008000, 0x008080, 0x008700, 0x00875F, 0x008787, 0x0087AF, 0x0087D7, 0x0087FF, 0x00AF00, 0x00AF5F, 0x00AF87, 0x00AFAF, 0x00AFFF, 0x00D700, 0x00D75F, 0x00D787, 0x00D7AF, 0x00D7D7, 0x00D7FF, 0x00FF00, 0x00FF5F, 0x00FF87, 0x00FFAF, 0x00FFD7, 0x00FFFF, 0x121212, 0x1C1C1C, 0x262626, 0x303030, 0x3A3A3A, 0x444444, 0x4E4E4E, 0x585858, 0x005F00, 0x5F0000, 0x5F005F, 0x5F0087, 0x5F00AF, 0x5F00D7, 0x5F00FF, 0x5F5F00, 0x5F5F5F, 0x5F5F87, 0x5F5FAF, 0x5F5FD7, 0x5F5FFF, 0x5F8700, 0x5F875F, 0x5F8787, 0x5F87AF, 0x5F87D7, 0x5F87FF, 0x5FAF00, 0x5FAF5F, 0x5FAF87, 0x5FAFAF, 0x5FAFD7, 0x5FAFFF, 0x5FD700, 0x5FD75F, 0x5FD787, 0x5FD7AF, 0x5FD7D7, 0x5FD7FF, 0x5FFF00, 0x5FFF5F, 0x5FFF87, 0x5FFFAF, 0x5FFFD7, 0x5FFFFF, 0x626262, 0x767676, 0x800000, 0x800080, 0x808000, 0x080808, 0x808080, 0x870000, 0x87005F, 0x870087, 0x8700AF, 0x8700D7, 0x8700FF, 0x875F00, 0x875F5F, 0x875F87, 0x875FAF, 0x875FD7, 0x875FFF, 0x878700, 0x87875F, 0x878787, 0x8787AF, 0x8787D7, 0x8787FF, 0x87AF00, 0x87AF5F, 0x87AF87, 0x87AFAF, 0x87AFD7, 0x87AFFF, 0x87D700, 0x87D75F, 0x87D787, 0x87D7AF, 0x87D7D7, 0x87D7FF, 0x87FF00, 0x87FF5F, 0x87FF87, 0x87FFAF, 0x87FFD7, 0x87FFFF, 0x8A8A8A, 0x949494, 0x9E9E9E, 0xA8A8A8, 0xAF0000, 0xAF005F, 0xAF0087, 0xAF00AF, 0xAF00D7, 0xAF00FF, 0xAF5F00, 0xAF5F5F, 0xAF5F87, 0xAF5FAF, 0xAF5FD7, 0xAF5FFF, 0xAF8700, 0xAF875F, 0xAF8787, 0xAF87AF, 0xAF87D7, 0xAF87FF, 0xAFAF00, 0xAFAF5F, 0xAFAF87, 0xAFAFAF, 0xAFAFD7, 0xAFAFFF, 0x00AFD7, 0xAFD700, 0xAFD75F, 0xAFD787, 0xAFD7AF, 0xAFD7D7, 0xAFD7FF, 0xAFFF00, 0xAFFF5F, 0xAFFF87, 0xAFFFAF, 0xAFFFD7, 0xAFFFFF, 0xB2B2B2, 0xBCBCBC, 0xC0C0C0, 0xC6C6C6, 0xD0D0D0, 0xD70000, 0xD7005F, 0xD70087, 0xD700AF, 0xD700D7, 0xD700FF, 0xD75F00, 0xD75F5F, 0xD75F87, 0xD75FAF, 0xD75FD7, 0xD75FFF, 0xD78700, 0xD7875F, 0xD78787, 0xD787AF, 0xD787D7, 0xD787FF, 0xD7AF00, 0xD7AF5F, 0xD7AF87, 0xD7AFAF, 0xD7AFD7, 0xD7AFFF, 0xD7D700, 0xD7D75F, 0xD7D787, 0xD7D7AF, 0xD7D7D7, 0xD7D7FF, 0xD7FF00, 0xD7FF5F, 0xD7FF87, 0xD7FFAF, 0xD7FFD7, 0xD7FFFF, 0xDADADA, 0xE4E4E4, 0xEEEEEE, 0xFF0000, 0xFF005F, 0xFF0087, 0xFF00AF, 0xFF00D7, 0xFF00FF, 0xFF5F00, 0xFF5F5F, 0xFF5F87, 0xFF5FAF, 0xFF5FD7, 0xFF5FFF, 0xFF8700, 0xFF875F, 0xFF8787, 0xFF87AF, 0xFF87D7, 0xFF87FF, 0xFFAF00, 0xFFAF5F, 0xFFAF87, 0xFFAFAF, 0xFFAFD7, 0xFFAFFF, 0xFFD700, 0xFFD75F, 0xFFD787, 0xFFD7AF, 0xFFD7D7, 0xFFD7FF, 0xFFFF00, 0xFFFF5F, 0xFFFF87, 0xFFFFAF, 0xFFFFD7, 0xFFFFFF
};

#define READ_BUF_SIZE 4096
uint32_t read_stdin(unsigned char **ptr) {
    size_t size = 0;
    ssize_t count = 0;
    size_t cur_size = READ_BUF_SIZE;
    unsigned char* data = malloc(cur_size);

    unsigned char buf[READ_BUF_SIZE];

    int fd = fileno(stdin);

    while((count = read(fd, buf, READ_BUF_SIZE)) > 0) {
        size += count;
        if (size > cur_size) {
            cur_size *= 2;
            data = realloc(data, cur_size);
            if (!data) {
                printf("Error: Could not allocate memory\n");
                exit(1);
            }
        }
        memcpy(data+size-count, buf, count);
    }
    // if count is not zero, some error occured
    if (count < 0) {
        perror("read_stdin");
        exit(1);
    }
    *ptr = data;

    return size;
}

color_yuv_t yuv_color_map[N_COLORS]; ///< palette of the terminal colors in YUV format

void rgb2yuv(const color_t *rgb, color_yuv_t *yuv) {
        float r = rgb->r/255.f, g = rgb->g/255.f, b = rgb->b/255.f;
        yuv->y = .299f*r + .587f*g + .114f*b;
        yuv->u = -.14173f*r + -.28886f*g + .436f*b;
        yuv->v = .615f*r + -.51499f*g + -.10001f*b;
}

float col_yuv_distance(const color_yuv_t *a, const color_yuv_t *b) {
        return SQUARED(b->y - a->y) + SQUARED(b->u - a->u) + SQUARED(b->v - a->v);
}
// convert color to the terminal 256 colors if it have been already converted, fond it in the map First convert to XYZ, then to *L*a*b, finally find the closest color in the map and store it
KHASH_MAP_INIT_INT(uint32_t, uint32_t)
khash_t(uint32_t) *hash_colors;
void init_hash_colors() {
        hash_colors = kh_init(uint32_t);
        int ret;
        khiter_t k;
        color_t rgb;
        for (int i = 0; i < N_COLORS; i++) {
                k = kh_put(uint32_t, hash_colors, color_map[i], &ret);
                kh_value(hash_colors, k) = color_map[i];
                rgb.r = X2R(color_map[i]);
                rgb.g = X2G(color_map[i]);
                rgb.b = X2B(color_map[i]);
                rgb2yuv(&rgb, &yuv_color_map[i]);
        }
}

void free_hash_colors() { kh_destroy(uint32_t, hash_colors); }

typedef struct gif_result_t {
        int delay;
        unsigned char *data;
        struct gif_result_t *next;
} gif_result;

STBIDEF unsigned char *stbi__xload_main(stbi__context *s, int *x, int *y, int *frames, int *channels) {
        unsigned char *result = 0;
        if (stbi__gif_test(s)) {
                stbi__gif g;
                gif_result head;
                gif_result *prev = 0, *gr = &head;

                int stride = 0;
                unsigned char* temp = 0;
                memset(&g, 0, sizeof(g));
                memset(&head, 0, sizeof(head));
                *frames = 0;

                while ((gr->data = stbi__gif_load_next(s, &g, channels, 4, 0))) {
                        if (gr->data == (unsigned char*)s) {
                                gr->data = 0;
                                break;
                        }
                        if (prev) prev->next = gr;
                        gr->delay = g.delay / 10; // the delay has been saved as 1/1000 s in the new stb_image.h
                        prev = gr;
                        gr = (gif_result*) stbi__malloc(sizeof(gif_result));
                        memset(gr, 0, sizeof(gif_result));
                        ++(*frames);

                        {
                          stride = 4 * g.w * g.h;
                          temp = prev->data;
                          prev->data = (unsigned char*)stbi__malloc(stride);
                          memcpy(prev->data, temp, stride);
                        }
                }
                STBI_FREE(g.out);
                STBI_FREE(g.history);
                STBI_FREE(g.background);

                if (gr != &head)
                        STBI_FREE(gr);

                if (*frames > 0) {
                        *x = g.w;
                        *y = g.h;
                }
                result = head.data;
                if (*frames > 1) {
                        unsigned int size = 4 * g.w * g.h;
                        unsigned char *p = 0;

                        result = (unsigned char*)stbi__malloc(*frames * (size + 2));
                        gr = &head;
                        p = result;

                        while (gr) {
                                prev = gr;
                                memcpy(p, gr->data, size);
                                p += size;
                                *p++ = gr->delay & 0xFF;
                                *p++ = (gr->delay & 0xFF00) >> 8;
                                gr = gr->next;

                                STBI_FREE(prev->data);
                                if (prev != &head) STBI_FREE(prev);
                        }
                }
        } else {
                stbi__result_info ri;
                result = stbi__load_main(s, x, y, channels, 0, &ri, 8);
                *frames = !!result;
        }
        return result;
}

STBIDEF unsigned char *stbi_xload(char const *filename, int *x, int *y, int *frames, int *channels) {
        FILE *f;
        stbi__context s;
        unsigned char *result = 0;

        if (!(f = stbi__fopen(filename, "rb")))
                return stbi__errpuc("can't fopen", "Unable to open file");

        stbi__start_file(&s, f);
        result = stbi__xload_main(&s, x, y, frames, channels);
        fclose(f);

        return result;
}

STBIDEF unsigned char *stbi_xload_from_memory(stbi_uc *buffer, int len, int *x, int *y, int *frames, int *channels) {
        stbi__context s;
        stbi__start_mem(&s, buffer, len);
        return stbi__xload_main(&s, x, y, frames, channels);
}

void setPixelGray(color_t *pixel, unsigned char* ptr) {
        pixel->r = pixel->g = pixel->b = ptr[0];
        pixel->a = 255;
}

void setPixelGrayAlpha(color_t *pixel, unsigned char* ptr) {
        pixel->r = pixel->g = pixel->b = ptr[0];
        pixel->a = ptr[1];
}

void setPixelRGB(color_t *pixel, unsigned char* ptr) {
        pixel->r = ptr[0];
        pixel->g = ptr[1];
        pixel->b = ptr[2];
        pixel->a = 255;
}

void setPixelRGBAlpha(color_t *pixel, unsigned char* ptr) {
        pixel->r = ptr[0];
        pixel->g = ptr[1];
        pixel->b = ptr[2];
        pixel->a = ptr[3];
}

void img_load_from_data(image_t *img, stbi_uc* ptr, int w, int h, int frames, int channels) {
        if (ptr && w && h) {
                img->width = w;
                img->height = h;
                img->delays = NULL;
                img->frames = frames;

                if (!(img->pixels = malloc(sizeof(color_t)*w*h * frames))) {
                        perror("malloc error\n");
                        exit(1);
                }

                if (frames > 1 && !(img->delays = malloc(sizeof(uint16_t) * (frames)))) { // avoid buffer overflow
                        perror("malloc error\n");
                        exit(1);
                }
                // fill the array
                void (*pixelSetter)(color_t *pixel, unsigned char* ptr) = &setPixelGray;
                switch (channels) {
                        case 1:
                                pixelSetter = &setPixelGray;
                                break;
                        case 2:
                                pixelSetter = &setPixelGrayAlpha;
                                break;
                        case 3:
                                pixelSetter = &setPixelRGB;
                                break;
                        case 4:
                                pixelSetter = &setPixelRGBAlpha;
                                break;
                }
                for (int frame = 0; frame < frames; frame++) {
                        int offset = frame * (sizeof(unsigned char) * channels * w*h + 2);
                        int i = 0;
                        for (int j = 0; j < w*h; i += channels, j++)
                                pixelSetter(&img->pixels[j + frame*w*h], ptr + i * sizeof(unsigned char) + offset);
                        if (frames > 1) {
                                uint16_t delay = ptr[offset + (1 + i) * sizeof(unsigned char)] << 8;
                                delay += ptr[offset + i * sizeof(unsigned char)];
                                img->delays[frame] = delay;
                        }
                }
                stbi_image_free(ptr);
        } else {
                perror("stb_image error");
                exit(1);
        }
}

void img_load_from_file(image_t *img, const char* file) {
        int channels, w, h, frames;
        unsigned char* ptr = stbi_xload(file, &w, &h, &frames, &channels);
        img_load_from_data(img, ptr, w, h, frames, channels);
}

void img_load_from_stdin(image_t *img) {
        int channels, w, h, frames;
        unsigned char *mem;
        uint32_t len = read_stdin(&mem);
        unsigned char* ptr = stbi_xload_from_memory(mem, len, &w, &h, &frames, &channels);

        free(mem);
        img_load_from_data(img, ptr, w, h, frames, channels);
}

void img_free(image_t *img) {
        free(img->pixels);
        if (img->delays)
                free(img->delays);
}

void img_resize(image_t *img, float wsc, float hsc) {
        color_t *pix = NULL;
        uint32_t w = img->width*wsc,
                 h = img->height*hsc,
                 wh;
        wsc = 1.f/wsc;
        hsc = 1.f/hsc;
        int32_t wsc_i = wsc,
                hsc_i = hsc;
        wh = hsc_i*wsc_i;

        if (!(pix = malloc(sizeof(color_t)*w*h * img->frames))) {
                perror("malloc error\n");
                exit(1);
        }
        for (uint32_t frame = 0; frame < img->frames; frame++) {
                uint32_t src_offset = img->height*img->width * frame,
                         offset = w*h * frame;
                for (uint32_t y = 0; y < h; ++y) {
                        for (uint32_t x = 0; x < w; ++x) {
                                uint32_t r = 0, g = 0, b = 0, a = 0,
                                         srcx = x*wsc,
                                         srcy = y*hsc;
                                for (int32_t yi = 0; yi < hsc_i; ++yi)
                                        for (int32_t xi = 0; xi < wsc_i; ++xi) {
                                                r += img->pixels[src_offset + srcx + xi +(srcy+yi)*img->width].r;
                                                g += img->pixels[src_offset + srcx + xi +(srcy+yi)*img->width].g;
                                                b += img->pixels[src_offset + srcx + xi +(srcy+yi)*img->width].b;
                                                a += img->pixels[src_offset + srcx + xi +(srcy+yi)*img->width].a;
                                        }
                                pix[offset + x+y*w].r = r/wh;
                                pix[offset + x+y*w].g = g/wh;
                                pix[offset + x+y*w].b = b/wh;
                                pix[offset + x+y*w].a = a/wh;
                        }
                }
        }
        free(img->pixels);
        img->pixels = pix;
        img->width = w;
        img->height = h;
}

