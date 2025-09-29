#pragma once

#include <apps/shared/math_toolbox_controller.h>

namespace Calculation {

class CalculationToolboxDataSource
    : public Shared::MathToolboxExtraCellsDataSource {
 public:
  int numberOfExtraCells() override {
    return m_displayAngleUnits ? k_numberOfExtraCells : 0;
  }

  Poincare::Layout extraCellLayoutAtRow(int row) override {
    assert(row >= 0 && row < k_numberOfExtraCells);
    return row == 0 ? "rad"_l : "°"_l;
  }

  void toggleAngleUnitsDisplay(bool areAngleUnitsDisplayed) {
    m_displayAngleUnits = areAngleUnitsDisplayed;
  }

 private:
  // 2 angle units
  constexpr static int k_numberOfExtraCells = 2;
  bool m_displayAngleUnits = false;
};

}  // namespace Calculation
