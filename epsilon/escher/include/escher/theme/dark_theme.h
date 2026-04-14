#pragma once

#include <escher/palette.h>

#include "theme_container.h"

namespace Escher {

constexpr static ThemeContainer k_darkTheme =
    ThemeContainer{.WallScreen = Palette::Black,
                   .WallScreenDark = Palette::Black,
                   .BackgroundWhite = Palette::Black,
                   // TODO: check color of popups (black in light theme)
                   .BackgroundPopup = Palette::White,
                   // TODO: add dark theme colors to palette
                   .SurfaceSelect = KDColor::RGB24(0xc3cAF4),
                   .BannerYellow = KDColor::RGB24(0xC3CAF4),
                   .BannerGrayWhite = KDColor::RGB24(0x9296F6),
                   .BannerGrayMiddle = KDColor::RGB24(0x9296F6),
                   .TextDefault = KDColor::RGB24(0xC3CAF4),
                   .TextGrayDark = KDColor::RGB24(0x9296F6),
                   .TextGrayDarkMiddle = KDColor::RGB24(0x9296F6),
                   .TextGrayVeryDark = KDColor::RGB24(0xC3CAF4),
                   .TextWhite = Palette::White,
                   .TextPurple = KDColor::RGB24(0xC3CAF4),
                   // TODO: check BluishGray mapping (if any)
                   .TextBluishGray = KDColor::RGB24(0x9296F6),
                   .TokenWhite = Palette::Black,
                   .TokenYellow = KDColor::RGB24(0x733FFF),
                   // TODO: confirm TokenRed and TokenPurple
                   .TokenRed = KDColor::RGB24(0x733FFF),
                   .TokenBlack = KDColor::RGB24(0xC3CAF4),
                   .TokenGrayMiddle = KDColor::RGB24(0xc3cAF4),
                   .TokenGrayBright = KDColor::RGB24(0x9296F6),
                   .TokenGrayDark = KDColor::RGB24(0x733FFF),
                   .DataRed = Palette::Red,
                   .DataBlue = Palette::Blue,
                   .DataGreen = Palette::Green,
                   .DataYellow = Palette::YellowDark,
                   .DataMagenta = Palette::Magenta,
                   .DataTurquoise = Palette::Turquoise,
                   .DataPink = Palette::Pink,
                   .DataOrange = Palette::Orange,
                   .DataViolet = Palette::Violet,
                   .DataMint = Palette::Mint};

}  // namespace Escher
