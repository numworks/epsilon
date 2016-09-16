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
  bool evenLine = isEven();
  KDColor background = evenLine ? KDColor(0xEEEEEE) : KDColor(0x777777);
  ctx->fillRect(rect, background);
  KDColor text = m_focused ? KDColorBlack : KDColorWhite;
  KDColor textBackground = m_focused ? KDColorWhite : KDColorBlack;

  Graph::Function * function = FunctionExpressionView::function();
  ctx->drawString(function->text(), KDPointZero, text, textBackground);
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
