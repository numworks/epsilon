#ifndef SHARED_RANGE_PARAMETER_CONTROLLER_H
#define SHARED_RANGE_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "interactive_curve_view_range.h"
#include "float_parameter_controller.h"

namespace Shared {

class RangeParameterController : public FloatParameterController {
public:
  RangeParameterController(Responder * parentResponder, InteractiveCurveViewRange * interactiveCurveViewRange);
  const char * title() const override;
  int numberOfRows() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  float parameterAtIndex(int index) override;
  void setParameterAtIndex(int parameterIndex, float f) override;
  constexpr static int k_numberOfTextCell = 4;
  InteractiveCurveViewRange * m_interactiveRange;
  char m_draftTextBuffer[EditableTextMenuListCell::k_bufferLength];
  EditableTextMenuListCell m_rangeCells[k_numberOfTextCell];
  SwitchMenuListCell m_yAutoCell;
};

}

#endif
