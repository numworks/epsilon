#include "color_names.h"

I18n::Message Shared::ColorNames::NameForColor(Escher::ThemeComponent color) {
  for (int i = 0; i < k_count; i++) {
    if (color == k_colors[i]) {
      return k_messages[i];
    }
  }
  return I18n::Message::UndefinedType;
}

I18n::Message Shared::ColorNames::NameForCurveColor(
    Escher::ThemeComponent color) {
  for (int i = 0; i < k_count; i++) {
    if (color == k_colors[i]) {
      return k_curveMessages[i];
    }
  }
  return I18n::Message::UndefinedType;
}
