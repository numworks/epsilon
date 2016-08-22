#include "function_cell.h"
#include <poincare.h>

FunctionCell::FunctionCell() :
  ChildlessView(),
  Responder(nullptr),
  m_function(nullptr),
  m_focused(false),
  m_even(false)
{
}

void FunctionCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor background = m_even ? KDColor(0xEEEEEE) : KDColor(0x777777);
  ctx->fillRect(rect, background);
  ctx->drawString(m_function->text(), KDPointZero, m_focused);
  m_function->layout()->draw(ctx, KDPointZero);
  //Expression * foo = Expression::parse("1+2/3");
  //ExpressionLayout * fooLayout = foo->createLayout();
  //fooLayout->draw(ctx, KDPointZero);
  //delete fooLayout;
  //delete foo;
}

void FunctionCell::setFunction(Graph::Function * f) {
  m_function = f;
}

void FunctionCell::setFocused(bool focused) {
  m_focused = focused;
  markRectAsDirty(bounds());
}

void FunctionCell::setEven(bool even) {
  m_even = even;
}
