#include "function_cell.h"

FunctionCell::FunctionCell() :
  ChildlessView(),
  Responder(nullptr),
  m_focused(false),
  m_even(false)
{
  m_message = "NULL";
}

void FunctionCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor background = m_even ? KDColor(0xEEEEEE) : KDColor(0x777777);
  ctx->fillRect(rect, background);
  ctx->drawString(m_message, KDPointZero, m_focused);
}

void FunctionCell::setMessage(const char * message) {
  m_message = message;
}

void FunctionCell::setFocused(bool focused) {
  m_focused = focused;
  markRectAsDirty(bounds());
}

void FunctionCell::setEven(bool even) {
  m_even = even;
}
