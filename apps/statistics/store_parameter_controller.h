#ifndef STATISTICS_STORE_PARAMETER_CONTROLLER_H
#define STATISTICS_STORE_PARAMETER_CONTROLLER_H

#include "../shared/store_parameter_controller.h"

namespace Statistics {

class StoreController;

class StoreParameterController : public Shared::StoreParameterController {
public:
  using Shared::StoreParameterController::StoreParameterController;
  void initializeColumnParameters() override;
private:
 I18n::Message sortMessage() override {
   return (m_columnIndex % 2 == 0) ? I18n::Message::SortValues : I18n::Message::SortSizes;
 }
};

}

#endif
