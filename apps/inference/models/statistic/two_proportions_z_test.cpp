#include "two_proportions_z_test.h"

#include <poincare_layouts.h>

namespace Inference {

double TwoProportionsZTest::estimateValue(int index) {
  switch (static_cast<EstimatesOrder>(index)) {
    case EstimatesOrder::P1:
      return TwoProportions::X1(parametersArray()) /
             TwoProportions::N1(parametersArray());
    case EstimatesOrder::P2:
      return TwoProportions::X2(parametersArray()) /
             TwoProportions::N2(parametersArray());
    case EstimatesOrder::Pooled:
      return (TwoProportions::X1(parametersArray()) +
              TwoProportions::X2(parametersArray())) /
             (TwoProportions::N1(parametersArray()) +
              TwoProportions::N2(parametersArray()));
    default:
      assert(false);
      return 0.0;
  }
}

Poincare::Layout TwoProportionsZTest::estimateLayout(int index) const {
  Poincare::Layout pHat = Poincare::CombinedCodePointsLayout::Builder(
      'p', UCodePointCombiningCircumflex);
  if (static_cast<EstimatesOrder>(index) == EstimatesOrder::Pooled) {
    return pHat;  // p̂
  }
  return Poincare::HorizontalLayout::Builder(
      pHat,
      Poincare::VerticalOffsetLayout::Builder(
          Poincare::CodePointLayout::Builder(
              static_cast<EstimatesOrder>(index) == EstimatesOrder::P1 ? '1'
                                                                       : '2'),
          Poincare::VerticalOffsetLayoutNode::VerticalPosition::Subscript));
}

I18n::Message TwoProportionsZTest::estimateDescription(int index) {
  switch (static_cast<EstimatesOrder>(index)) {
    case EstimatesOrder::P1:
      return TwoProportions::Sample1ProportionDescription();
    case EstimatesOrder::P2:
      return TwoProportions::Sample2ProportionDescription();
    case EstimatesOrder::Pooled:
      return TwoProportions::PooledProportionDescription();
    default:
      assert(false);
      return I18n::Message::Default;
  }
}

}  // namespace Inference
