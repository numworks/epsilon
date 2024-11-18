#ifndef SHARED_SINGLE_INTERACTIVE_CURVE_VIEW_RANGE_CONTROLLER_H
#define SHARED_SINGLE_INTERACTIVE_CURVE_VIEW_RANGE_CONTROLLER_H

#include <apps/shared/single_range_controller.h>

#include "interactive_curve_view_range.h"

namespace Shared {

class SingleInteractiveCurveViewRangeController
    : public SingleRangeController<float> {
 public:
  SingleInteractiveCurveViewRangeController(
      Escher::Responder* parentResponder,
      InteractiveCurveViewRange* interactiveCurveViewRange,
      MessagePopUpController* confirmPopUpController);

  const char* title() override {
    return I18n::translate(m_axis == OMG::Axis::Horizontal
                               ? I18n::Message::ValuesOfX
                               : I18n::Message::ValuesOfY);
  }
  int numberOfRows() const override {
    return SingleRangeController<float>::numberOfRows() + 1;
  }
  int typeAtRow(int row) const override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  KDCoordinate separatorBeforeRow(int row) override;
  bool textFieldDidReceiveEvent(Escher::AbstractTextField* textField,
                                Ion::Events::Event event) override;
  void textFieldDidAbortEditing(Escher::AbstractTextField* textField) override;

  OMG::Axis axis() const { return m_axis; }
  void setAxis(OMG::Axis axis);

 private:
  constexpr static int k_gridUnitCellType = 3;
  I18n::Message parameterMessage(int index) const override {
    assert(index == 0 || index == 1);
    return index == 0 ? I18n::Message::Minimum : I18n::Message::Maximum;
  }
  bool boundsParametersAreDifferent();
  bool parametersAreDifferent() override;
  void extractParameters() override;
  void setAutoRange() override;
  float limit() const override { return InteractiveCurveViewRange::k_maxFloat; }
  void confirmParameters() override;
  void pop(bool onConfirmation) override { stackController()->pop(); }
  bool setParameterAtIndex(int parameterIndex, float f) override;
  int reusableParameterCellCount(int type) const override;
  Escher::HighlightCell* reusableParameterCell(int index, int type) override;
  Escher::TextField* textFieldOfCellAtIndex(Escher::HighlightCell* cell,
                                            int index) override;
  float parameterAtIndex(int index) override;
  bool hasUndefinedValue(const char* text, float floatValue,
                         int row) const override;
  bool gridUnitIsAuto() const { return std::isnan(m_gridUnitParam); }

  InteractiveCurveViewRange* m_range;
  // m_secondaryRangeParam is only used when activating xAuto while yAuto is on.
  Poincare::Range1D<float> m_secondaryRangeParam;

  Escher::MenuCellWithEditableText<Escher::MessageTextView> m_gridUnitCell;
  float m_gridUnitParam;

  OMG::Axis m_axis;
};

}  // namespace Shared

#endif
