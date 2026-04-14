#pragma once

#include <stdint.h>

namespace Escher {

// List of all theme components
#define THEME_COMPONENTS(X) \
  X(WallScreen)             \
  X(WallScreenDark)         \
  X(BackgroundWhite)        \
  X(BackgroundPopup)        \
  X(SurfaceSelect)          \
  X(BannerYellow)           \
  X(BannerGrayWhite)        \
  X(BannerGrayMiddle)       \
  X(TextDefault)            \
  X(TextGrayDark)           \
  X(TextGrayDarkMiddle)     \
  X(TextGrayVeryDark)       \
  X(TextWhite)              \
  X(TextPurple)             \
  X(TextBluishGray)         \
  X(TokenWhite)             \
  X(TokenYellow)            \
  X(TokenRed)               \
  X(TokenBlack)             \
  X(TokenGrayMiddle)        \
  X(TokenGrayBright)        \
  X(TokenGrayDark)          \
  X(DataRed)                \
  X(DataBlue)               \
  X(DataGreen)              \
  X(DataYellow)             \
  X(DataMagenta)            \
  X(DataTurquoise)          \
  X(DataPink)               \
  X(DataOrange)             \
  X(DataViolet)             \
  X(DataMint)

// TODO: check TextBluishGray

constexpr static int k_numberOfDataComponents = 10;

enum class ThemeComponent : uint8_t {
#define X(name) name,
  THEME_COMPONENTS(X)
#undef X
};

constexpr ThemeComponent DataColorComponent(int index) {
  return static_cast<ThemeComponent>(
      static_cast<uint8_t>(ThemeComponent::DataRed) + index);
}

}  // namespace Escher
