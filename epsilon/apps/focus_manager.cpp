#include "focus_manager.h"

#include <assert.h>

bool FocusManager::dispatchEvent(Ion::Events::Event event,
                                 const Escher::BlinkTimer& blinkTimer) {
  if (!m_isActive) {
    if (event != Ion::Events::ShiftPlus) {
      return false;
    }
    if (blinkTimer.isActive()) {
      // Set offset to make cursor visible
      focusOn(blinkTimer.getOrigin());
    }
    m_isActive = true;
  } else if (event == Ion::Events::ShiftLeft && m_horizontalOffset > 0) {
    m_horizontalOffset--;
  } else if (event == Ion::Events::ShiftRight &&
             m_horizontalOffset < k_maxHorizontalOffset) {
    m_horizontalOffset++;
  } else if (event == Ion::Events::ShiftUp && m_verticalOffset > 0) {
    m_verticalOffset--;
  } else if (event == Ion::Events::ShiftDown &&
             m_verticalOffset < k_maxVerticalOffset) {
    m_verticalOffset++;
  } else {
    for (Ion::Events::Event e : k_eventsThatResetFocus) {
      if (event == e) {
        m_isActive = false;
        Ion::Display::resetFocus();
        return true;
      }
    }
    return false;
  }
  Ion::Display::setFocus(k_factor, KDPoint(m_horizontalOffset * k_step,
                                           m_verticalOffset * k_step));
  return true;
}

void FocusManager::focusOn(KDPoint point) {
  /* Find best offsets such that Rect(m_horizontalOffset * k_step,
   * m_verticalOffset * k_step, k_width, k_height) is centered on point.
   * The + step/2 gives nearest-integer rounding. */
  int idealH = (point.x() - k_width / 2 + k_step / 2) / k_step;
  m_horizontalOffset =
      std::clamp<KDCoordinate>(idealH, 0, k_maxHorizontalOffset);
  int idealV = (point.y() - k_height / 2 + k_step / 2) / k_step;
  m_verticalOffset = std::clamp<KDCoordinate>(idealV, 0, k_maxVerticalOffset);
  assert(KDRect(m_horizontalOffset * k_step, m_verticalOffset * k_step, k_width,
                k_height)
             .contains(point));
}
