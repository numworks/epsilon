#ifndef KANDINSKY_FONTS_FONT_CONSTANTS_H
#define KANDINSKY_FONTS_FONT_CONSTANTS_H

#ifdef __cplusplus
constexpr
#endif
#if KANDINSKY_FONT_MONOCHROME
    int k_grayscaleBitsPerPixel = 1;
#else
int k_grayscaleBitsPerPixel = 4;
#endif

#endif
