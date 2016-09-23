#include "function_name_view.h"
#include "function_cell.h"
#include <poincare.h>

FunctionNameView::FunctionNameView(Responder * parentResponder, Invocation invocation) :
  ChildlessView(),
  Responder(parentResponder),
  m_focused(false),
  m_invocation(invocation)
{
}

void FunctionNameView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  // First color the color indicator
  KDColor functionColor = function()->color();
  ctx->fillRect(KDRect(0, 0, k_colorIndicatorThickness, height), functionColor);
  // Select the background color according to the even line and the cursor selection
  bool evenLine = isEven();
  KDColor background = evenLine ? FunctionCell::k_evenLineBackgroundColor : FunctionCell::k_oddLineBackgroundColor;
  background = m_focused ? FunctionCell::k_selectedLineBackgroundColor : background;
  ctx->fillRect(KDRect(4, 0, width-4, height), background);
  // Select text color according to the state of the function
  bool active = function()->isActive();
  KDColor text = active ? KDColorBlack : FunctionCell::k_desactiveTextColor;
  Graph::Function * function = FunctionNameView::function();
  ctx->drawString(function->name(), KDPoint(4, 0), text, background);
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
