#include "../shared/store_parameter_controller.h"

namespace Statistics {

void StoreParameterController::intializeColumnParameters() {
  Shared::StoreParameterController::initializeColumnParameters();
  m_clearColumn.setMessage(I18n::Message::ClearColumn);
}

}
