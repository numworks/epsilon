#ifndef ION_SCREEN_H
#define ION_SCREEN_H

/* ION abstracts pushing pixels to the screen.
 *
 * There could be a single entry point, set_pixel, but setting pixels one by one
 * incurs quite a large overhead because you need to send the coordinate of each
 * pixel to the screen.
 *
 * Many displays support sending contiguous pixels without having to repeat the
 * pixel coordinates every time. We're therefore leveraging this capability
 * which results in a very consequent speedup (up to ~10x faster). */

#include <stdint.h>

/* ION manipulates RGB565 colors */
typedef uint16_t ion_color_t;

/* Set the color of a single pixel. */
void ion_set_pixel(uint16_t x, uint16_t y, ion_color_t color);

/* Fill a rect with a single color */
void ion_fill_rect(
    uint16_t x, uint16_t y,
    uint16_t width, uint16_t height,
    ion_color_t color
);

/* Fill a rect with a color buffer */
void ion_fill_rect_from_buffer(
    uint16_t x, uint16_t y,
    uint16_t width, uint16_t height,
    ion_color_t * buffer
);

#define ION_SCREEN_WIDTH 320
#define ION_SCREEN_HEIGHT 240

#endif
