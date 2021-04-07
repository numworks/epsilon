#ifndef SHARED_RANGE_PARAMETER_CONTROLLER_H
#define SHARED_RANGE_PARAMETER_CONTROLLER_H

#include "button_with_separator.h"
#include "cell_with_separator.h"
#include "discard_pop_up_controller.h"
#include "float_parameter_controller.h"
#include "interactive_curve_view_range.h"
#include <escher/message_table_cell_with_chevron_and_buffer.h>
#include <escher/message_table_cell_with_editable_text.h>
#include <escher/message_table_cell_with_switch.h>
#include <escher/unequal_view.h>

namespace Shared {

class RangeParameterController : public Escher::SelectableListViewController {
public:
  RangeParameterController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * interactiveCurveViewRange);

  const char * title() override { return I18n::translate(I18n::Message::Axis); }

  int numberOfRows() const override { return displayNormalizeCell() + k_numberOfRangeCells + 1; }
  int typeAtIndex(int index) override;
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;

  void setRange(InteractiveCurveViewRange * range);
  Escher::StackViewController * stackController() { return static_cast<Escher::StackViewController *>(parentResponder()); }
  TELEMETRY_ID("Range");

private:
  constexpr static int k_numberOfRangeCells = 2;

  class CellWithUnequal : public Escher::MessageTableCell {
  public:
    using Escher::MessageTableCell::MessageTableCell;
    Escher::View * accessoryView() const { return const_cast<Escher::UnequalView *>(&m_unequalView); }
  private:
    Escher::UnequalView m_unequalView;
  };
  class NormalizeCell : public CellWithSeparator {
  public:
    NormalizeCell() : m_cell(I18n::Message::MakeOrthonormal) {}
    KDSize minimalSizeForOptimalDisplay() const override;
  private:
    Escher::HighlightCell * cell() override { return &m_cell; }
    bool separatorAboveCell() const override { return false; }
    mutable CellWithUnequal m_cell;
  };

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

  void buttonAction();
  bool displayNormalizeCell() const { return !m_interactiveRange->zoomNormalize(); }

  InteractiveCurveViewRange * m_interactiveRange;
  InteractiveCurveViewRange m_tempInteractiveRange;
  NormalizeCell m_normalizeCell;
  Escher::MessageTableCellWithChevronAndBuffer m_rangeCells[k_numberOfRangeCells];
  ButtonWithSeparator m_okButton;
  DiscardPopUpController m_confirmPopUpController;
  SingleRangeController m_singleRangeController;
};

}

#endif
