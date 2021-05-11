#ifndef SHARED_SINGLE_RANGE_CONTROLLER_H
#define SHARED_SINGLE_RANGE_CONTROLLER_H

#include "simple_float_parameter_controller.h"
#include "interactive_curve_view_range.h"
#include <escher/message_table_cell_with_editable_text.h>
#include <escher/message_table_cell_with_switch.h>

namespace Shared {

class SingleRangeController : public SimpleFloatParameterController<float> {
public:
  SingleRangeController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * interactiveCurveViewRange);

  const char * title() override { return I18n::translate(m_editXRange ? I18n::Message::ValuesOfX : I18n::Message::ValuesOfY); }
  int numberOfRows() const override { return k_numberOfTextCells + 1; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  bool handleEvent(Ion::Events::Event event) override;
  bool editXRange() const { return m_editXRange; }
  void setEditXRange(bool editXRange) { m_editXRange = editXRange; }

private:
  constexpr static int k_numberOfTextCells = 2;

  class LockableEditableCell : public Escher::MessageTableCellWithEditableText {
  public:
    Escher::Responder * responder() override;
    void setController(SingleRangeController * controller) { m_controller = controller; }
  private:
    SingleRangeController * m_controller;
  };

  bool autoStatus() const { return m_editXRange ? m_range->xAuto() : m_range->yAuto(); }
  float parameterAtIndex(int index) override;
  int reusableParameterCellCount(int type) override { return k_numberOfTextCells + 1; }
  Escher::HighlightCell * reusableParameterCell(int index, int type) override;
  bool setParameterAtIndex(int parameterIndex, float f) override;

  Escher::MessageTableCellWithSwitch m_autoCell;
  LockableEditableCell m_boundsCells[k_numberOfTextCells];
  InteractiveCurveViewRange * m_range;
  bool m_editXRange;
};

}

#endif
