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
  X(TokenGrayDark)

// TODO: check TextBluishGray

enum class ThemeComponent : uint8_t {
#define X(name) name,
  THEME_COMPONENTS(X)
#undef X
};

}  // namespace Escher
