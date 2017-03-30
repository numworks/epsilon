#ifndef HARDWARE_TEST_APP_H
#define HARDWARE_TEST_APP_H

#include <escher.h>
#include "keyboard_controller.h"

class AppsContainer;

namespace HardwareTest {

class App : public ::App {
public:
  App(AppsContainer * container);
private:
  KeyboardController m_keyboardController;
};

}

#endif

