#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_CONTROLLER_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_CONTROLLER_H

#include <escher/buffer_text_highlight_cell.h>
#include <escher/button_cell.h>
#include <escher/invocation.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/selectable_table_view.h>
#include <escher/stack_view_controller.h>

#include "inference/models/statistic/chi2_test.h"
#include "inference/statistic/chi_square_and_slope/categorical_cell.h"
#include "inference/statistic/chi_square_and_slope/categorical_table_cell.h"

namespace Inference {

/* Common controller between InputHomogeneityController,
 * ResultsHomogeneityTabController, InputGoodnessController,
 * ResultsGoodnessController.
 * A CategoricalController is a table whose first cell
 * is also a table of class CategoricalTableCell. */

class CategoricalController
    : public Escher::SelectableListViewController<Escher::ListViewDataSource>,
      public Escher::ScrollViewDelegate,
      public Escher::SelectableListViewDelegate {
 public:
  CategoricalController(Escher::Responder* parent,
                        Escher::ViewController* nextController,
                        Escher::Invocation invocation);

  static bool ButtonAction(CategoricalController* controller, void* s);

  // ScrollViewDelegate
  void scrollViewDidChangeOffset(
      ScrollViewDataSource* scrollViewDataSource) override;
  bool updateBarIndicator(bool vertical, bool* visible) override;

  // SelectableListViewDelegate
  void listViewDidChangeSelectionAndDidScroll(
      Escher::SelectableListView* l, int previousRow, KDPoint previousOffset,
      bool withinTemporarySelection = false) override;

  // ListViewDataSource
  int typeAtRow(int row) const override { return row; }  // One cell per type
  int numberOfRows() const override { return indexOfNextCell() + 1; }
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override { return 1; }
  KDCoordinate separatorBeforeRow(int row) override {
    return row == indexOfNextCell() ? k_defaultRowSeparator : 0;
  }

  void initView() override;

 protected:
  KDCoordinate nonMemoizedRowHeight(int row) override;

  virtual int indexOfTableCell() const { return 0; }
  virtual int indexOfNextCell() const { return indexOfTableCell() + 1; }
  virtual CategoricalTableCell* categoricalTableCell() = 0;

  Escher::ViewController* m_nextController;
  Escher::ButtonCell m_next;

 private:
  virtual void createDynamicCells() = 0;
};

/* Common Controller between InputHomogeneityController and
 * InputGoodnessController. */

class InputCategoricalController : public CategoricalController,
                                   public Shared::ParameterTextFieldDelegate {
 public:
  InputCategoricalController(Escher::StackViewController* parent,
                             Escher::ViewController* resultsController,
                             Statistic* statistic);

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

  static bool ButtonAction(InputCategoricalController* controller, void* s);

  // Responder
  bool handleEvent(Ion::Events::Event event) override;

  // ViewController
  void viewWillAppear() override;
  Escher::ViewController::TitlesDisplay titlesDisplay() override {
    return Escher::ViewController::TitlesDisplay::DisplayLastTitle;
  }

  // ListViewDataSource
  Escher::HighlightCell* reusableCell(int index, int type) override;

 protected:
  KDCoordinate nonMemoizedRowHeight(int row) override;

  InputCategoricalTableCell* categoricalTableCell() override = 0;
  virtual int indexOfSignificanceCell() const = 0;
  int indexOfNextCell() const override { return indexOfSignificanceCell() + 1; }
  virtual int indexOfEditedParameterAtIndex(int index) const {
    assert(index == indexOfSignificanceCell());
    return m_statistic->indexOfThreshold();
  }

  Statistic* m_statistic;
  InputCategoricalCell m_significanceCell;
};

class InputNamedListsCategoricalController : public InputCategoricalController,
                                             Escher::DropdownCallback {
 public:
  InputNamedListsCategoricalController(
      Escher::StackViewController* parent,
      Escher::ViewController* resultsController, Statistic* statistic);

  void viewWillAppear() override;

 protected:
  class DropdownDataSource : public Escher::ExplicitListViewDataSource {
   public:
    int numberOfRows() const override { return k_numberOfRows; }
    Escher::HighlightCell* cell(int row) override { return &m_cells[row]; }

   private:
    constexpr static int k_numberOfRows = 3;  // FIXME
    Escher::SmallBufferTextHighlightCell m_cells[k_numberOfRows];
  };

  constexpr static int k_dummyCellIndex = 0;

  int indexOfTableCell() const override { return k_dummyCellIndex + 1; }
  KDCoordinate nonMemoizedRowHeight(int row) override {
    return row == k_dummyCellIndex
               ? m_dropdownCell.minimalSizeForOptimalDisplay().height()
               : InputCategoricalController::nonMemoizedRowHeight(row);
  }
  Escher::HighlightCell* reusableCell(int index, int type) override {
    return type == k_dummyCellIndex
               ? &m_dropdownCell
               : InputCategoricalController::reusableCell(index, type);
  }

  virtual char listPrefix(int column) const = 0;

  DropdownDataSource m_dropdownDataSource;
  DropdownCategoricalCell m_dropdownCell;
};

}  // namespace Inference

#endif
