#ifndef HARDWARE_TEST_SCREEN_TEST_CONTROLLER_H
#define HARDWARE_TEST_SCREEN_TEST_CONTROLLER_H

#include <escher.h>
#include "pattern_view.h"
#include "pattern.h"

namespace HardwareTest {

class ScreenTestController final : public ViewController {
public:
  ScreenTestController(Responder * parentResponder) :
    ViewController(parentResponder),
    m_patternIndex(0),
    m_view() {}
  View * view() override {
    return &m_view;
  }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
private:
  void showNextPattern();
  int m_patternIndex;
  PatternView m_view;
};

}

#endif

