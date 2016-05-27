#include <escher/solid_color_view.h>

SolidColorView::SolidColorView(KDColor color) :
  ChildlessView(),
  m_color(color)
{
}

void SolidColorView::drawRect(KDRect rect) const {
  KDFillRect(rect, m_color);
}

#if ESCHER_VIEW_LOGGING
const char * SolidColorView::className() const {
  return "SolidColorView";
}
#endif
