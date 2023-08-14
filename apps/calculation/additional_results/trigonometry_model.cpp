#include "trigonometry_model.h"

#include "trigonometry_list_controller.h"

namespace Calculation {

float TrigonometryModel::yRange() const {
  // With the cell overlap, only one separator needs to be counted
  constexpr static float k_height =
      TrigonometryListController::k_illustrationHeight -
      Escher::Metric::CellSeparatorThickness;
  return (k_height - 1) * k_targetRatio;
}

}  // namespace Calculation
