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
  void viewWillAppear() override;
  int numberOfRows() override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  float previousParameterAtIndex(int index) override;
  float parameterAtIndex(int index) override;
  void setParameterAtIndex(int parameterIndex, float f) override;
  constexpr static int k_numberOfTextCell = 4;
  InteractiveCurveViewRange * m_interactiveRange;
  char m_draftTextBuffer[MessageTableCellWithEditableText::k_bufferLength];
  MessageTableCellWithEditableText m_rangeCells[k_numberOfTextCell];
  MessageTableCellWithSwitch m_yAutoCell;
  float m_previousParameters[k_numberOfTextCell];
  bool m_previousSwitchState;
};

}

#endif
