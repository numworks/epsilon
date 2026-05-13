#pragma once

#include <escher/palette.h>
#include <kandinsky/basic_color.h>
#include <stdint.h>

namespace Escher {

// Theme-dependent components: color varies per theme
#define THEMED_COMPONENTS(X)   \
  X(WallScreen)                \
  X(WallScreenDark)            \
  X(BackgroundWhite)           \
  X(BackgroundPopup)           \
  X(BackgroundBoxTitle)        \
  X(BackgroundButtonRow)       \
  X(CalculationBorderEven)     \
  X(CalculationBorderOdd)      \
  X(CalculationBorderSelected) \
  X(SurfaceSelect)             \
  X(SurfaceSelectDark)         \
  X(SurfaceSelectApps)         \
  X(BannerYellow)              \
  X(BannerPurple)              \
  X(BannerGrayWhite)           \
  X(BannerGrayMiddle)          \
  X(BannerGrayDarkMiddle)      \
  X(BannerGrayDark)            \
  X(TextDefault)               \
  X(TextTables)                \
  X(TextBanner)                \
  X(TextSelect)                \
  X(TextSelectGrayDark)        \
  X(TextSelectGrayVeryDark)    \
  X(TextSelectPurple)          \
  X(TextGrayDark)              \
  X(TextGrayDarkMiddle)        \
  X(TextGrayVeryDark)          \
  X(TextWhite)                 \
  X(TextPurple)                \
  X(BorderWhite)               \
  X(BorderWallScreen)          \
  X(BorderGrayMiddle)          \
  X(BorderGrayWhite)           \
  X(BorderGrayBright)          \
  X(BorderGrayDarkMiddle)      \
  X(BorderGrayDark)            \
  X(BorderPurple)              \
  X(BorderPurpleDark)          \
  X(BorderYellowDark)          \
  X(ButtonLightBorderMain)     \
  X(ButtonLightBorderBottom)   \
  X(ButtonLightShadow1)        \
  X(ButtonLightShadow2)        \
  X(ButtonGrayBorder)          \
  X(ButtonGrayShadow)          \
  X(SwitchButtonActive)        \
  X(SwitchButtonInactive)      \
  X(AxisBlack)                 \
  X(AxisLight)                 \
  X(AxisMedium)                \
  X(AxisGrayBright)            \
  X(AxisGrayDark)              \
  X(AxisGrayVeryDark)          \
  X(AxisRed)                   \
  X(TokenWhite)                \
  X(TokenYellow)               \
  X(TokenRed)                  \
  X(TokenBlack)                \
  X(TokenGrayMiddle)           \
  X(TokenGrayBright)           \
  X(TokenGrayDarkest)          \
  X(TokenGrayDark)             \
  X(TokenEllipsis)             \
  X(TokenBatteryLevel)         \
  X(TokenLowBattery)           \
  X(DataRed)                   \
  X(DataBlue)                  \
  X(DataGreen)                 \
  X(DataYellow)                \
  X(DataMagenta)               \
  X(DataTurquoise)             \
  X(DataPink)                  \
  X(DataOrange)                \
  X(DataViolet)                \
  X(DataMint)                  \
  X(DataRedLight)              \
  X(DataBlueLight)             \
  X(DataGreenLight)            \
  X(DataYellowLight)           \
  X(DataMagentaLight)          \
  X(DataTurquoiseLight)        \
  X(DataPinkLight)             \
  X(DataOrangeLight)           \
  X(DataVioletLight)           \
  X(DataMintLight)             \
  X(PythonText)                \
  X(PythonGutterBackground)

/* TODO: check the following colors which are not in the specs but used anyway:
 * - AxisGrayDark (additional result complex graph)
 * - AxisGrayBright (polar grid)
 */

/* Fixed components: color is the same in all themes.
 * If a color component is declared as fixed, it avoids declaring it with the
 * same value in all themes. A fixed component color also doesn't need to be
 * stored in the ThemeContainer, it is resolved directly in the
 * ThemeContainer::get switch. */
#define FIXED_COMPONENTS(X)                         \
  X(BasicBlack, BasicColor::Black)                  \
  X(BasicWhite, BasicColor::White)                  \
  X(BasicRed, BasicColor::Red)                      \
  X(BasicGreen, BasicColor::Green)                  \
  X(BasicBlue, BasicColor::Blue)                    \
  X(BasicYellow, BasicColor::Yellow)                \
  X(BasicOrange, BasicColor::Orange)                \
  X(BasicPurple, BasicColor::Purple)                \
  X(PythonComment, Palette::PYTHON_KhakiGray)       \
  X(PythonNumber, Palette::PYTHON_DarkCyan)         \
  X(PythonKeyword, Palette::PYTHON_Red)             \
  X(PythonOperator, Palette::PYTHON_RaspberryRed)   \
  X(PythonString, Palette::PYTHON_DarkBlue)         \
  X(PythonAutocomplete, Palette::PYTHON_GrayMiddle) \
  X(PythonHighlight, Palette::Select)

enum class ThemeComponent : uint8_t {
#define X(name) name,
  THEMED_COMPONENTS(X)
#undef X
#define X(name, color) name,
      FIXED_COMPONENTS(X)
#undef X
};

}  // namespace Escher
