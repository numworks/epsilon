#ifndef ON_BOARDING_APP_H
#define ON_BOARDING_APP_H

#include "../shared/shared_app.h"
#include "localization_controller.h"

namespace OnBoarding {

class App : public Escher::App {
 public:
  class Snapshot : public Shared::SharedApp::Snapshot {
   public:
    App* unpack(Escher::Container* container) override;
    const Descriptor* descriptor() const override;
  };

  void willBecomeInactive() override;
  void didBecomeActive(Escher::Window* window) override;

 private:
  App(Snapshot* snapshot);
  void reinitOnBoarding();
  LocalizationController m_localizationController;
};

}  // namespace OnBoarding

#endif
