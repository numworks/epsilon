#ifndef ESCHER_PALETTE_H
#define ESCHER_PALETTE_H

#include <kandinsky/color.h>

class Palette {
public:
  constexpr static KDColor YellowDark = KDColor::RGB24(0xffb734);
  constexpr static KDColor YellowLight = KDColor::RGB24(0xffcc7b);
  constexpr static KDColor PurpleBright = KDColor::RGB24(0x656975);
  constexpr static KDColor PurpleDark = KDColor::RGB24(0x414147);
  constexpr static KDColor GreyWhite = KDColor::RGB24(0xf5f5f5);
  constexpr static KDColor GreyBright = KDColor::RGB24(0xececec);
  constexpr static KDColor GreyMiddle = KDColor::RGB24(0xd9d9d9);
  constexpr static KDColor GreyDark = KDColor::RGB24(0xa7a7a7);
  constexpr static KDColor Select = KDColor::RGB24(0xd4d7e0);
  constexpr static KDColor WallScreen = KDColor::RGB24(0xf7f9fa);
  constexpr static KDColor SubTab = KDColor::RGB24(0xb8bbc5);
  constexpr static KDColor LowBattery = KDColor::RGB24(0xf30211);
};

#endif
