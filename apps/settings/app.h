#ifndef SETTINGS_APP_H
#define SETTINGS_APP_H

#include <escher.h>
#include "main_controller.h"
#include "../shared/resettable_app.h"

namespace Settings {

class App : public Shared::ResettableApp {
public:
  App(Container * container);
private:
  MainController m_mainController;
  StackViewController m_stackViewController;
};

}

#endif
