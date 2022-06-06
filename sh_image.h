#ifndef __SH_IMAGE_H__
#define __SH_IMAGE_H__
/*******************************************************************************
 *  sh_image                                                                   *
 *  Wed Dec 18 CET 2013                                                        *
 *  Copyright Eduardo San Martin Morote                                        *
 *  eduardo.san-martin-morote@ensimag.fr                                       *
 *  http://posva.net                                                           *
 ******************************************************************************/
#include <stdint.h>
#include <stdint.h>
/**
 * @defgroup sh_color sh_color
 * Manipulation of pixels
 * @{
 */
/**
 * @brief Structure defining a color in RGB format
 */
typedef struct {
        uint8_t r, ///< Red
                g, ///< Green
                b, ///< Blue
                a; ///< Alpha
} color_t;

/**
 * @brief Structure defining a color in YUV format
 */
typedef struct {
        float y, ///< Y'
              u, ///< u
              v; ///< v
} color_yuv_t;

#define N_COLORS 247 ///< Number of colors in terminal
//uint32_t color_map[N_COLORS]; ///< palette of the terminal colors in RGB 0x00RRGGBB
// yuv equivalents
//color_yuv_t yuv_color_map[N_COLORS]; ///< palette of the terminal colors in YUV format
/**
 * @brief Convert a color from RGB to YUV
 * @param rgb color in rgb
 * @param yuv returned conversion
 */
void rgb2yuv(const color_t *rgb, color_yuv_t *yuv);
/**
 * @brief Squared distance between 2 YUV colors
 *
 * @param a from color
 * @param b to color
 * @return the distance
 */
float col_yuv_distance(const color_yuv_t *a, const color_yuv_t *b);

/** @brief Initialize the hastable for the colors */
void init_hash_colors();
/** @brief Free the hashtables for the colors */
void free_hash_colors();

#define SQUARED(x) ((x)*(x)) ///< x*x
#define RGB2X(r,g,b) (((r) << 16) | ((g) << 8) | (b)) ///< rgb in uint32_t as 0x00RRGGBB
#define X2R(x) (((x) & 0xff0000) >> 16) ///< Extract Red from uint32_t
#define X2G(x) (((x) & 0xff00) >> 8) ///< Extract Green from uint32_t
#define X2B(x) ((x) & 0xff) ///< Extract Blue from uint32_t
uint32_t read_stdin(unsigned char **ptr);

/**
 * @defgroup sh_image sh_image
 * This module allows to load pictures from these formats:
 *
 *    <ul>
 *    <li>JPEG baseline (no JPEG progressive)</li>
 *
 *    <li>PNG 8-bit only</li>
 *
 *    <li>TGA (not sure what subset, if a subset)</li>
 *
 *    <li>BMP non-1bpp, non-RLE</li>
 *
 *    <li>PSD (composited view only, no extra channels)</li>
 *
 *    <li>GIF (*comp always reports as 4-channel)</li>
 *
 *    <li>HDR (radiance rgbE format)</li>
 *
 *    <li>PIC (Softimage PIC)</li>
 *    </ul>
 *
 * It also implements the needed functions to resize, copy and convert the
 * colors of an image
 * @{
 */

/**
 * @brief Structure to handle images
 */
typedef struct {
        color_t *pixels; ///< 1 dim pixels pixels[x+y*w]
        uint32_t width, ///< Width of the image
                 height, ///< Height of the image
                 frames; ///< Number of frames
        uint16_t *delays; ///< Array of delays. Length = frames - 1
} image_t;

/**
 * @brief Load an image from a file
 *
 * @param img instance
 * @param file file to load
 */
void img_load_from_file(image_t *img, const char* file);

/**
 * @brief Load an image from stdin
 *
 * @param img instance
 */
void img_load_from_stdin(image_t *img);

/** @brief Free the ressources for an image @param img instance affected */
void img_free(image_t *img);

/**
 * @brief Resize an image using the nearest color
 *
 * @param img instance affected
 * @param wsc horizontal scale
 * @param hsc vertical scale
 */
void img_resize(image_t *img, float wsc, float hsc);

#endif

