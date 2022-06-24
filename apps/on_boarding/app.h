#ifndef ON_BOARDING_APP_H
#define ON_BOARDING_APP_H

#include <escher.h>
#include "logo_controller.h"
#include "localization_controller.h"
#include "../shared/shared_app.h"

namespace OnBoarding {

class App : public ::App {
public:
  class Snapshot : public ::SharedApp::Snapshot {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  };
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
