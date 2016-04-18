#include <escher/solid_color_view.h>

SolidColorView::SolidColorView(KDRect frame, KDColor color) :
  View(frame),
  m_color(color)
{
}

void SolidColorView::drawRect(KDRect rect) {
  KDFillRect(rect, m_color);
}
