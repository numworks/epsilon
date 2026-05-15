#include "focus_manager.h"

bool FocusManager::dispatchEvent(Ion::Events::Event event) {
  if (!m_isActive) {
    if (event != Ion::Events::ShiftPlus) {
      return false;
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
