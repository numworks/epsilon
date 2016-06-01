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
  markAsNeedingRedraw();
}

void TabViewCell::setActive(bool active) {
  m_active = active;
  markAsNeedingRedraw();
}

void TabViewCell::drawRect(KDRect rect) const {
  KDDrawString(m_name, {0,0}, m_active);
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
