#pragma once

#include <apps/shared/math_toolbox_controller.h>

namespace Calculation {

class CalculationToolboxDataSource
    : public Shared::MathToolboxExtraCellsDataSource {
 public:
  int numberOfExtraCells() override { return k_numberOfExtraCells; }

  Poincare::Layout extraCellLayoutAtRow(int row) override {
    assert(row >= 0 && row < k_numberOfExtraCells);
    return row == 0 ? "rad"_l : "°"_l;
  }

 private:
  // 2 angle units
  constexpr static int k_numberOfExtraCells = 2;
};

}  // namespace Calculation
