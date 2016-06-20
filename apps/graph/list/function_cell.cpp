#include "function_cell.h"

FunctionCell::FunctionCell() :
  ChildlessView(),
  m_focused(false),
  m_even(false)
{
  m_message = "NULL";
}

void FunctionCell::drawRect(KDRect rect) const {
  KDColor background = m_even ? KDColorRGB(0xEE, 0xEE, 0xEE) : KDColorRGB(0x77,0x77,0x77);
  KDFillRect(rect, background);
  KDDrawString(m_message, KDPointZero, m_focused);
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
