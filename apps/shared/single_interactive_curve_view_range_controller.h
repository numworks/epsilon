#ifndef SHARED_SINGLE_INTERACTIVE_CURVE_VIEW_RANGE_CONTROLLER_H
#define SHARED_SINGLE_INTERACTIVE_CURVE_VIEW_RANGE_CONTROLLER_H

#include <apps/shared/single_range_controller.h>

#include "interactive_curve_view_range.h"

namespace Shared {

class SingleInteractiveCurveViewRangeController
    : public SingleRangeController<float> {
 public:
  using Axis = InteractiveCurveViewRange::Axis;

  SingleInteractiveCurveViewRangeController(
      Escher::Responder* parentResponder,
      InteractiveCurveViewRange* interactiveCurveViewRange,
      MessagePopUpController* confirmPopUpController);

  const char* title() override {
    return I18n::translate(m_axis == Axis::X ? I18n::Message::ValuesOfX
                                             : I18n::Message::ValuesOfY);
  }

  Axis axis() const { return m_axis; }
  void setAxis(Axis axis);

 private:
  I18n::Message parameterMessage(int index) const override {
    assert(index == 0 || index == 1);
    return index == 0 ? I18n::Message::Minimum : I18n::Message::Maximum;
  }
  bool parametersAreDifferent() override;
  void extractParameters() override;
  void setAutoRange() override;
  float limit() const override { return InteractiveCurveViewRange::k_maxFloat; }
  void confirmParameters() override;
  void pop(bool onConfirmation) override { stackController()->pop(); }

  InteractiveCurveViewRange* m_range;
  // m_secondaryRangeParam is only used when activating xAuto while yAuto is on.
  Poincare::Range1D<float> m_secondaryRangeParam;

  Axis m_axis;
};

}  // namespace Shared

#endif
