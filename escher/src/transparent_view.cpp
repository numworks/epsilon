#include <escher/transparent_view.h>

void TransparentView::markRectAsDirty(KDRect rect) {
  if (m_superview) {
    m_superview->markRectAsDirty(KDRect(m_superview->pointFromPointInView(this, rect.origin()), rect.size()));
  }
  View::markRectAsDirty(rect);
}
