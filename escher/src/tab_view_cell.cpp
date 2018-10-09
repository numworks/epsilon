#include <escher/tab_view_cell.h>
#include <escher/palette.h>
extern "C" {
#include <assert.h>
}

TabViewCell::TabViewCell() :
  View(),
  m_active(false),
  m_selected(false),
  m_controller(nullptr)
{
}

void TabViewCell::setNamedController(ViewController * controller) {
  m_controller = controller;
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

KDSize TabViewCell::minimalSizeForOptimalDisplay() const {
  return KDFont::SmallFont->stringSize(m_controller->title());
}

void TabViewCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  // choose the background color
  KDColor text = m_active ? Palette::PurpleBright : KDColorWhite;
  KDColor background = m_active ? KDColorWhite : Palette::PurpleBright;
  KDColor selection = m_active ? Palette::Select : Palette::SelectDark;
  background = m_selected ? selection : background;
  // Color the background according to the state of the tab cell
  if (m_active || m_selected) {
    ctx->fillRect(KDRect(0, 0, width, 1), Palette::PurpleBright);
    ctx->fillRect(KDRect(0, 1, width, height-1), background);
  } else {
    ctx->fillRect(KDRect(0, 0, width, height), background);
  }
  // Write title
  KDSize textSize = KDFont::SmallFont->stringSize(m_controller->title());
  KDPoint origin((m_frame.width() - textSize.width())/2, (m_frame.height() - textSize.height())/2);
  ctx->drawString(m_controller->title(), origin, KDFont::SmallFont, text, background);
}

#if ESCHER_VIEW_LOGGING
const char * TabViewCell::className() const {
  return "TabViewCell";
}

void TabViewCell::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " active=\"" << m_active << "\"";
  os << " name=\"" << m_controller->title() << "\"";
}
#endif
