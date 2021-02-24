#include <kandinsky/color.h>

KDColor KDColor::blend(KDColor first, KDColor second, uint8_t alpha) {
  /* This function is a hot path since it's being called for every single pixel
   * whenever we want to display a string. In this context, we're quite often
   * calling it with a value of either 0 or 0xFF, which can be very trivially
   * dealt with. Similarly, blending the same two colors yields a trivial
   * result and can be bypassed. Let's make a special case for them. */
  if (alpha == 0) {
    return second;
  }
  if (alpha == 0xFF) {
    return first;
  }
  if (first == second) {
    return first;
  }

  // We want to do first*alpha + second*(1-alpha)
  // First is RRRRR GGGGGG BBBBB
  // Second is same

  uint16_t oneMinusAlpha = 0x100-alpha;
  uint16_t red = first.red()*alpha + second.red()*oneMinusAlpha;
  uint16_t green = first.green()*alpha + second.green()*oneMinusAlpha;
  uint16_t blue = first.blue()*alpha + second.blue()*oneMinusAlpha;
  return RGB888(red>>8, green>>8, blue>>8);
}
