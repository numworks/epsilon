#pragma once

#include <escher/palette.h>

#include "theme_container.h"

namespace Escher {

constexpr static ThemeContainer k_darkTheme = ThemeContainer{
    .WallScreen = Palette::Black,
    .WallScreenDark = Palette::Black,
    .BackgroundWhite = Palette::Black,
    // TODO: check color of popups (black in light theme)
    .BackgroundPopup = Palette::White,
    .BackgroundPopUpTitle = Palette::PopUpTitleBackground,
    .SurfaceSelect = Palette::DARK_VioletLight,
    .SurfaceSelectDark = Palette::DARK_VioletLight,
    .BannerYellow = Palette::DARK_VioletLight,
    .BannerPurple = Palette::DARK_VioletMiddle,
    .BannerGrayWhite = Palette::DARK_VioletMiddle,
    .BannerGrayMiddle = Palette::DARK_VioletMiddle,
    .TextDefault = Palette::DARK_VioletLight,
    .TextTables = Palette::White,
    .TextBanner = Palette::Black,
    .TextSelect = Palette::Black,
    // TODO: should selected gray texts be another color than Black?
    .TextSelectGrayDark = Palette::Black,
    .TextSelectGrayVeryDark = Palette::Black,
    .TextSelectPurple = Palette::Black,
    .TextGrayDark = Palette::DARK_VioletMiddle,
    .TextGrayDarkMiddle = Palette::DARK_VioletMiddle,
    .TextGrayVeryDark = Palette::DARK_VioletLight,
    .TextWhite = Palette::Black,
    .TextPurple = Palette::DARK_VioletLight,
    // TODO: check BluishGray mapping (if any)
    .TextBluishGray = Palette::DARK_VioletMiddle,
    .BorderWhite = Palette::DARK_VioletDeep,
    .BorderWallScreen = Palette::DARK_VioletDeep,
    .BorderGrayMiddle = Palette::DARK_VioletDeep,
    .BorderGrayWhite = Palette::DARK_VioletMiddle,
    .BorderGrayBright = Palette::DARK_VioletMiddle,
    .BorderGrayDarkMiddle = Palette::Black,
    .BorderGrayDark = Palette::Black,
    .BorderPurple = Palette::Black,
    .BorderPurpleDark = Palette::Black,
    .BorderYellowDark = Palette::Black,
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
    .TokenGrayWhite = Palette::DARK_VioletLight,
    .TokenGrayMiddle = Palette::DARK_VioletLight,
    .TokenGrayBright = Palette::DARK_VioletMiddle,
    .TokenGrayDarkest = Palette::DARK_VioletDeep,
    .TokenGrayDark = Palette::DARK_VioletDeep,
    // TODO: decide on battery logo colors
    .TokenBatteryLevel = Palette::YellowLightBattery,
    .TokenLowBattery = Palette::LowBattery};

}  // namespace Escher
