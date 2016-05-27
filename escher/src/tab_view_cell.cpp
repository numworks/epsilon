#include <escher/tab_view_cell.h>
extern "C" {
#include <assert.h>
}

TabViewCell::TabViewCell() :
  ChildlessView(),
  m_active(false),
  m_name(nullptr)
{
}

void TabViewCell::setName(const char * name) {
  m_name = name;
  redraw();
}

void TabViewCell::setActive(bool active) {
  m_active = active;
  redraw();
}

void TabViewCell::drawRect(KDRect rect) const {
  KDDrawString(m_name, {0,0}, m_active);
}

#if ESCHER_VIEW_LOGGING
const char * TabViewCell::className() const {
  return "TabViewCell";
}
#endif

