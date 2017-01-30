#ifndef SETTINGS_APP_H
#define SETTINGS_APP_H

#include <escher.h>
#include "main_controller.h"
#include "../preferences.h"

namespace Settings {

class App : public ::App {
public:
  App(Container * container, Preferences * preferences);
private:
  MainController m_mainController;
  StackViewController m_stackViewController;
};

}

#endif
