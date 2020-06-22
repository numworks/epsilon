#include "country_controller.h"

namespace Settings {

CountryController::CountryController(Responder * parentResponder, KDCoordinate verticalMargin) :
  Shared::CountryController(parentResponder, verticalMargin)
{
  m_contentView.shouldDisplayTitle(false);
}


bool CountryController::handleEvent(Ion::Events::Event event) {
  if (Shared::CountryController::handleEvent(event) || event == Ion::Events::Left) {
    static_cast<StackViewController *>(parentResponder())->pop();
    return true;
  }
  return false;
}

}
