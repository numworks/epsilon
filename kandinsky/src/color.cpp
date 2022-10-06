#include <kandinsky/color.h>
#include <cmath>
#include <algorithm>
#include <assert.h>

KDColor KDColor::Blend(KDColor first, KDColor second, uint8_t alpha) {
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

KDColor KDColor::HSVBlend(KDColor color1, KDColor color2) {
  HSVColor HSVcolor1 = color1.convertToHSV();
  HSVColor HSVcolor2 = color2.convertToHSV();

  double meanS = (HSVcolor1.S + HSVcolor2.S) / 2.0;
  double meanV = (HSVcolor1.V + HSVcolor2.V) / 2.0;

  double Hmin = std::min(HSVcolor1.H, HSVcolor2.H);
  double Hmax = std::max(HSVcolor1.H, HSVcolor2.H);
  double meanH;
  if (Hmax - Hmin < 180.0) {
    meanH = (Hmax + Hmin) / 2.0;
  } else {
    /* This handles the case where the mean angle H is not
     * between Hmin and Hmax. For example if Hmin = 358° and
     * Hmax = 6°, Hmean = 2°. */
    double rotatedHmax = std::fmod(Hmax + 180.0, 360.0);
    double rotatedHmin = std::fmod(Hmin + 180.0, 360.0);
    double rotatedMean = (rotatedHmax + rotatedHmin) / 2.0;
    meanH = std::fmod(rotatedMean + 180.0, 360.0);
  }

  assert(0.0 <= meanH && meanH < 360.0);
  assert(0.0 <= meanS && meanS <= 1.0);
  assert(0.0 <= meanV && meanV <= 255.0);
  return ConvertHSVToRGB(HSVColor({meanH, meanS, meanV}));
}

KDColor::HSVColor
#if PLATFORM_DEVICE
// Needed because compiler does not respect procedure call standards
  __attribute__((pcs("aapcs-vfp")))
#endif
                  KDColor::convertToHSV() const {
  double R = static_cast<double>(red());
  double G = static_cast<double>(green());
  double B = static_cast<double>(blue());

  double V = std::max(std::max(R, G), B);
  double m = std::min(std::min(R, G), B);

  double delta = V - m;
  double S = V > 0 ? delta / V : 0.0;

  double H;
  if (delta == 0) {
    H = 0.0;
  } else if (V == R) {
    H = 60 * (G - B) / delta;
  } else if (V == G) {
    H = 120.0 + 60.0 * (B - R) / delta;
  } else {
    H = 240.0 + 60.0 * (R - G) / delta;
  }
  // Make H positive
  H = (std::fmod((H + 360.0), 360.0));

  assert(0.0 <= H && H < 360.0);
  assert(0.0 <= S && S <= 1.0);
  assert(0.0 <= V && V <= 255.0);
  return HSVColor({H, S, V});
}

KDColor KDColor::ConvertHSVToRGB(KDColor::HSVColor color) {
  double H = color.H;
  double S = color.S;
  double V = color.V;

  assert(0.0 <= H && H < 360.0);
  assert(0.0 <= S && S <= 1.0);
  assert(0.0 <= V && V <= 255.0);

  double alpha = V * (1 - S);
  double beta = V * (1 - S * std::fabs(std::fmod(H / 60.0, 2.0) - 1));

  assert(0.0 <= alpha && alpha <= 255.0);
  assert(0.0 <= beta && beta <= 255.0);

  uint8_t intV = static_cast<uint8_t>(std::round(V));
  uint8_t intAlpha = static_cast<uint8_t>(std::round(alpha));
  uint8_t intBeta = static_cast<uint8_t>(std::round(beta));

  if (H < 60.0) {
    return KDColor::RGB888(intV, intBeta, intAlpha);
  }
  if (H < 120.0) {
    return KDColor::RGB888(intBeta, intV, intAlpha);
  }
  if (H < 180.0) {
    return KDColor::RGB888(intAlpha, intV, intBeta);
  }
  if (H < 240.0) {
    return KDColor::RGB888(intAlpha, intBeta, intV);
  }
  if (H < 300.0) {
    return KDColor::RGB888(intBeta, intAlpha, intV);
  }
  if (H < 360.0) {
    return KDColor::RGB888(intV, intAlpha, intBeta);
  }
  assert(false);
  return KDColorBlack;
}
