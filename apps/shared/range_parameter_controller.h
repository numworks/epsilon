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
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  View * loadView() override;
  void unloadView(View * view) override;
  constexpr static int k_numberOfTextCell = 4;
  InteractiveCurveViewRange * m_interactiveRange;
  char m_draftTextBuffer[MessageTableCellWithEditableText::k_bufferLength];
  MessageTableCellWithEditableText * m_rangeCells[k_numberOfTextCell];
  MessageTableCellWithSwitch * m_yAutoCell;
};

}

#endif
