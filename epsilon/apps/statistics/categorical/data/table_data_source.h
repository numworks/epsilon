#pragma once

// TODO remove useless
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/highlight_cell.h>
#include <escher/solid_color_cell.h>
#include <escher/table_view_data_source.h>
#include <shared/buffer_function_title_cell.h>

#include "store.h"

namespace Statistics::Categorical {

/* DataSource for the categorical table. Contains the reusable cells used in the
 * table and fills them accordingly */
class TableViewDataSource : public Escher::TableViewDataSource {
  static constexpr int k_numberOfSignificantDigits =
      Poincare::Preferences::ShortNumberOfSignificantDigits;

  using EvenOddBufferCell =
      Escher::FloatEvenOddBufferTextCell<k_numberOfSignificantDigits>;
  using EvenOddEditableCell =
      Escher::EvenOddEditableTextCell<k_numberOfSignificantDigits>;

 public:
  TableViewDataSource(Escher::Responder* editableCellsParentResponder,
                      Escher::TextFieldDelegate* textFieldDelegate);
  // Escher::TableViewDataSource
  int numberOfRows() const override { return m_numberOfRows; }
  int numberOfColumns() const override { return m_numberOfColumns; }
  Escher::HighlightCell* reusableCell(int i, int type) override;
  int reusableCellCount(int type) const override;
  int typeAtLocation(int column, int row) const override;
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;
  bool canSelectCellAtLocation(int column, int row) override {
    return typeAtLocation(column, row) != k_typeOfTopLeftCell;
  }

 protected:
  virtual const Store* store() const = 0;

  struct ColumnInfo {
    bool isDataColumn;
    int groupNumber;
  };
  ColumnInfo columnInfo(int col) const;
  /* Given a column in the selectableTable returns the corresponding
   * column/group in the tableData.
   * Asserts that the given column is not a RF column */
  int dataColumn(int col);
  int dataRow(int row) const {
    assert(row > 0);
    return row - 1;
  }

  // Shared::TableViewDataSource
  constexpr static int k_rowHeight = Escher::Metric::SmallEditableCellHeight;
  KDCoordinate nonMemoizedRowHeight(int row) override { return k_rowHeight; }
  // NOTE for now, same as Shared::EditableCellTableViewController::k_cellWidth;
  constexpr static int k_columnWidth = 102;
  KDCoordinate nonMemoizedColumnWidth(int column) override {
    return k_columnWidth;
  }

  constexpr static int k_typeOfInnerCells = 0;
  constexpr static int k_typeOfHeaderCells = k_typeOfInnerCells + 1;
  constexpr static int k_typeOfVerticalHeaderCells = k_typeOfHeaderCells + 1;
  constexpr static int k_typeOfTopLeftCell = k_typeOfVerticalHeaderCells + 1;
  constexpr static int k_typeOfRFCells = k_typeOfTopLeftCell + 1;

  int m_numberOfRows;
  int m_numberOfColumns;

 private:
  void prepareHeaderCell(Escher::HighlightCell* cell, int column);

  constexpr static int k_maxNumberOfHeaderCells = 5;
  constexpr static int k_maxNumberOfVerticalHeaderCells = 9;
  constexpr static int k_maxNumberOfNonEditableHeaderCells = 18;
  constexpr static int k_maxNumberOfEditableCells = 32;

  std::array<Shared::BufferFunctionTitleCell, k_maxNumberOfHeaderCells>
      m_headerCells;
  std::array<EvenOddBufferCell, k_maxNumberOfVerticalHeaderCells>
      m_verticalHeaderCells;
  std::array<EvenOddBufferCell, k_maxNumberOfNonEditableHeaderCells>
      m_nonEditableCells;
  std::array<EvenOddEditableCell, k_maxNumberOfEditableCells> m_editableCells;
  Escher::SolidColorCell m_topLeftCell;
};

}  // namespace Statistics::Categorical
