#include "editable_expression_view.h"

namespace Calculation {

EditableExpressionView::EditableExpressionView(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, ScrollableExpressionViewWithCursorDelegate * scrollableExpressionViewWithCursorDelegate) :
  ::EditableExpressionView(parentResponder, textFieldDelegate, scrollableExpressionViewWithCursorDelegate)
{
  setEditing(true);
}

bool EditableExpressionView::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    return false;
  }
  if (event == Ion::Events::Ans) {
    insertText("ans");
    return true;
  }
  if (isEditing() && isEmpty() &&
      (event == Ion::Events::Multiplication ||
       event == Ion::Events::Plus ||
       event == Ion::Events::Power ||
       event == Ion::Events::Square ||
       event == Ion::Events::Division ||
       event == Ion::Events::Sto)) {
    insertText("ans");
  }
  return(::EditableExpressionView::handleEvent(event));
}

}
