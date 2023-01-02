#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_TABLE_CELL_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_TABLE_CELL_H

#include <escher/even_odd_editable_text_cell.h>
#include <escher/highlight_cell.h>
#include <escher/responder.h>
#include <escher/scroll_view.h>
#include <escher/selectable_table_view.h>
#include <shared/column_helper.h>
#include <shared/parameter_text_field_delegate.h>
#include <shared/text_field_delegate.h>
#include "inference/statistic/chi_square_and_slope/categorical_table_view_data_source.h"
#include "inference/shared/dynamic_cells_data_source.h"
#include "inference/models/statistic/statistic.h"
#include "inference/models/statistic/table.h"

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

  KDSize minimalSizeForOptimalDisplay() const override { return m_selectableTableView.minimalSizeForOptimalDisplay(); }
protected:
  constexpr static int k_bottomMargin = 5;

  void layoutSubviews(bool force = false) override;

  Escher::SelectableTableView m_selectableTableView;

private:
  int numberOfSubviews() const override { return 1; }
  Escher::View * subviewAtIndex(int i) override { return &m_selectableTableView; }
};

class EditableCategoricalTableCell : public CategoricalTableCell, public Shared::TextFieldDelegate, public DynamicCellsDataSourceDelegate<Escher::EvenOddEditableTextCell>, public DynamicSizeTableViewDataSource, public Shared::ClearColumnHelper {
public:
  EditableCategoricalTableCell(Escher::Responder * parentResponder, Escher::TableViewDataSource * dataSource, Escher::SelectableTableViewDelegate * selectableTableViewDelegate, DynamicSizeTableViewDataSourceDelegate * dynamicSizeTableViewDelegate, Statistic * statistic);

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::AbstractTextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) override;

  // Responder
  bool handleEvent(Ion::Events::Event e) override;

  // DynamicCellsDataSourceDelegate<Escher::EvenOddEditableTextCell>
  void initCell(Escher::EvenOddEditableTextCell, void * cell, int index) override;

  // DynamicSizeTableViewDataSource
  virtual bool recomputeDimensions();

protected:
  // ClearColumnHelper
  Escher::SelectableTableView * table() override { return selectableTableView(); }
  int numberOfElementsInColumn(int column) const override;
  void clearSelectedColumn() override;
  bool isColumnClearable(int column) override { return relativeColumnIndex(column) >= 0; }

  virtual int relativeColumnIndex(int columnIndex) const = 0;
  int relativeRowIndex(int rowIndex) { return rowIndex - 1; }
  bool deleteSelectedValue();
  Table * tableModel();
  const Table * constTableModel() const { return const_cast<EditableCategoricalTableCell *>(this)->tableModel(); }

  Statistic * m_statistic;
};

class DoubleColumnTableCell : public EditableCategoricalTableCell, public CategoricalTableViewDataSource, public DynamicCellsDataSource<Escher::EvenOddEditableTextCell, k_doubleColumnTableNumberOfReusableCells> {
public:
  DoubleColumnTableCell(Escher::Responder * parentResponder, DynamicSizeTableViewDataSourceDelegate * dynamicSizeTableViewDataSourceDelegate, Escher::SelectableTableViewDelegate * selectableTableViewDelegate, Statistic * statistic);

  // EditableCategoricalTableCell
  CategoricalTableViewDataSource * tableViewDataSource() override { return this; }

  // DataSource
  int numberOfRows() const override { return m_numberOfRows + 1; } // Add header
  int numberOfColumns() const override { return m_numberOfColumns; }
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int i, int type) override;
  int typeAtLocation(int i, int j) override { return j == 0 ? k_typeOfHeaderCells : k_typeOfInnerCells; }
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;

  // DynamicCellsDataSource
  Escher::SelectableTableView * tableView() override { return &m_selectableTableView; }

  constexpr static int k_maxNumberOfColumns = 2;
  constexpr static int k_numberOfReusableCells = k_maxNumberOfColumns * k_maxNumberOfReusableRows;
private:
  KDCoordinate nonMemoizedColumnWidth(int i) override { return k_columnWidth; }
  // CategoricalTableViewDataSource
  int relativeColumnIndex(int columnIndex) const override { return columnIndex; }
  virtual Escher::HighlightCell * headerCell(int index) = 0;
};

}

#endif
