#include "country_controller.h"

namespace Settings {

bool CountryController::handleEvent(Ion::Events::Event event) {
  if (Shared::CountryController::handleEvent(event) || event == Ion::Events::Left) {
    static_cast<StackViewController *>(parentResponder())->pop();
    return true;
  }
  return false;
}

}
