#ifndef SHARED_RANGE_PARAMETER_CONTROLLER_H
#define SHARED_RANGE_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "interactive_curve_view_range.h"
#include "float_parameter_controller.h"

namespace Shared {

class RangeParameterController : public FloatParameterController {
public:
  RangeParameterController(Responder * parentResponder, InteractiveCurveViewRange * interactiveCurveViewRange);
  const char * title() override;
  int numberOfRows() override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  class MessageTableCellWithConvertibleEditableText : public MessageTableCellWithEditableText {
  public:
    MessageTableCellWithConvertibleEditableText(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, char * draftTextBuffer, I18n::Message message, InteractiveCurveViewRange * interactiveCurveViewRange) :
      MessageTableCellWithEditableText(parentResponder, textFieldDelegate, draftTextBuffer, message),
      m_interactiveRange(interactiveCurveViewRange) {}
    Responder * responder() override {
      if (m_interactiveRange->yAuto()) {
        return nullptr;
      } else {
        return this;
      }
    }
  private:
    InteractiveCurveViewRange * m_interactiveRange;
  };
  HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  View * loadView() override;
  void unloadView(View * view) override;
  constexpr static int k_numberOfEditableTextCell = 2;
  constexpr static int k_numberOfConvertibleTextCell = 2;
  constexpr static int k_numberOfTextCell = k_numberOfEditableTextCell+k_numberOfConvertibleTextCell;
  InteractiveCurveViewRange * m_interactiveRange;
  char m_draftTextBuffer[MessageTableCellWithEditableText::k_bufferLength];
  MessageTableCellWithEditableText * m_xRangeCells[k_numberOfEditableTextCell];
  MessageTableCellWithConvertibleEditableText * m_yRangeCells[k_numberOfConvertibleTextCell];
  MessageTableCellWithSwitch * m_yAutoCell;
};

}

#endif
