#include <kandinsky/color.h>

KDColor KDColor::blend(const KDColor fg, const KDColor bg, const uint8_t blending) {
  /* This function is a hot path since it's being called for every single pixel
   * whenever we want to display a string. In this context, we're quite often
   * calling it with a value of either 0 or 0xFF, which can be very trivially
   * dealt with. So let's make a special case for them. */

  // the most common case => return foreground
  if(blending == 0xFF) {
    return fg;
  }

  // the less common case => return background
  if(blending == 0x00) {
    return bg;
  }

  // What we want to do is:
  //
  // color = (foreground * blending) + (background * (1 - blending))
  //
  // so :
  //
  // color = background + ((foreground - background) * blending)
  //
  // using 32-bits data because the cortex-m4 registers are 32-bits -> fast

  const uint32_t alpha = blending + 1; // '+ 1' is important for accuracy

  const uint32_t fg_r5 = (fg.m_value & 0xF800); // extract red
  const uint32_t fg_g6 = (fg.m_value & 0x07E0); // extract green
  const uint32_t fg_b5 = (fg.m_value & 0x001F); // extract blue

  const uint32_t bg_r5 = (bg.m_value & 0xF800); // extract red
  const uint32_t bg_g6 = (bg.m_value & 0x07E0); // extract green
  const uint32_t bg_b5 = (bg.m_value & 0x001F); // extract blue

  const uint32_t color = ((bg_r5 + (((fg_r5 - bg_r5) * alpha) >> 8)) & 0xF800)
                       | ((bg_g6 + (((fg_g6 - bg_g6) * alpha) >> 8)) & 0x07E0)
                       | ((bg_b5 + (((fg_b5 - bg_b5) * alpha) >> 8)) & 0x001F);

  return KDColor(color);
}
