#include <escher/solid_color_view.h>

SolidColorView::SolidColorView(KDColor color) :
  ChildlessView(),
  m_color(color)
{
}

void SolidColorView::drawRect(KDRect rect) const {
  KDFillRect(rect, &m_color, 1);
}

#if ESCHER_VIEW_LOGGING
const char * SolidColorView::className() const {
  return "SolidColorView";
}

void SolidColorView::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " color=\"" << (int)m_color << "\"";
}
#endif
