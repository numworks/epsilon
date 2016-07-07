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

#include <kandinsky.h>

void ion_screen_push_rect(KDRect rect, const KDColor * pixels);
void ion_screen_push_rect_uniform(KDRect rect, KDColor color);
void ion_screen_pull_rect(KDRect rect, KDColor * pixels);
/*
void ion_screen_set_working_area(KDRect area);
void ion_screen_push_pixels(const KDColor * pixels, size_t count);
void ion_screen_pull_pixels(KDColor * pixels, size_t count);
*/

#define ION_SCREEN_WIDTH 320
#define ION_SCREEN_HEIGHT 240

#endif
