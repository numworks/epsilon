#ifndef ON_BOARDING_APP_H
#define ON_BOARDING_APP_H

#include <escher.h>
#include "language_controller.h"
#include "logo_controller.h"
#include "update_controller.h"

namespace OnBoarding {

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    App * build(Container * container) override;
  };
  static Descriptor * buildDescriptor();
  void reinitOnBoarding();
  bool hasTimer();
  Timer * timer();
private:
  App(Container * container, Descriptor * descriptor);
  LanguageController m_languageController;
  LogoController m_logoController;
};

}

#endif
