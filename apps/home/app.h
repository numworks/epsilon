#ifndef HOME_APP_H
#define HOME_APP_H

#include <escher.h>
#include "controller.h"

class AppsContainer;

namespace Home {

class App : public ::App {
public:
  App(AppsContainer * container);
private:
  Controller m_controller;
};

}

#endif
