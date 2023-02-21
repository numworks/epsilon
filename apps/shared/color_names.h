#ifndef SHARED_COLOR_NAMES_H
#define SHARED_COLOR_NAMES_H

#include <apps/i18n.h>
#include <escher/palette.h>

namespace Shared {

namespace ColorNames {

constexpr static int k_count = 8;
constexpr static KDColor k_colors[k_count] = {
    Escher::Palette::Red,     Escher::Palette::Blue,
    Escher::Palette::Green,   Escher::Palette::YellowDark,
    Escher::Palette::Magenta, Escher::Palette::Turquoise,
    Escher::Palette::Pink,    Escher::Palette::Orange};

constexpr static I18n::Message k_messages[k_count] = {
    I18n::Message::ColorRed,     I18n::Message::ColorBlue,
    I18n::Message::ColorGreen,   I18n::Message::ColorYellowDark,
    I18n::Message::ColorMagenta, I18n::Message::ColorTurquoise,
    I18n::Message::ColorPink,    I18n::Message::ColorOrange};

constexpr static I18n::Message k_curveMessages[k_count] = {
    I18n::Message::ColorRedCurve,     I18n::Message::ColorBlueCurve,
    I18n::Message::ColorGreenCurve,   I18n::Message::ColorYellowDarkCurve,
    I18n::Message::ColorMagentaCurve, I18n::Message::ColorTurquoiseCurve,
    I18n::Message::ColorPinkCurve,    I18n::Message::ColorOrangeCurve,
};

I18n::Message NameForColor(KDColor color);
I18n::Message NameForCurveColor(KDColor color);

}  // namespace ColorNames

}  // namespace Shared

#endif
