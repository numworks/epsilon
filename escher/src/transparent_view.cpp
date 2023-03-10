#include <escher/transparent_view.h>

namespace Escher {

void TransparentView::markRectAsDirty(KDRect rect) {
  if (m_superview) {
    m_superview->markRectAsDirty(rect.translatedBy(relativeFrame().origin()));
  }
  View::markRectAsDirty(rect);
}

}  // namespace Escher
