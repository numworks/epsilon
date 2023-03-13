#include <escher/transparent_view.h>

namespace Escher {

void TransparentView::markRectAsDirty(KDRect rect) {
  if (m_superview) {
    m_superview->markAbsoluteRectAsDirty(rect.translatedBy(absoluteOrigin()));
  }
  View::markRectAsDirty(rect);
}

}  // namespace Escher
