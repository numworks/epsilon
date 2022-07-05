#include "main_controller.h"

namespace Periodic {

bool MainController::handleEvent(Ion::Events::Event e) {
  if (e == Ion::Events::Up) {
    return m_elementsView.moveCursorVertically(false);
  }
  if (e == Ion::Events::Down) {
    return m_elementsView.moveCursorVertically(true);
  }
  if (e == Ion::Events::Left) {
    return m_elementsView.moveCursorHorizontally(false);
  }
  if (e == Ion::Events::Right) {
    return m_elementsView.moveCursorHorizontally(true);
  }
  return false;
}

}
