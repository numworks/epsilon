#include "function_name_view.h"
#include "function_cell.h"
#include <poincare.h>

FunctionNameView::FunctionNameView(Responder * parentResponder, Invocation invocation) :
  ChildlessView(),
  Responder(parentResponder),
  m_invocation(invocation),
  m_focused(false)
{
}

void FunctionNameView::drawRect(KDContext * ctx, KDRect rect) const {
  bool evenLine = isEven();
  KDColor background = evenLine ? KDColor(0xEEEEEE) : KDColor(0x777777);
  ctx->fillRect(rect, background);
  KDColor text = m_focused ? KDColorBlack : KDColorWhite;
  KDColor textBackground = m_focused ? KDColorWhite : KDColorBlack;

  Graph::Function * function = FunctionNameView::function();
  ctx->drawString(function->name(), KDPointZero, text, textBackground);
  // m_function->layout()->draw(ctx, KDPointZero);
}


void FunctionNameView::didBecomeFirstResponder() {
  m_focused = true;
  markRectAsDirty(bounds());
}

void FunctionNameView::didResignFirstResponder() {
  m_focused = false;
  markRectAsDirty(bounds());
}

bool FunctionNameView::isFocused() const {
  return m_focused;
}

Graph::Function * FunctionNameView::function() const {
  FunctionCell * parentCell = (FunctionCell *) parentResponder();
  return parentCell->function();
}

bool FunctionNameView::isEven() const {
  FunctionCell * parentCell = (FunctionCell *) parentResponder();
  return parentCell->isEven();
}


bool FunctionNameView::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::ENTER:
      m_invocation.perform(this);
      return true;
    default:
      return false;
  }
}
