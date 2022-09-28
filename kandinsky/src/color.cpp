#include <kandinsky/color.h>
#include <cmath>
#include <algorithm>

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

static double ConvertDegreeRadian(double x, bool degreeToRadian) {
  return x * (degreeToRadian ? M_PI / 180.0 : 180.0 / M_PI);
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
    double rotatedHmax = std::fmod(Hmax + 180.0, 360.0);
    double rotatedHmin = std::fmod(Hmin + 180.0, 360.0);
    double rotatedMean = (rotatedHmax + rotatedHmin) / 2.0;
    meanH = std::fmod(rotatedMean + 180.0, 360.0);
  }

  return ConvertHSVToRGB(HSVColor({meanH, meanS, meanV}));
}

KDColor::HSVColor KDColor::convertToHSV() const {
  double R = static_cast<double>(red());
  double G = static_cast<double>(green());
  double B = static_cast<double>(blue());

  double M = std::max(std::max(R, G), B);
  double m = std::min(std::min(R, G), B);

  double V = M / 255.0;
  double S = M > 0 ? 1.0 - m / M : 0.0;

  double div = ConvertDegreeRadian(std::acos((R - 0.5 * G - 0.5 * B) / std::sqrt(R * R + G * G + B * B - R * G - R * B - G * B)), false);
  double H = G < B ? 360.0 - div : div;

  return HSVColor({H, S, V});
}

KDColor KDColor::ConvertHSVToRGB(KDColor::HSVColor color) {
  double H = color.H;
  double S = color.S;
  double V = color.V;

  double M = 255.0 * V;
  double m = M * (1 - S);

  double z = (M - m) * (1.0 - std::fabs(std::fmod((H / 60.0), 2.0) - 1.0));

  uint8_t intM = static_cast<uint8_t>(M);
  uint8_t intm = static_cast<uint8_t>(m);
  uint8_t intzm = static_cast<uint8_t>(z + m);
  if (H < 60) {
    return KDColor::RGB888(intM, intzm, intm);
  }
  if (H < 120) {
    return KDColor::RGB888(intzm, intM, intm);
  }
  if (H < 180) {
    return KDColor::RGB888(intm, intM, intzm);
  }
  if (H < 240) {
    return KDColor::RGB888(intm, intzm, intM);
  }
  if (H < 300) {
    return KDColor::RGB888(intzm, intm, intM);
  }
  if (H < 360) {
    return KDColor::RGB888(intM, intm, intzm);
  }
  return KDColorBlack;
}
