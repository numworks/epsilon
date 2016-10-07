#include <escher/tab_view_cell.h>
extern "C" {
#include <assert.h>
}

constexpr KDColor TabViewCell::k_defaultTabColor;
constexpr KDColor TabViewCell::k_selectedTabColor;
constexpr KDColor TabViewCell::k_separatorTabColor;

TabViewCell::TabViewCell() :
  ChildlessView(),
  m_active(false),
  m_selected(false),
  m_name(nullptr)
{
}

void TabViewCell::setName(const char * name) {
  m_name = name;
  markRectAsDirty(bounds());
}

void TabViewCell::setActive(bool active) {
  m_active = active;
  markRectAsDirty(bounds());
}

void TabViewCell::setSelected(bool selected) {
  m_selected = selected;
  markRectAsDirty(bounds());
}

void TabViewCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  // choose the background color
  KDColor text = m_active ? k_defaultTabColor : KDColorWhite;
  KDColor background = m_active ? KDColorWhite : k_defaultTabColor;
  background = m_selected ? k_selectedTabColor : background;
  // Color the background according to the state of the tab cell
  if (m_active || m_selected) {
    ctx->fillRect(KDRect(0, 0, width, height), background);
  } else {
    ctx->fillRect(KDRect(0, 0, width, 1), k_separatorTabColor);
    ctx->fillRect(KDRect(0, 1, width, height-1), background);
  }
  // Write title
  KDSize textSize = KDText::stringSize(m_name);
  KDPoint origin(0.5f*(m_frame.width() - textSize.width()),0.5f*(m_frame.height() - textSize.height()));
  ctx->drawString(m_name, origin, text, background);
}

#if ESCHER_VIEW_LOGGING
const char * TabViewCell::className() const {
  return "TabViewCell";
}

void TabViewCell::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " active=\"" << m_active << "\"";
  os << " name=\"" << m_name << "\"";
}
#endif
