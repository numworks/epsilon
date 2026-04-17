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
                   .SurfaceSelect = Palette::DARK_VioletLight,
                   .BannerYellow = Palette::DARK_VioletLight,
                   .BannerGrayWhite = Palette::DARK_VioletMiddle,
                   .BannerGrayMiddle = Palette::DARK_VioletMiddle,
                   .TextDefault = Palette::DARK_VioletLight,
                   .TextGrayDark = Palette::DARK_VioletMiddle,
                   .TextGrayDarkMiddle = Palette::DARK_VioletMiddle,
                   .TextGrayVeryDark = Palette::DARK_VioletLight,
                   .TextWhite = Palette::White,
                   .TextPurple = Palette::DARK_VioletLight,
                   // TODO: check BluishGray mapping (if any)
                   .TextBluishGray = Palette::DARK_VioletMiddle,
                   .AxisBlack = Palette::DARK_AxisMain,
                   .AxisLight = Palette::DARK_AxisSecondary,
                   .AxisMedium = Palette::DARK_AxisSecondary,
                   .AxisGrayBright = Palette::DARK_AxisSecondary,
                   .AxisGrayDark = Palette::DARK_AxisSecondary,
                   .AxisGrayVeryDark = Palette::DARK_AxisSecondary,
                   .AxisRed = Palette::Red,
                   .TokenWhite = Palette::Black,
                   .TokenYellow = Palette::DARK_VioletDeep,
                   // TODO: confirm TokenRed and TokenPurple
                   .TokenRed = Palette::DARK_VioletDeep,
                   .TokenBlack = Palette::DARK_VioletLight,
                   .TokenGrayMiddle = Palette::DARK_VioletLight,
                   .TokenGrayBright = Palette::DARK_VioletMiddle,
                   .TokenGrayDarkest = Palette::DARK_VioletDeep,
                   .TokenGrayDark = Palette::DARK_VioletDeep};

}  // namespace Escher
