#pragma once

#include <escher/palette.h>

#include "theme_container.h"

namespace Escher {

constexpr static ThemeContainer k_lightTheme =
    ThemeContainer{.WallScreen = Palette::WallScreen,
                   .WallScreenDark = Palette::WallScreenDark,
                   .BackgroundWhite = Palette::White,
                   .BackgroundPopup = Palette::Black,
                   .SurfaceSelect = Palette::Select,
                   .BannerYellow = Palette::YellowDark,
                   .BannerGrayWhite = Palette::GrayWhite,
                   .BannerGrayMiddle = Palette::GrayMiddle,
                   .TextDefault = Palette::Black,
                   .TextGrayDark = Palette::GrayDark,
                   .TextGrayDarkMiddle = Palette::GrayDarkMiddle,
                   .TextGrayVeryDark = Palette::GrayVeryDark,
                   .TextWhite = Palette::White,
                   .TextPurple = Palette::PurpleBright,
                   .TextBluishGray = Palette::BlueishGray,
                   .TokenWhite = Palette::White,
                   .TokenYellow = Palette::YellowDark,
                   .TokenRed = Palette::Red,
                   .TokenBlack = Palette::Black,
                   .TokenGrayMiddle = Palette::GrayMiddle,
                   .TokenGrayBright = Palette::GrayBright,
                   .TokenGrayDark = Palette::GrayDark};
}  // namespace Escher
