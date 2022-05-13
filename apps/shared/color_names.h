#ifndef SHARED_COLOR_NAMES_H
#define SHARED_COLOR_NAMES_H

#include <escher/palette.h>
#include <apps/i18n.h>

namespace Shared {

namespace ColorNames {

static constexpr int Count = 8;
static constexpr KDColor Colors[Count] = {
  Escher::Palette::Red,
  Escher::Palette::Blue,
  Escher::Palette::Green,
  Escher::Palette::YellowDark,
  Escher::Palette::Magenta,
  Escher::Palette::Turquoise,
  Escher::Palette::Pink,
  Escher::Palette::Orange
};
static constexpr I18n::Message Messages[Count] = {
  I18n::Message::ColorRed,
  I18n::Message::ColorBlue,
  I18n::Message::ColorGreen,
  I18n::Message::ColorYellowDark,
  I18n::Message::ColorMagenta,
  I18n::Message::ColorTurquoise,
  I18n::Message::ColorPink,
  I18n::Message::ColorOrange
};

I18n::Message NameForColor(KDColor color);

}

}

#endif
