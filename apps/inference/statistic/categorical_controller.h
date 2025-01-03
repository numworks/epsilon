#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_CONTROLLER_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_CONTROLLER_H

#include <escher/button_cell.h>

#include "categorical_cell.h"
#include "categorical_table_cell.h"
#include "inference/models/statistic/chi2_test.h"

namespace Inference {

/* Common controller between InputHomogeneityController,
 * ResultsHomogeneityTabController, InputGoodnessController,
 * ResultsGoodnessController.
 * A CategoricalController is a table whose first cell
 * is also a table of class CategoricalTableCell. */

class CategoricalController
    : public Escher::SelectableListViewController<Escher::ListViewDataSource>,
      public Escher::ScrollViewDataSourceDelegate,
      public Escher::ScrollViewDelegate,
      public Escher::SelectableListViewDelegate {
 public:
  CategoricalController(Escher::Responder* parent,
                        Escher::ViewController* nextController,
                        Escher::Invocation invocation);

  static bool ButtonAction(CategoricalController* controller, void* s);

  // ScrollViewDataSourceDelegate
  bool updateBarIndicator(bool vertical, bool* visible) override;

  // SelectableListViewDelegate
  void listViewDidChangeSelectionAndDidScroll(
      Escher::SelectableListView* l, int previousRow, KDPoint previousOffset,
      bool withinTemporarySelection = false) override;

  // ListViewDataSource
  // Like an ExplicitListViewDataSource (only table cell is not explicit)
  int typeAtRow(int row) const override final { return row; }
  int numberOfRows() const override { return indexOfNextCell() + 1; }
  Escher::HighlightCell* reusableCell(int index, int type) override final;
  int reusableCellCount(int type) const override final { return 1; }
  void fillCellForRow(Escher::HighlightCell* cell, int row) override final {}
  void initWidth(Escher::TableView* tableView) override;
  KDCoordinate separatorBeforeRow(int row) const override {
    return row == indexOfNextCell() ? k_defaultRowSeparator : 0;
  }

  void initView() override;

  virtual int indexOfTableCell() const { return 0; }
  void didScroll();

  void setNextController(Escher::ViewController* controller) {
    m_nextController = controller;
  }

 protected:
  KDCoordinate nonMemoizedRowHeight(int row) override final;

  virtual Escher::HighlightCell* explicitCellAtRow(int row);
  virtual const Escher::HighlightCell* explicitCellAtRow(int row) const;
  virtual int indexOfNextCell() const { return indexOfTableCell() + 1; }
  virtual CategoricalTableCell* categoricalTableCell() = 0;

  Escher::ViewController* m_nextController;
  ButtonCategoricalCell m_next;

 private:
  // ScrollViewDelegate
  KDRect visibleRectInBounds(Escher::ScrollView* scrollView) override;

  virtual void createDynamicCells() = 0;

  KDCoordinate listVerticalOffset() {
    return m_selectableListView.contentOffset().y();
  }
  KDCoordinate tableCellVerticalOffset() {
    return categoricalTableCell()->selectableTableView()->contentOffset().y();
  }
  KDCoordinate tableCellFullHeight() {
    return categoricalTableCell()->contentSizeWithMargins().height();
  }
};

/* Common Controller between InputHomogeneityController and
 * InputGoodnessController. */

class InputCategoricalController : public CategoricalController,
                                   public Shared::ParameterTextFieldDelegate {
 public:
  InputCategoricalController(Escher::StackViewController* parent,
                             Escher::ViewController* nextController,
                             Statistic* statistic,
                             Escher::Invocation invocation);

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
  Escher::ViewController::TitlesDisplay titlesDisplay() const override {
    return Escher::ViewController::TitlesDisplay::DisplayLastTitle;
  }

 protected:
  const Escher::HighlightCell* explicitCellAtRow(int row) const override;
  Escher::HighlightCell* explicitCellAtRow(int row) override;
  InputCategoricalTableCell* categoricalTableCell() override = 0;
  virtual int indexOfSignificanceCell() const = 0;
  int indexOfNextCell() const override { return indexOfSignificanceCell() + 1; }
  virtual int indexOfEditedParameterAtIndex(int index) const {
    assert(index == indexOfSignificanceCell());
    return m_statistic->indexOfThreshold();
  }

  Statistic* m_statistic;
  InputCategoricalCell<Escher::MessageTextView> m_significanceCell;
};

}  // namespace Inference

#endif
