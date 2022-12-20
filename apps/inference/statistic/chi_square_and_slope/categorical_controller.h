#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_CONTROLLER_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_CONTROLLER_H

#include <escher/cell_with_margins.h>
#include <escher/invocation.h>
#include <escher/message_table_cell_with_editable_text_with_message.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/selectable_table_view.h>
#include <escher/stack_view_controller.h>
#include <shared/button_with_separator.h>
#include "inference/statistic/chi_square_and_slope/categorical_table_cell.h"
#include "inference/statistic/chi_square_and_slope/dynamic_size_table_view_data_source.h"
#include "inference/models/statistic/chi2_test.h"

namespace Inference {

/* Common controller between InputHomogeneityController,
 * InputGoodnessController and ResultsHomogeneityController.
 * A CategoricalController is a table whose first cell is also a table of class
 * CategoricalTableCell. */

class CategoricalController : public Escher::SelectableListViewController<Escher::ListViewDataSource>, public Escher::SelectableTableViewDelegate, public Escher::ScrollViewDelegate {
public:
  CategoricalController(Escher::Responder * parent, Escher::ViewController * nextController, Escher::Invocation invocation);

  static bool ButtonAction(CategoricalController * controller, void * s);

  // Responder
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event & event) override;

  // ScrollViewDelegate
  void scrollViewDidChangeOffset(ScrollViewDataSource * scrollViewDataSource) override;
  bool updateBarIndicator(bool vertical, bool * visible) override;

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;

  // ListViewDataSource
  int typeAtIndex(int index) const override { return index; } // One cell per type
  int numberOfRows() const override { return indexOfNextCell() + 1; }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return 1; }

protected:
  constexpr static int k_marginVertical = 5;
  constexpr static int k_indexOfTableCell = 0;

  KDCoordinate nonMemoizedRowHeight(int index) override;

  virtual int indexOfNextCell() const { return 1; }
  virtual CategoricalTableCell * categoricalTableCell() = 0;

  Escher::ViewController * m_nextController;
  Shared::ButtonWithSeparator m_next;
};

/* Common Controller between InputHomogeneityController and
 * InputGoodnessController. */

class InputCategoricalController : public CategoricalController, public Shared::ParameterTextFieldDelegate, public DynamicSizeTableViewDataSourceDelegate {
public:
  InputCategoricalController(Escher::StackViewController * parent, Escher::ViewController * resultsController, Statistic * statistic, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate);

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::AbstractTextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) override;

  static bool ButtonAction(InputCategoricalController * controller, void * s);

  // ViewController
  void viewWillAppear() override;
  Escher::ViewController::TitlesDisplay titlesDisplay() override { return Escher::ViewController::TitlesDisplay::DisplayLastTitle; }

  // DynamicSizeTableViewDataSourceDelegate
  void tableViewDataSourceDidChangeSize() override;

  // ListViewDataSource
  Escher::HighlightCell * reusableCell(int index, int type) override;

protected:
  EditableCategoricalTableCell * categoricalTableCell() override = 0;
  virtual int indexOfSignificanceCell() const = 0;
  int indexOfNextCell() const override { return indexOfSignificanceCell() + 1; }
  bool handleEditedValue(int i, double p, Escher::AbstractTextField * textField, Ion::Events::Event event);
  virtual int indexOfEditedParameterAtIndex(int index) const {
    if (index == indexOfSignificanceCell()) {
      return m_statistic->indexOfThreshold();
    }
    assert(false);
    return -1;
  }

  Statistic * m_statistic;
  Escher::MessageTableCellWithEditableTextWithMessage m_innerSignificanceCell;
  Escher::CellWithMargins m_significanceCell;
};

}  // namespace Inference

#endif /* INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_CATEGORICAL_CONTROLLER_H */
