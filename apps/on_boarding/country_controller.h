#ifndef ON_BOARDING_COUNTRY_CONTROLLER_H
#define ON_BOARDING_COUNTRY_CONTROLLER_H

#include <escher.h>
#include "../shared/country_controller.h"

namespace OnBoarding {

class CountryController : public Shared::CountryController {
public:
  CountryController(Responder * parentResponder);
  void resetSelection() override;
  bool handleEvent(Ion::Events::Event event) override;
  ViewController::DisplayParameter displayParameter() override { return ViewController::DisplayParameter::WantsMaximumSpace; }

};

}

#endif
