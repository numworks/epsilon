#ifndef APPS_LOGO_CONTROLLER_H
#define APPS_LOGO_CONTROLLER_H

#include <escher.h>
#include "logo_view.h"

namespace OnBoarding {

class LogoController final : public ViewController, public Timer {
public:
  LogoController() : ViewController(nullptr), Timer(10), m_logoView() {}
  View * view() override {
    return &m_logoView;
  }
private:
  bool fire() override;
  LogoView m_logoView;
};

}

#endif

