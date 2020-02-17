#ifndef SHARED_RANGE_PARAMETER_CONTROLLER_H
#define SHARED_RANGE_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "interactive_curve_view_range.h"
#include "float_parameter_controller.h"

namespace Shared {

class RangeParameterController : public FloatParameterController<float> {
public:
  RangeParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * interactiveCurveViewRange);
  const char * title() override;
  int numberOfRows() const override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("Range");
private:
  class MessageTableCellWithConvertibleEditableText : public MessageTableCellWithEditableText {
  public:
    Responder * responder() override {
      if (m_interactiveRange->yAuto()) {
        return nullptr;
      } else {
        return this;
      }
    }
    void setInteractiveCurveViewRange(InteractiveCurveViewRange * interactiveCurveViewRange) { m_interactiveRange = interactiveCurveViewRange; }
  private:
    InteractiveCurveViewRange * m_interactiveRange;
  };
  HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  float parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, float f) override;
  constexpr static int k_numberOfEditableTextCell = 2;
  constexpr static int k_numberOfConvertibleTextCell = 2;
  constexpr static int k_numberOfTextCell = k_numberOfEditableTextCell+k_numberOfConvertibleTextCell;
  InteractiveCurveViewRange * m_interactiveRange;
  MessageTableCellWithEditableText m_xRangeCells[k_numberOfEditableTextCell];
  MessageTableCellWithConvertibleEditableText m_yRangeCells[k_numberOfConvertibleTextCell];
  MessageTableCellWithSwitch m_yAutoCell;
};

}

#endif
