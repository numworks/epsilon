#include "two_proportions_z_test.h"

#include <poincare/k_tree.h>
#include <poincare/layout.h>

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
  constexpr KTree pHat =
      KCombinedCodePointL<'p', UCodePointCombiningCircumflex>();
  if (static_cast<EstimatesOrder>(index) == EstimatesOrder::Pooled) {
    return KRackL(pHat);  // p̂
  }
  return static_cast<EstimatesOrder>(index) == EstimatesOrder::P1
             ? pHat ^ KSubscriptL("1"_l)
             : pHat ^ KSubscriptL("2"_l);
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
