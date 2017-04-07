#ifndef HOME_APP_H
#define HOME_APP_H

#include <escher.h>
#include "controller.h"
#include "../shared/resettable_app.h"

class AppsContainer;

namespace Home {

class App : public Shared::ResettableApp {
public:
  App(AppsContainer * container);
private:
  Controller m_controller;
};

}

#endif
