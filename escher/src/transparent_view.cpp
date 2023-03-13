#include <escher/transparent_view.h>

namespace Escher {

void TransparentView::markRectAsDirty(KDRect rect) {
  // TODO
  // if (m_superview) {
  // m_superview->markRectAsDirty(rect.translatedBy(
  // m_frame.translatedBy(m_superview->absoluteOrigin().opposite())
  // .origin()));
  // }
  View::markRectAsDirty(rect);
}

}  // namespace Escher
