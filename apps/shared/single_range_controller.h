#ifndef SHARED_SINGLE_RANGE_CONTROLLER_H
#define SHARED_SINGLE_RANGE_CONTROLLER_H

#include "float_parameter_controller.h"
#include "interactive_curve_view_range.h"
#include <escher/message_table_cell_with_editable_text.h>
#include <escher/message_table_cell_with_switch.h>

namespace Shared {

class SingleRangeController : public FloatParameterController<float> {
public:
  SingleRangeController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * interactiveCurveViewRange);

  const char * title() override { return I18n::translate(m_editXRange ? I18n::Message::ValuesOfX : I18n::Message::ValuesOfY); }
  int numberOfRows() const override { return k_numberOfTextCells + 2; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  bool handleEvent(Ion::Events::Event event) override;
  void setEditXRange(bool editXRange) { m_editXRange = editXRange; }

private:
  constexpr static int k_numberOfTextCells = 2;

  float parameterAtIndex(int index) override;
  int reusableParameterCellCount(int type) override { return k_numberOfTextCells + 1; }
  Escher::HighlightCell * reusableParameterCell(int index, int type) override;
  bool setParameterAtIndex(int parameterIndex, float f) override;

  Escher::MessageTableCellWithSwitch m_autoCell;
  Escher::MessageTableCellWithEditableText m_boundsCells[k_numberOfTextCells];
  InteractiveCurveViewRange * m_range;
  bool m_editXRange;
};

}

#endif
