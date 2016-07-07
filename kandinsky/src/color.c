#include <kandinsky/color.h>

#define KDAlphaBlend(c1,c2,a) ((uint8_t)((((uint16_t)c1*(0xFF-a))+((uint16_t)c2*a))/0xFF))

KDColor KDColorBlend(KDColor a, KDColor b, uint8_t alpha) {
  return KDColorRGB(
      KDAlphaBlend(KDColorRedComponent(a), KDColorRedComponent(b), alpha),
      KDAlphaBlend(KDColorGreenComponent(a), KDColorGreenComponent(b), alpha),
      KDAlphaBlend(KDColorBlueComponent(a), KDColorBlueComponent(b), alpha)
      );
}
