#ifndef INFERENCE_SLOPE_COLUMN_PARAMETER_CONTROLLER_H
#define INFERENCE_SLOPE_COLUMN_PARAMETER_CONTROLLER_H

#include "shared/store_parameter_controller.h"

namespace Inference {

class SlopeColumnParameterController : public Shared::StoreParameterController {
public:
  using StoreParameterController::StoreParameterController;
  Escher::ViewController::TitlesDisplay titlesDisplay() override { return Escher::ViewController::TitlesDisplay::DisplayLastThreeTitles; }
  int numberOfRows() const override { return sizeof(k_typesOrder)/sizeof(int); }
  int typeAtIndex(int index) override { return k_typesOrder[index]; }

private:
  constexpr static int k_typesOrder[] = {k_sortCellType, k_fillFormulaCellType, k_clearCellType};

};

}

#endif

