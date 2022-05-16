#ifndef PROBABILITY_GUI_CATEGORICAL_TABLE_CELL_H
#define PROBABILITY_GUI_CATEGORICAL_TABLE_CELL_H

#include <escher/even_odd_editable_text_cell.h>
#include <escher/highlight_cell.h>
#include <escher/responder.h>
#include <escher/scroll_view.h>
#include <escher/selectable_table_view.h>
#include <shared/clear_column_helper.h>
#include <shared/parameter_text_field_delegate.h>
#include <shared/text_field_delegate.h>
#include "inference/statistic/chi_square_test/categorical_table_view_data_source.h"
#include "inference/shared/dynamic_cells_data_source.h"
#include "inference/models/statistic/chi2_test.h"

namespace Inference {

class CategoricalTableCell : public Escher::HighlightCell, public Escher::Responder, public Escher::SelectableTableViewDataSource, Escher::ScrollViewDelegate {
public:
  CategoricalTableCell(Escher::Responder * parentResponder, Escher::TableViewDataSource * dataSource, Escher::SelectableTableViewDelegate * selectableTableViewDelegate);

  // Responder
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;

  // View
  void drawRect(KDContext * ctx, KDRect rect) const override;

  // HighlightCell
  // Avoid reloading the whole table when
  void reloadCell() override {}
  Responder * responder() override { return this; }

  // ScrollViewDelegate
  void scrollViewDidChangeOffset(ScrollViewDataSource * scrollViewDataSource) override;

  Escher::SelectableTableView * selectableTableView() { return &m_selectableTableView; }
  virtual CategoricalTableViewDataSource * tableViewDataSource() = 0;

protected:
  constexpr static int k_bottomMargin = 5;

  void layoutSubviews(bool force = false) override;

  Escher::SelectableTableView m_selectableTableView;

private:
  int numberOfSubviews() const override { return 1; }
  Escher::View * subviewAtIndex(int i) override { return &m_selectableTableView; }
  KDSize minimalSizeForOptimalDisplay() const override { return m_selectableTableView.minimalSizeForOptimalDisplay(); }
};

class EditableCategoricalTableCell : public CategoricalTableCell, public Shared::TextFieldDelegate, public DynamicCellsDataSourceDelegate<Escher::EvenOddEditableTextCell>, public DynamicSizeTableViewDataSource, public Shared::ClearColumnHelper {
public:
  EditableCategoricalTableCell(Escher::Responder * parentResponder, Escher::TableViewDataSource * dataSource, Escher::SelectableTableViewDelegate * selectableTableViewDelegate, DynamicSizeTableViewDataSourceDelegate * dynamicSizeTableViewDelegate, Chi2Test * chi2Test);

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::TextField * textField, Ion::Events::Event event) override;
  virtual bool textFieldDidFinishEditing(Escher::TextField * textField, const char * text, Ion::Events::Event event) override;

  // Responder
  bool handleEvent(Ion::Events::Event e) override;

  // DynamicCellsDataSourceDelegate<Escher::EvenOddEditableTextCell>
  void initCell(Escher::EvenOddEditableTextCell, void * cell, int index) override;

  // DynamicSizeTableViewDataSource
  virtual bool recomputeDimensions(Chi2Test * test);

protected:
  // ClearColumnHelper
  Escher::SelectableTableView * table() override { return selectableTableView(); }
  int numberOfElementsInColumn(int column) const override;
  void clearSelectedColumn() override;
  bool isColumnClearable(int column) override { return relativeColumnIndex(column) >= 0; }

  virtual int relativeColumnIndex(int columnIndex) const = 0;
  int relativeRowIndex(int rowIndex) { return rowIndex - 1; }
  virtual bool deleteSelectedValue();

  Chi2Test * m_statistic;
};

}  // namespace Inference

#endif /* PROBABILITY_GUI_CATEGORICAL_TABLE_CELL_H */
