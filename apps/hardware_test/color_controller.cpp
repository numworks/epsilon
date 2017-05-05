#include "color_controller.h"
#include "../apps_container.h"
extern "C" {
#include <assert.h>
}

namespace HardwareTest {

ColorController::ColorController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view()
{
}

View * ColorController::view() {
  return &m_view;
}

bool ColorController::handleEvent(Ion::Events::Event event) {
  m_view.setColors(nextColor(m_view.fillColor()), nextColor(m_view.outlineColor()));
  if (m_view.fillColor() == KDColorBlack) {
    AppsContainer * container = (AppsContainer *)app()->container();
    container->switchTo(container->appAtIndex(0));
  }
  return true;
}

void ColorController::viewWillAppear() {
  m_view.setColors(KDColorBlack, KDColorWhite);
}

KDColor ColorController::nextColor(KDColor color) {
  if (color == KDColorBlack) {
    return KDColorWhite;
  }
  if (color == KDColorWhite) {
    return KDColorRed;
  }
  if (color == KDColorRed) {
    return KDColorBlue;
  }
  if (color == KDColorBlue) {
    return KDColorGreen;
  }
  if (color == KDColorGreen) {
    return KDColorBlack;
  }
  return KDColorRed;
}

}

