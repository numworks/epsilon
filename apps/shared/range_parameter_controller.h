#ifndef SHARED_RANGE_PARAMETER_CONTROLLER_H
#define SHARED_RANGE_PARAMETER_CONTROLLER_H

#include "button_with_separator.h"
#include "cell_with_separator.h"
#include "interactive_curve_view_range.h"
#include "single_interactive_curve_view_range_controller.h"
#include "pop_up_controller.h"
#include <escher/message_table_cell_with_chevron_and_buffer.h>
#include <escher/unequal_view.h>

namespace Shared {

class RangeParameterController : public Escher::SelectableListViewController<Escher::MemoizedListViewDataSource> {
public:
  RangeParameterController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * interactiveCurveViewRange);

  const char * title() override { return I18n::translate(I18n::Message::Axis); }

  int numberOfRows() const override { return displayNormalizeCell() + k_numberOfRangeCells + 1; }
  int typeAtIndex(int index) const override;
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  bool handleEvent(Ion::Events::Event event) override;

  void setRange(InteractiveCurveViewRange * range);
  Escher::StackViewController * stackController() { return static_cast<Escher::StackViewController *>(parentResponder()); }
  TELEMETRY_ID("Range");

private:
  constexpr static int k_numberOfRangeCells = 2;
  constexpr static int k_normalizeCellType = 0;
  constexpr static int k_rangeCellType = 1;
  constexpr static int k_okCellType = 2;

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
    const Escher::TableCell * constCell() const override { return &m_cell; }
    bool separatorAboveCell() const override { return false; }
    mutable CellWithUnequal m_cell;
  };


  void buttonAction();
  bool displayNormalizeCell() const { return !m_tempInteractiveRange.zoomNormalize(); }

  InteractiveCurveViewRange * m_interactiveRange;
  InteractiveCurveViewRange m_tempInteractiveRange;
  NormalizeCell m_normalizeCell;
  Escher::MessageTableCellWithChevronAndBuffer m_rangeCells[k_numberOfRangeCells];
  ButtonWithSeparator m_okButton;
  Shared::MessagePopUpController m_confirmPopUpController;
  SingleInteractiveCurveViewRangeController m_singleInteractiveCurveViewRangeController;
};

}

#endif
