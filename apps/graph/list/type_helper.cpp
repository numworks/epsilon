#include "type_helper.h"
#include <apps/i18n.h>
#include <poincare/expression.h>
#include <poincare/layout_helper.h>

namespace Graph {

namespace PlotTypeHelper {

I18n::Message Message(int index) {
  assert(0 <= index && index < NumberOfTypes);
  static constexpr I18n::Message message[NumberOfTypes] = {
    I18n::Message::Cartesian,
    I18n::Message::Polar,
    I18n::Message::Parametric
  };
  return message[index];
}

Poincare::Layout Layout(int index) {
  assert(0 <= index && index < NumberOfTypes);
  static constexpr const char * texts[NumberOfTypes] = {
    "y=f(x)",
    "r=f(θ)",
    "[[x][y]]=f(t)"
  };
  const char * text = texts[index];
  if (index < 2) {
    return Poincare::LayoutHelper::String(text, strlen(text));
  }
  Poincare::Expression parametric = Poincare::Expression::Parse(text);
  return parametric.createLayout(Poincare::Preferences::PrintFloatMode::Decimal, 1);
}

}

}
