#include "editable_expression_view.h"

namespace Calculation {

EditableExpressionView::EditableExpressionView(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout, EditableExpressionViewDelegate * delegate) :
  ::EditableExpressionView(parentResponder, expressionLayout, delegate)
{
  setEditing(true);
}

bool EditableExpressionView::privateHandleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    return false;
  }
  if (event == Ion::Events::Ans) {
    m_expressionViewWithCursor.cursor()->insertText("ans");
    return true;
  }
  Poincare::ExpressionLayout * layout = m_expressionViewWithCursor.expressionView()->expressionLayout();
  bool layoutIsEmpty = layout->isEmpty()
    || (layout->isHorizontal()
        && layout->numberOfChildren() == 0);
  if (isEditing() && layoutIsEmpty &&
      (event == Ion::Events::Multiplication ||
       event == Ion::Events::Plus ||
       event == Ion::Events::Power ||
       event == Ion::Events::Square ||
       event == Ion::Events::Division ||
       event == Ion::Events::Sto)) {
    m_expressionViewWithCursor.cursor()->insertText("ans");
  }
  return(::EditableExpressionView::privateHandleEvent(event));
}

}
