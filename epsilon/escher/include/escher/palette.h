#pragma once

#include <kandinsky/color.h>

/* TODO: Palette should be hidden once all colors are accessed through a
 * ThemeComponent. */

namespace Escher {

class Palette {
 public:
  constexpr static KDColor White = KDColor::RGB24(0xffffff);
  constexpr static KDColor Black = KDColor::RGB24(0x000000);

  constexpr static KDColor YellowDark = KDColor::RGB24(0xffb734);
  constexpr static KDColor YellowLightBattery = KDColor::RGB24(0xffcc7b);
  constexpr static KDColor YellowLight = KDColor::RGB24(0xffebc7);
  constexpr static KDColor PurpleBright = KDColor::RGB24(0x656975);
  constexpr static KDColor PurpleDark = KDColor::RGB24(0x414147);
  constexpr static KDColor GrayWhite = KDColor::RGB24(0xf5f5f5);
  constexpr static KDColor GrayBright = KDColor::RGB24(0xececec);
  constexpr static KDColor GrayMiddle = KDColor::RGB24(0xd9d9d9);
  constexpr static KDColor GrayDarkMiddle = KDColor::RGB24(0xb8bbc5);
  constexpr static KDColor GrayDark = KDColor::RGB24(0xa7a7a7);
  constexpr static KDColor GrayVeryDark = KDColor::RGB24(0x8c8c8c);
  constexpr static KDColor GrayDarkest = KDColor::RGB24(0x333333);
  constexpr static KDColor Select = KDColor::RGB24(0xd4d7e0);
  constexpr static KDColor SelectDark = KDColor::RGB24(0xb0b8d8);
  constexpr static KDColor WallScreen = KDColor::RGB24(0xf7f9fa);
  constexpr static KDColor WallScreenDark = KDColor::RGB24(0xe0e6ed);
  constexpr static KDColor WallScreenDarker = KDColor::RGB24(0xe4e6e7);
  constexpr static KDColor PopUpTitleBackground = KDColor::RGB24(0x696475);
  constexpr static KDColor LowBattery = KDColor::RGB24(0xf30211);

  // Data colors
  constexpr static KDColor Red = KDColor::RGB24(0xff000c);
  constexpr static KDColor RedLight = KDColor::RGB24(0xffcccc);
  constexpr static KDColor Magenta = KDColor::RGB24(0xff0588);
  constexpr static KDColor MagentaLight = KDColor::RGB24(0xffd9eb);
  constexpr static KDColor Turquoise = KDColor::RGB24(0x60c1ec);
  constexpr static KDColor TurquoiseLight = KDColor::RGB24(0xd0edff);
  constexpr static KDColor Pink = KDColor::RGB24(0xff99aa);
  constexpr static KDColor PinkLight = KDColor::RGB24(0xffe3e7);
  constexpr static KDColor Blue = KDColor::RGB24(0x5075f2);
  constexpr static KDColor BlueLight = KDColor::RGB24(0xdce3fd);
  constexpr static KDColor Orange = KDColor::RGB24(0xfe871f);
  constexpr static KDColor OrangeLight = KDColor::RGB24(0xffc89a);
  constexpr static KDColor Green = KDColor::RGB24(0x50c102);
  constexpr static KDColor GreenLight = KDColor::RGB24(0xdcf3cc);
  constexpr static KDColor Brown = KDColor::RGB24(0x8d7350);
  constexpr static KDColor Purple = KDColor::RGB24(0x6e2d79);
  constexpr static KDColor BlueishGray = KDColor::RGB24(0x919ea4);
  constexpr static KDColor Cyan = KDColor::RGB24(0x00ffff);
  constexpr static KDColor Violet = KDColor::RGB24(0x8d37e9);
  constexpr static KDColor VioletLight = KDColor::RGB24(0xd7b3ff);
  constexpr static KDColor Mint = KDColor::RGB24(0x00c6b1);
  constexpr static KDColor MintLight = KDColor::RGB24(0xb1fef7);

  // Dark theme colors
  constexpr static KDColor DARK_VioletDeep = KDColor::RGB24(0x733FFF);
  constexpr static KDColor DARK_VioletMiddle = KDColor::RGB24(0x9296F6);
  constexpr static KDColor DARK_VioletPale = KDColor::RGB24(0xC3CAF4);
  constexpr static KDColor DARK_AxisMain = KDColor::RGB24(0xDEDADE);
  constexpr static KDColor DARK_AxisSecondary = KDColor::RGB24(0x636363);
  // Dark theme data colors
  constexpr static KDColor DARK_Red = KDColor::RGB24(0xFF6B70);
  constexpr static KDColor DARK_Blue = KDColor::RGB24(0x7B96F9);
  constexpr static KDColor DARK_Green = KDColor::RGB24(0x4AB300);
  constexpr static KDColor DARK_Yellow = KDColor::RGB24(0xFFB531);
  constexpr static KDColor DARK_Magenta = KDColor::RGB24(0xFF6BBA);
  constexpr static KDColor DARK_Turquoise = KDColor::RGB24(0xA6DAF2);
  constexpr static KDColor DARK_Pink = KDColor::RGB24(0xFFD6DC);
  constexpr static KDColor DARK_Orange = KDColor::RGB24(0xFFA352);

  // Python red is the same as Red
  constexpr static KDColor PYTHON_Red = KDColor::RGB24(0xff000c);
  constexpr static KDColor PYTHON_KhakiGray = KDColor::RGB24(0x999988);
  constexpr static KDColor PYTHON_DarkCyan = KDColor::RGB24(0x009999);
  constexpr static KDColor PYTHON_RaspberryRed = KDColor::RGB24(0xd73a49);
  constexpr static KDColor PYTHON_DarkBlue = KDColor::RGB24(0x032f62);
  // The Python gray is between GrayMiddle and GrayDarkMiddle
  constexpr static KDColor PYTHON_GrayMiddle = KDColor::RGB24(0xC6C6C6);
};

}  // namespace Escher
