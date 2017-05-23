#include "screen_test_controller.h"
#include "../apps_container.h"
extern "C" {
#include <assert.h>
}

namespace HardwareTest {

ScreenTestController::ScreenTestController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_patternIndex(0),
  m_view()
{
}

View * ScreenTestController::view() {
  return &m_view;
}

bool ScreenTestController::handleEvent(Ion::Events::Event event) {
  if (m_patternIndex == Pattern::numberOfPatterns()) {
    AppsContainer * container = (AppsContainer *)app()->container();
    container->switchTo(container->appSnapshotAtIndex(0));
  } else {
    showNextPattern();
  }
  return true;
}

void ScreenTestController::viewWillAppear() {
  m_patternIndex = 0;
  showNextPattern();
}

void ScreenTestController::showNextPattern() {
  m_view.setPattern(Pattern::patternAtIndex(m_patternIndex++));
}

}

