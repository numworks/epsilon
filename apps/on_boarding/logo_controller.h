#ifndef APPS_LOGO_CONTROLLER_H
#define APPS_LOGO_CONTROLLER_H

#include <escher.h>
#include "logo_view.h"

namespace OnBoarding {

class LogoController : public ViewController, public Timer {
public:
  LogoController();
  View * view() override { return &m_logoView; }
  void viewWillAppear() override;
  void viewDidDisappear() override;
private:
  bool fire() override;
  LogoView m_logoView;
  KDColor m_previousLEDColor;
  bool m_didPerformTests;
};

}

#endif

