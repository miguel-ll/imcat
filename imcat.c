#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "sh_image.h"
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>

#define USAGE "Usage: imcat [-hct] [-w width] [-l loops] [-r resolution] file\n\n" \
  "  -h: Displays this message. \n"                                      \
  "  -w: Terminal width/columns by default.\n"                           \
  "  -l: Loops are only useful with GIF files. A value of 1 means that the GIF will" \
  "be displayed twice because it loops once. A negative value means infinite looping.\n"                                                           \
  "  -r: Resolution must be 1 or 2. By default imcat checks for unicode support to use higher resolution.\n" \
// transparency threshold -- all pixels with alpha below 25%.
#define TRANSP_ALPHA 64

extern char *optarg;
extern int optind, optopt, opterr, optreset;

// for <C-C>
volatile int loops = -1, loop = -1;
volatile char stop = 0;

uint32_t terminal_columns() {
    struct winsize win;
    ioctl(1, TIOCGWINSZ, &win);
    return win.ws_col;
}

uint32_t terminal_rows() {
    struct winsize win;
    ioctl(1, TIOCGWINSZ, &win);
    return win.ws_row;
}

void intHandler() {
    loops = loop;
    stop = 1;
}

int getopt(int argc, char * const argv[], const char *optstring);

char supportsUTF8() {
    const char* LC_ALL = getenv("LC_ALL");
    const char* LANG = getenv("LANG");
    const char* LC_CTYPE = getenv("LC_CTYPE");
    const char* UTF = "UTF-8";
    return (LC_ALL && strstr(LC_ALL, UTF)) || (LANG && strstr(LANG, UTF)) || (LC_CTYPE && strstr(LC_CTYPE, UTF));
}

int main(int argc, char *argv[]) {
    init_hash_colors();
    image_t img;
    char *file;
    char *num;
    int c;
    opterr = 0;

    uint32_t cols = 0, rows = 0, precision = 0;
    uint32_t max_cols = 0, max_rows = 0;
    float scale_cols = 0, scale_rows = 0;

    while ((c = getopt (argc, argv, "w:l:r:ct")) != -1)
        switch (c) {
            case 'w':
                cols = strtol(optarg, &num, 0) >> 1;
                break;
            case 'l':
                loops = strtol(optarg, &num, 0);
                break;
            case 'r':
                precision = strtol(optarg, &num, 0);
                break;
            default:
                printf(USAGE);
                exit(1);
                break;
        }
    if (argc > 1)
        file = argv[argc-1];
    else {
        printf(USAGE);
        exit(1);
    }
    if (precision == 0 || precision > 2) {
        if (supportsUTF8())
            precision = 2;
        else
            precision = 1;
    }
    // if precision is 2 we can use the terminal full width/height. Otherwise we can only use half
    max_cols = terminal_columns() / (2 / precision);
    max_rows = terminal_rows() * precision;

    if (strcmp(file, "-") == 0) {
        img_load_from_stdin(&img);
    } else {
        img_load_from_file(&img, file);
    }
    if (cols == 0 && rows == 0) {
        scale_cols = max_cols / (float)img.width;
        scale_rows = max_rows / (float)img.height;
        if (scale_rows < scale_cols && max_rows < img.height)
            img_resize(&img, scale_rows, scale_rows);
        else if (max_cols < img.width)
            img_resize(&img, scale_cols, scale_cols);
    } else if (cols > 0 && cols < img.width) {
        scale_cols = cols / (float)img.width;
        img_resize(&img, scale_cols, scale_cols);
     } else if (rows > 0 && rows < img.height) {
        scale_rows = rows / (float)img.height;
        img_resize(&img, scale_rows, scale_rows);
    }
    if (img.frames > 1) { //for GIF
        system("clear");
        signal(SIGINT, intHandler);
    } else {
        loops = 0;
    }
    // save the cursor position and hide it
    printf("\e[s\e[?25l");
    while (loop++ < loops || loops < 0) {
        uint32_t offset = 0;
        for (uint32_t frame = 0; frame < img.frames; frame++) {
            if (frame > 0 || loop > 0) {
                if (frame > 0)
                    usleep(img.delays[frame - 1] * 10000);
                else
                    usleep(img.delays[img.frames - 1] * 10000);
                printf("\e[u");
            }
            uint32_t index, x, y;
            for (y = 0; y < img.height; y += precision) {
                for (x = 0; x < img.width; x++) {
                    index = y * img.width + x + offset;
                    const color_t* upperPixel = &img.pixels[index];
                    if (precision == 2) {
                        if (y < img.height - 1) {
                            const color_t* lowerPixel = &img.pixels[index + img.width];
                            if (upperPixel->a < TRANSP_ALPHA) { // first pixel is transparent
                                if (lowerPixel->a < TRANSP_ALPHA)
                                    printf("\e[m ");
                                else
                                        printf("\x1b[0;38;2;%d;%d;%dm\u2584", lowerPixel->r, lowerPixel->g, lowerPixel->b);
                            } else {
                                if (lowerPixel->a < TRANSP_ALPHA)
                                        printf("\x1b[0;38;2;%d;%d;%dm\u2580", upperPixel->r, upperPixel->g, upperPixel->b);
                                else
                                        printf("\x1b[48;2;%d;%d;%dm\x1b[38;2;%d;%d;%dm\u2584", upperPixel->r, upperPixel->g, upperPixel->b, lowerPixel->r, lowerPixel->g, lowerPixel->b);
                            }
                        } else { // this is the last line
                            if (upperPixel->a < TRANSP_ALPHA)
                                printf("\e[m ");
                            else
                                    printf("\x1b[0;38;2;%d;%d;%dm\u2580", upperPixel->r, upperPixel->g, upperPixel->b);
                        }
                    } else {
                        if (img.pixels[index].a < TRANSP_ALPHA)
                            printf("\e[m  ");
                        else
                                printf("\x1b[0;48;2;%d;%d;%dm  ", img.pixels[index].r, img.pixels[index].g, img.pixels[index].b);
                    }
                }
                printf("\e[m\n");
            }
            offset += img.width * img.height;
            if (stop) frame = img.frames;
        }
    }
    printf("\e[?25h"); //display the cursor again

    img_free(&img);
    free_hash_colors();
    return 0;
}

