#pragma once

#include <stdint.h>

namespace Escher {

// Theme-dependent components: color varies per theme
#define THEMED_COMPONENTS(X) \
  X(WallScreen)              \
  X(WallScreenDark)          \
  X(BackgroundWhite)         \
  X(BackgroundPopup)         \
  X(SurfaceSelect)           \
  X(BannerYellow)            \
  X(BannerGrayWhite)         \
  X(BannerGrayMiddle)        \
  X(TextDefault)             \
  X(TextGrayDark)            \
  X(TextGrayDarkMiddle)      \
  X(TextGrayVeryDark)        \
  X(TextWhite)               \
  X(TextPurple)              \
  X(TextBluishGray)          \
  X(TokenWhite)              \
  X(TokenYellow)             \
  X(TokenRed)                \
  X(TokenBlack)              \
  X(TokenGrayMiddle)         \
  X(TokenGrayBright)         \
  X(TokenGrayDark)

// TODO: check TextBluishGray

// Fixed components: color is the same in all themes.
#define FIXED_COMPONENTS(X)            \
  X(DataRed, Palette::Red)             \
  X(DataBlue, Palette::Blue)           \
  X(DataGreen, Palette::Green)         \
  X(DataYellow, Palette::YellowDark)   \
  X(DataMagenta, Palette::Magenta)     \
  X(DataTurquoise, Palette::Turquoise) \
  X(DataPink, Palette::Pink)           \
  X(DataOrange, Palette::Orange)       \
  X(DataViolet, Palette::Violet)       \
  X(DataMint, Palette::Mint)

enum class ThemeComponent : uint8_t {
#define X(name) name,
  THEMED_COMPONENTS(X)
#undef X
#define X(name, color) name,
      FIXED_COMPONENTS(X)
#undef X
};

constexpr ThemeComponent DataColorComponent(int index) {
  return static_cast<ThemeComponent>(
      static_cast<uint8_t>(ThemeComponent::DataRed) + index);
}

}  // namespace Escher
