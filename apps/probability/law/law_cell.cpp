#include "law_cell.h"

Probability::LawCell::LawCell() :
  ChildlessView(),
  Responder(nullptr),
  m_focused(false),
  m_even(false)
{
  m_message = "NULL";
}

void Probability::LawCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor background = m_even ? KDColor(0xEEEEEE) : KDColor(0x777777);
  ctx->fillRect(rect, background);
  ctx->drawString(m_message, KDPointZero, m_focused);
}

void Probability::LawCell::setMessage(const char * message) {
  m_message = message;
}

void Probability::LawCell::setFocused(bool focused) {
  m_focused = focused;
  markRectAsDirty(bounds());
}

void Probability::LawCell::setEven(bool even) {
  m_even = even;
}
