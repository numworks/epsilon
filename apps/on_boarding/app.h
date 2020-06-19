#ifndef ON_BOARDING_APP_H
#define ON_BOARDING_APP_H

#include <escher.h>
#include "country_controller.h"
#include "language_controller.h"
#include "logo_controller.h"

namespace OnBoarding {

class App : public ::App {
public:
  class Snapshot : public ::App::Snapshot {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  };

  static App * app() {
    return static_cast<App *>(Container::activeApp());
  }

  CountryController * countryController() { return &m_countryController; }
  int numberOfTimers() override;
  Timer * timerAtIndex(int i) override;
  bool processEvent(Ion::Events::Event) override;
  void didBecomeActive(Window * window) override;
private:
  App(Snapshot * snapshot);
  void reinitOnBoarding();
  StackViewController m_stackController;
  LanguageController m_languageController;
  CountryController m_countryController;
  LogoController m_logoController;
};

}

#endif
