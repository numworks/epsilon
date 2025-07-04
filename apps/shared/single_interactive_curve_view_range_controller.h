#ifndef SHARED_SINGLE_INTERACTIVE_CURVE_VIEW_RANGE_CONTROLLER_H
#define SHARED_SINGLE_INTERACTIVE_CURVE_VIEW_RANGE_CONTROLLER_H

#include <apps/shared/single_range_controller.h>

#include "interactive_curve_view_range.h"
#include "poincare_helpers.h"

namespace Shared {

class SingleInteractiveCurveViewRangeController
    : public SingleRangeControllerExactExpressions {
 public:
  using ParameterType = SingleRangeControllerExactExpressions::ParameterType;
  using FloatType = SingleRangeControllerExactExpressions::FloatType;

  SingleInteractiveCurveViewRangeController(
      Escher::Responder* parentResponder,
      InteractiveCurveViewRange* interactiveCurveViewRange,
      MessagePopUpController* confirmPopUpController);

  const char* title() const override {
    return I18n::translate(m_axis == OMG::Axis::Horizontal
                               ? I18n::Message::ValuesOfX
                               : I18n::Message::ValuesOfY);
  }
  int numberOfRows() const override {
    return SingleRangeControllerExactExpressions::numberOfRows() + 1;
  }
  int typeAtRow(int row) const override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  KDCoordinate separatorBeforeRow(int row) const override;
  bool textFieldDidReceiveEvent(Escher::AbstractTextField* textField,
                                Ion::Events::Event event) override;
  void textFieldDidAbortEditing(Escher::AbstractTextField* textField) override;

  OMG::Axis axis() const { return m_axis; }
  void setAxis(OMG::Axis axis);

 private:
  constexpr static int k_stepCellType = 3;
  I18n::Message parameterMessage(int index) const override {
    assert(index == 0 || index == 1);
    return index == 0 ? I18n::Message::Minimum : I18n::Message::Maximum;
  }
  // Step value must be strictly postive and not contain random numbers.
  static bool IsValidStepValue(ParameterType value) {
    FloatType floatValue = PoincareHelpers::ToFloat(value);
    return (std::isnan(floatValue) || floatValue > 0.0f) &&
           (value.expression().isUninitialized() ||
            !value.expression().hasRandomNumber());
  }
  bool boundsParametersAreDifferent();
  bool parametersAreDifferent() override;
  void extractParameters() override;
  void setAutoRange() override;
  float limit() const override { return InteractiveCurveViewRange::k_maxFloat; }
  void confirmParameters() override;
  void pop(bool onConfirmation) override { stackController()->pop(); }
  bool setParameterAtIndex(int parameterIndex, ParameterType value) override;
  int reusableParameterCellCount(int type) const override;
  Escher::HighlightCell* reusableParameterCell(int index, int type) override;
  Escher::TextField* textFieldOfCellAtIndex(Escher::HighlightCell* cell,
                                            int index) override;
  ParameterType parameterAtIndex(int index) override;
  bool hasUndefinedValue(const char* text, ParameterType value,
                         int row) const override;
  bool stepIsAuto() const {
    return std::isnan(PoincareHelpers::ToFloat(m_stepParameter));
  }

  InteractiveCurveViewRange* m_range;
  // m_secondaryRangeParam is only used when activating xAuto while yAuto is on.
  Poincare::Range1D<FloatType> m_secondaryRangeParam;

  Escher::MenuCellWithEditableText<Escher::MessageTextView> m_gridUnitCell;
  ParameterType m_stepParameter;

  OMG::Axis m_axis;
};

}  // namespace Shared

#endif
