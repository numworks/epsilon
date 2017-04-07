#ifndef HARDWARE_TEST_APP_H
#define HARDWARE_TEST_APP_H

#include <escher.h>
#include "keyboard_controller.h"
#include "../shared/resettable_app.h"

class AppsContainer;

namespace HardwareTest {

class App : public Shared::ResettableApp {
public:
  App(AppsContainer * container);
private:
  KeyboardController m_keyboardController;
};

}

#endif

