#ifndef SHARED_RANGE_PARAMETER_CONTROLLER_H
#define SHARED_RANGE_PARAMETER_CONTROLLER_H

#include "interactive_curve_view_range.h"
#include "cell_with_separator.h"
#include "float_parameter_controller.h"
#include "discard_pop_up_controller.h"
#include <escher/message_table_cell_with_editable_text.h>
#include <escher/unequal_view.h>

namespace Shared {

class RangeParameterController : public FloatParameterController<float> {
public:
  RangeParameterController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * interactiveCurveViewRange);
  const char * title() override;
  int numberOfRows() const override;
  int typeAtIndex(int index) override;
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  void setRange(InteractiveCurveViewRange * range);
  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("Range");
private:
  class CellWithUnequal : public Escher::MessageTableCell {
  public:
    using Escher::MessageTableCell::MessageTableCell;
    Escher::View * accessoryView() const { return const_cast<Escher::UnequalView *>(&m_unequalView); }
  private:
    Escher::UnequalView m_unequalView;
  };
  class NormalizeCell : public CellWithSeparator {
  public:
    NormalizeCell() : m_cell(I18n::Message::Orthonormal) {}
    KDSize minimalSizeForOptimalDisplay() const override;
  private:
    Escher::HighlightCell * cell() override { return &m_cell; }
    bool separatorAboveCell() const override { return false; }
    mutable CellWithUnequal m_cell;
  };

  Escher::HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  float parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, float f) override;
  void buttonAction() override;
  bool displayNormalizeCell() const { return !m_interactiveRange->zoomNormalize() && !m_forceHideNormalizeCell; }
  constexpr static int k_numberOfTextCell = 4;
  InteractiveCurveViewRange * m_interactiveRange;
  InteractiveCurveViewRange m_tempInteractiveRange;
  Escher::MessageTableCellWithEditableText m_rangeCells[k_numberOfTextCell];
  DiscardPopUpController m_confirmPopUpController;
  NormalizeCell m_normalizeCell;
  bool m_forceHideNormalizeCell;
};

}

#endif
