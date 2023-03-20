#ifndef INFERENCE_STATISTIC_INPUT_CONTROLLER_H
#define INFERENCE_STATISTIC_INPUT_CONTROLLER_H

#include <apps/shared/float_parameter_controller.h>
#include <escher/highlight_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/menu_cell_with_editable_text.h>

#include "inference/models/statistic/statistic.h"
#include "inference/shared/dynamic_cells_data_source.h"
#include "results_controller.h"

namespace Inference {

using InputParameterCell =
    Escher::MenuCellWithEditableText<Escher::ExpressionView,
                                     Escher::MessageTextView>;

class InputController
    : public Shared::FloatParameterController<double>,
      public DynamicCellsDataSource<InputParameterCell,
                                    k_maxNumberOfInputParameterCell>,
      public DynamicCellsDataSourceDelegate<InputParameterCell> {
  friend class InputSlopeController;

 public:
  InputController(Escher::StackViewController* parent,
                  ResultsController* resultsController, Statistic* statistic,
                  Escher::InputEventHandlerDelegate* handler);
  int numberOfRows() const override {
    return m_statistic->numberOfParameters() + 1 /* button */;
  }
  const char* title() override {
    InputTitle(this, m_statistic, m_titleBuffer, k_titleBufferSize);
    return m_titleBuffer;
  }
  ViewController::TitlesDisplay titlesDisplay() override;
  int typeAtIndex(int i) const override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  void buttonAction() override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;
  KDCoordinate separatorBeforeRow(int index) override;

  void initCell(InputParameterCell, void* cell, int index) override;
  Escher::SelectableTableView* tableView() override {
    return &m_selectableListView;
  }

  constexpr static int k_numberOfReusableCells =
      Ion::Display::Height / Escher::TableCell::k_minimalLargeFontCellHeight +
      2;

 protected:
  double parameterAtIndex(int i) override {
    return m_statistic->parameterAtIndex(i);
  }

 private:
  int reusableParameterCellCount(int type) override;
  Escher::HighlightCell* reusableParameterCell(int index, int type) override;
  Escher::TextField* textFieldOfCellAtIndex(Escher::HighlightCell* cell,
                                            int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  static void InputTitle(Escher::ViewController* vc, Statistic* statistic,
                         char* titleBuffer, size_t titleBufferSize);

  constexpr static int k_numberOfTitleSignificantDigits =
      Poincare::Preferences::VeryShortNumberOfSignificantDigits;
  constexpr static int k_titleBufferSize =
      sizeof("H0:= Ha: α=") + 7 /* μ1-μ2 */ +
      3 * (Poincare::PrintFloat::charSizeForFloatsWithPrecision(
               k_numberOfTitleSignificantDigits) -
           1) +
      2 /* op */ + 10;
  char m_titleBuffer[k_titleBufferSize];
  Statistic* m_statistic;
  ResultsController* m_resultsController;

  constexpr static int k_significanceCellType = 2;

  Escher::MenuCellWithEditableText<Escher::MessageTextView,
                                   Escher::MessageTextView>
      m_significanceCell;
};

}  // namespace Inference

#endif
