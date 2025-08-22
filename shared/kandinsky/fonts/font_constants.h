#pragma once

#ifdef __cplusplus
constexpr
#endif
#if KANDINSKY_FONT_MONOCHROME
    int k_grayscaleBitsPerPixel = 1;
#else
int k_grayscaleBitsPerPixel = 4;
#endif
