#pragma once

#include <apps/i18n.h>
#include <escher/theme/data_colors.h>

namespace Shared {

namespace ColorNames {

// Subset of DataColors
constexpr static int k_count = 8;
constexpr static Escher::ThemeComponent k_colors[k_count] = {
    Escher::ThemeComponent::DataRed,     Escher::ThemeComponent::DataBlue,
    Escher::ThemeComponent::DataGreen,   Escher::ThemeComponent::DataYellow,
    Escher::ThemeComponent::DataMagenta, Escher::ThemeComponent::DataTurquoise,
    Escher::ThemeComponent::DataPink,    Escher::ThemeComponent::DataOrange};

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

I18n::Message NameForColor(Escher::ThemeComponent color);
I18n::Message NameForCurveColor(Escher::ThemeComponent color);

}  // namespace ColorNames

}  // namespace Shared
