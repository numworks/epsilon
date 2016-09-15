#include "law_cell.h"

constexpr KDColor separatorColor = KDColor(0xB4B7B9);
constexpr KDColor tableBackgroundColor = KDColor(0xF0F3F5);
constexpr KDCoordinate margin = 20;
constexpr KDColor focusedCellBackgroundColor = KDColor(0xBFD3EB);
constexpr KDColor cellBackgroundColor = KDColor(0xFCFCFC);

Probability::LawCell::LawCell() :
  ChildlessView(),
  Responder(nullptr),
  m_focused(false),
  m_even(false)
{
  m_message = "NULL";
}

void Probability::LawCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDColor backgroundColor = (m_focused ? focusedCellBackgroundColor : cellBackgroundColor);
  KDColor textColor = (m_focused ? KDColorWhite : KDColorBlack);

  ctx->fillRect(KDRect(margin+1, 1, width-2*margin-1, height-1), backgroundColor);
  ctx->fillRect(KDRect(0,0,margin,height), tableBackgroundColor);
  ctx->fillRect(KDRect(margin,0,width-2*margin,1), separatorColor);
  ctx->fillRect(KDRect(margin,0,1,height), separatorColor);
  ctx->fillRect(KDRect(width-margin,0,1,height), separatorColor);
  ctx->fillRect(KDRect(width-margin+1,0,margin, height), tableBackgroundColor);

  //KDColor background = m_even ? KDColor(0xEEEEEE) : KDColor(0x777777);
  //ctx->fillRect(rect, background);
  //ctx->drawString(m_message, KDPoint(margin+10, 5), textColor, backgroundColor);
}

void Probability::LawCell::setMessage(const char * message) {
  m_message = message;
}

void Probability::LawCell::didBecomeFirstResponder() {
  m_focused = true;
  markRectAsDirty(bounds());
}

void Probability::LawCell::didResignFirstResponder() {
  m_focused = false;
  markRectAsDirty(bounds());
}

void Probability::LawCell::setEven(bool even) {
  m_even = even;
}
