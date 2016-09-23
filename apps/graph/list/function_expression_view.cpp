#include "function_expression_view.h"
#include "function_cell.h"
#include <poincare.h>

FunctionExpressionView::FunctionExpressionView(Responder * parentResponder) :
  ChildlessView(),
  Responder(parentResponder),
  m_focused(false)
{
}

void FunctionExpressionView::drawRect(KDContext * ctx, KDRect rect) const {
  // Select the background color according to the even line and the cursor selection
  bool evenLine = isEven();
  KDColor background = evenLine ? FunctionCell::k_evenLineBackgroundColor : FunctionCell::k_oddLineBackgroundColor;
  background = m_focused ? FunctionCell::k_selectedLineBackgroundColor : background;
  ctx->fillRect(rect, background);
  // Select text color according to the state of the function
  bool active = function()->isActive();
  KDColor text = active ? KDColorBlack : FunctionCell::k_desactiveTextColor;
  Graph::Function * function = FunctionExpressionView::function();
  ctx->drawString(function->text(), KDPointZero, text, background);
  // m_function->layout()->draw(ctx, KDPointZero);
}

void FunctionExpressionView::didBecomeFirstResponder() {
  m_focused = true;
  markRectAsDirty(bounds());
}

void FunctionExpressionView::didResignFirstResponder() {
  m_focused = false;
  markRectAsDirty(bounds());
}

bool FunctionExpressionView::isFocused() const {
  return m_focused;
}

Graph::Function * FunctionExpressionView::function() const{
  FunctionCell * parentCell = (FunctionCell *) parentResponder();
  return parentCell->function();
}

bool FunctionExpressionView::isEven() const {
  FunctionCell * parentCell = (FunctionCell *) parentResponder();
  return parentCell->isEven();
}

bool FunctionExpressionView::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::ENTER:
     // afficher un champs texte
      return true;
    default:
      return false;
  }
}
