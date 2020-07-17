#ifndef ON_BOARDING_APP_H
#define ON_BOARDING_APP_H

#include <escher.h>
#include "logo_controller.h"
#include "localization_controller.h"

namespace OnBoarding {

class App : public ::App {
public:
  class Snapshot : public ::App::Snapshot {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  };

  int numberOfTimers() override;
  Timer * timerAtIndex(int i) override;
  bool processEvent(Ion::Events::Event) override;
  void didBecomeActive(Window * window) override;
private:
  App(Snapshot * snapshot);
  void reinitOnBoarding();
  LocalizationController m_localizationController;
  LogoController m_logoController;
};

}

#endif
