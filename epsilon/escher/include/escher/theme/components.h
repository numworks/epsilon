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

/* Fixed components: color is the same in all themes.
 * If a color component is declared as fixed, it avoids declaring it with the
 * same value in all themes. A fixed component color also doesn't need to be
 * stored in the ThemeContainer, it is resolved directly in the
 * ThemeContainer::get switch. */
#define FIXED_COMPONENTS(X)                      \
  X(DataRed, Palette::Red)                       \
  X(DataBlue, Palette::Blue)                     \
  X(DataGreen, Palette::Green)                   \
  X(DataYellow, Palette::YellowDark)             \
  X(DataMagenta, Palette::Magenta)               \
  X(DataTurquoise, Palette::Turquoise)           \
  X(DataPink, Palette::Pink)                     \
  X(DataOrange, Palette::Orange)                 \
  X(DataViolet, Palette::Violet)                 \
  X(DataMint, Palette::Mint)                     \
  X(DataRedLight, Palette::RedLight)             \
  X(DataBlueLight, Palette::BlueLight)           \
  X(DataGreenLight, Palette::GreenLight)         \
  X(DataYellowLight, Palette::YellowLight)       \
  X(DataMagentaLight, Palette::MagentaLight)     \
  X(DataTurquoiseLight, Palette::TurquoiseLight) \
  X(DataPinkLight, Palette::PinkLight)           \
  X(DataOrangeLight, Palette::OrangeLight)       \
  X(DataVioletLight, Palette::VioletLight)       \
  X(DataMintLight, Palette::MintLight)

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

constexpr ThemeComponent DataColorLightComponent(int index) {
  return static_cast<ThemeComponent>(
      static_cast<uint8_t>(ThemeComponent::DataRedLight) + index);
}

}  // namespace Escher
