#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_CONTROLLER_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_CONTROLLER_H

#include <escher/button_cell.h>
#include <escher/invocation.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/selectable_table_view.h>
#include <escher/stack_view_controller.h>

#include "inference/models/statistic/chi2_test.h"
#include "inference/statistic/chi_square_and_slope/categorical_table_cell.h"
#include "inference/statistic/chi_square_and_slope/input_categorical_cell.h"

namespace Inference {

/* Common controller between InputHomogeneityController,
 * InputGoodnessController and ResultsHomogeneityTabController.
 * A CategoricalController is a table whose first cell is also a table of class
 * CategoricalTableCell. */

class CategoricalController
    : public Escher::SelectableListViewController<Escher::ListViewDataSource>,
      public Escher::ScrollViewDelegate,
      public Escher::SelectableListViewDelegate {
 public:
  CategoricalController(Escher::Responder* parent,
                        Escher::ViewController* nextController,
                        Escher::Invocation invocation);

  static bool ButtonAction(CategoricalController* controller, void* s);

  // Responder
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  // ScrollViewDelegate
  void scrollViewDidChangeOffset(
      ScrollViewDataSource* scrollViewDataSource) override;
  bool updateBarIndicator(bool vertical, bool* visible) override;

  // SelectableListViewDelegate
  void listViewDidChangeSelectionAndDidScroll(
      SelectableListView* l, int previousRow, KDPoint previousOffset,
      bool withinTemporarySelection = false) override;

  // ListViewDataSource
  int typeAtRow(int row) const override { return row; }  // One cell per type
  int numberOfRows() const override { return indexOfNextCell() + 1; }
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return 1; }
  KDCoordinate separatorBeforeRow(int row) override {
    return row == indexOfNextCell() ? k_defaultRowSeparator : 0;
  }

 protected:
  constexpr static int k_marginVertical = 5;
  constexpr static int k_indexOfTableCell = 0;

  KDCoordinate nonMemoizedRowHeight(int row) override;

  virtual int indexOfNextCell() const { return 1; }
  virtual CategoricalTableCell* categoricalTableCell() = 0;

  Escher::ViewController* m_nextController;
  Escher::ButtonCell m_next;
};

/* Common Controller between InputHomogeneityController and
 * InputGoodnessController. */

class InputCategoricalController : public CategoricalController,
                                   public Shared::ParameterTextFieldDelegate {
 public:
  InputCategoricalController(
      Escher::StackViewController* parent,
      Escher::ViewController* resultsController, Statistic* statistic,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate);

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 const char* text,
                                 Ion::Events::Event event) override;

  static bool ButtonAction(InputCategoricalController* controller, void* s);

  // ViewController
  void viewWillAppear() override;
  Escher::ViewController::TitlesDisplay titlesDisplay() override {
    return Escher::ViewController::TitlesDisplay::DisplayLastTitle;
  }

  // ListViewDataSource
  Escher::HighlightCell* reusableCell(int index, int type) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;

 protected:
  InputCategoricalTableCell* categoricalTableCell() override = 0;
  virtual int indexOfSignificanceCell() const = 0;
  int indexOfNextCell() const override { return indexOfSignificanceCell() + 1; }
  bool handleEditedValue(int i, double p, Escher::AbstractTextField* textField,
                         Ion::Events::Event event);
  virtual int indexOfEditedParameterAtIndex(int index) const {
    if (index == indexOfSignificanceCell()) {
      return m_statistic->indexOfThreshold();
    }
    assert(false);
    return -1;
  }

  Statistic* m_statistic;
  InputCategoricalCell m_significanceCell;
};

}  // namespace Inference

#endif
