#ifndef INFERENCE_SLOPE_COLUMN_PARAMETER_CONTROLLER_H
#define INFERENCE_SLOPE_COLUMN_PARAMETER_CONTROLLER_H

#include <array>

#include "shared/store_parameter_controller.h"

namespace Inference {

class SlopeColumnParameterController : public Shared::StoreParameterController {
 public:
  using StoreParameterController::StoreParameterController;
  Escher::ViewController::TitlesDisplay titlesDisplay() override {
    return m_titlesDisplay;
  }
  void setTitlesDisplay(Escher::ViewController::TitlesDisplay titlesDisplay) {
    m_titlesDisplay = titlesDisplay;
  }
  int numberOfRows() const override { return std::size(k_typesOrder); }
  int typeAtIndex(int index) const override { return k_typesOrder[index]; }

 private:
  constexpr static int k_typesOrder[] = {k_sortCellType, k_fillFormulaCellType,
                                         k_clearCellType};

  Escher::ViewController::TitlesDisplay m_titlesDisplay;
};

}  // namespace Inference

#endif
