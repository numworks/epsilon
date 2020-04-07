#ifndef ESCHER_PALETTE_H
#define ESCHER_PALETTE_H

#include <kandinsky/color.h>
#include <stddef.h>

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
  constexpr static KDColor GreyVeryDark = KDColor::RGB24(0x8c8c8c);
  constexpr static KDColor Select = KDColor::RGB24(0xd4d7e0);
  constexpr static KDColor SelectDark = KDColor::RGB24(0xb0b8d8);
  constexpr static KDColor WallScreen = KDColor::RGB24(0xf7f9fa);
  constexpr static KDColor WallScreenDark = KDColor::RGB24(0xe0e6ed);
  constexpr static KDColor SubTab = KDColor::RGB24(0xb8bbc5);
  constexpr static KDColor LowBattery = KDColor::RGB24(0xf30211);
  constexpr static KDColor Red = KDColor::RGB24(0xff000c);
  constexpr static KDColor RedLight = KDColor::RGB24(0xfe6363);
  constexpr static KDColor Magenta = KDColor::RGB24(0xff0588);
  constexpr static KDColor Turquoise = KDColor::RGB24(0x60c1ec);
  constexpr static KDColor Pink = KDColor::RGB24(0xffabb6);
  constexpr static KDColor Blue = KDColor::RGB24(0x5075f2);
  constexpr static KDColor BlueLight = KDColor::RGB24(0x718fee);
  constexpr static KDColor Orange = KDColor::RGB24(0xfe871f);
  constexpr static KDColor Green = KDColor::RGB24(0x50c102);
  constexpr static KDColor GreenLight = KDColor::RGB24(0x52db8f);
  constexpr static KDColor Brown = KDColor::RGB24(0x8d7350);
  constexpr static KDColor Purple = KDColor::RGB24(0x6e2d79);
  constexpr static KDColor DataColor[] = {Red, Blue, Green, YellowDark, Magenta, Turquoise, Pink, Orange};
  constexpr static KDColor DataColorLight[] = {RedLight, BlueLight, GreenLight, YellowLight};

  constexpr static size_t numberOfDataColors() { return sizeof(DataColor)/sizeof(KDColor); }
  constexpr static size_t numberOfLightDataColors() { return sizeof(DataColorLight)/sizeof(KDColor); }
  static KDColor nextDataColor(int * colorIndex);
};

#endif
