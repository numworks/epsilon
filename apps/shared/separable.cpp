#include "separable.h"

namespace Shared {

void Separable::setSeparatorLeft(bool separator) {
  if (m_separatorLeft != separator) {
    m_separatorLeft = separator;
    didSetSeparator();
  }
}

KDRect Separable::separatorRect(KDRect bounds,
                                KDCoordinate verticalOffset) const {
  KDCoordinate visibleOffset = m_separatorLeft ? 0 : verticalOffset;
  return KDRect(bounds.left(), bounds.top() + visibleOffset, k_thickness,
                bounds.height() - visibleOffset);
}

KDRect Separable::rectWithoutSeparator(KDRect r) const {
  return KDRect(r.left() + k_thickness, r.top(), r.width() - k_thickness,
                r.height());
}

}  // namespace Shared
