#ifndef HARDWARE_TEST_SCREEN_TEST_CONTROLLER_H
#define HARDWARE_TEST_SCREEN_TEST_CONTROLLER_H

#include <escher.h>
#include "led_test_controller.h"
#include "pattern_view.h"
#include "pattern.h"

namespace HardwareTest {

class ScreenTestController : public ViewController {
public:
  ScreenTestController(Responder * parentResponder);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
private:
  void showNextPattern();
  int m_patternIndex;
  PatternView m_view;
  LEDTestController m_ledTestController;
};

}

#endif

