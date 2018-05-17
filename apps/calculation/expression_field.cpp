#include "expression_field.h"

namespace Calculation {

bool ExpressionField::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    return false;
  }
  if (event == Ion::Events::Ans) {
    handleEventWithText("ans");
    return true;
  }
  if (isEditing() && isEmpty() &&
      (event == Ion::Events::Multiplication ||
       event == Ion::Events::Plus ||
       event == Ion::Events::Power ||
       event == Ion::Events::Square ||
       event == Ion::Events::Division ||
       event == Ion::Events::Sto)) {
    handleEventWithText("ans");
  }
  return(::ExpressionField::handleEvent(event));
}

}
