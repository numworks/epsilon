#ifndef ON_BOARDING_APP_H
#define ON_BOARDING_APP_H

#include <escher.h>
#include "language_controller.h"
#include "logo_controller.h"

namespace OnBoarding {

class App : public ::App {
public:
  App(Container * container);
  void reinitOnBoarding();
  bool hasTimer();
  Timer * timer();
private:
  LanguageController m_languageController;
  LogoController m_logoController;
};

}

#endif
