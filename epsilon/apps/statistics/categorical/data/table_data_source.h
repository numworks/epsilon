#pragma once

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
class TableViewDataSource : public Escher::TableViewDataSource,
                            public Escher::RegularTableSize1DManager {
  static constexpr int k_numberOfSignificantDigits =
      Escher::AbstractEvenOddBufferTextCell::k_defaultPrecision;

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
  /* Given a column in the selectableTable, returns the corresponding
   * column/group in the tableData.
   * Asserts that the given column is not a RF column. */
  int dataColumn(int col);
  int dataRow(int row) const {
    assert(row > 0);
    return row - 1;
  }

  // Shared::TableViewDataSource
  constexpr static int k_rowHeight = Escher::Metric::SmallEditableCellHeight;
  KDCoordinate defaultRowHeight() override { return k_rowHeight; }
  // NOTE for now, same as Shared::EditableCellTableViewController::k_cellWidth;
  constexpr static int k_columnWidth = 102;
  KDCoordinate defaultColumnWidth() override { return k_columnWidth; }

  constexpr static int k_typeOfInnerCells = 0;
  constexpr static int k_typeOfHeaderCells = k_typeOfInnerCells + 1;
  constexpr static int k_typeOfVerticalHeaderCells = k_typeOfHeaderCells + 1;
  constexpr static int k_typeOfTopLeftCell = k_typeOfVerticalHeaderCells + 1;
  constexpr static int k_typeOfRFCells = k_typeOfTopLeftCell + 1;

  int m_numberOfRows;
  int m_numberOfColumns;

 private:
  void prepareHeaderCell(Escher::HighlightCell* cell, int column);
  Escher::TableSize1DManager* columnWidthManager() override { return this; }
  Escher::TableSize1DManager* rowHeightManager() override { return this; }

  constexpr static int k_maxNumberOfHeaderCells =
      (Ion::Display::Width - k_columnWidth) / k_columnWidth + 2;
  // TODO: Avoid these two harcoded heights
  constexpr static int k_statisticsTabHeight = 22;
  constexpr static int k_typeOfDataRowHeight = 26;
  // Account for title bar, statistics tab, type of data tab and header cell row
  constexpr static int k_availableHeigthForVerticalHeaderCells =
      Ion::Display::Height - Escher::Metric::TitleBarHeight -
      k_statisticsTabHeight - k_typeOfDataRowHeight - k_rowHeight;
  /* [Cropped cells] + [available height] / [row height]
   * TODO: Only 1 cropped cell should be visible in practice */
  constexpr static int k_maxNumberOfVerticalHeaderCells =
      2 + k_availableHeigthForVerticalHeaderCells / k_rowHeight;

  constexpr static int k_maxNumberOfEditableCells =
      k_maxNumberOfHeaderCells * k_maxNumberOfVerticalHeaderCells;
  constexpr static int k_maxNumberOfNonEditableCells =
      (k_maxNumberOfHeaderCells + 1) * k_maxNumberOfVerticalHeaderCells / 2;

  std::array<Shared::BufferFunctionTitleCell, k_maxNumberOfHeaderCells>
      m_headerCells;
  std::array<EvenOddBufferCell, k_maxNumberOfVerticalHeaderCells>
      m_verticalHeaderCells;
  std::array<EvenOddBufferCell, k_maxNumberOfNonEditableCells>
      m_nonEditableCells;
  std::array<EvenOddEditableCell, k_maxNumberOfEditableCells> m_editableCells;
  Escher::SolidColorCell m_topLeftCell;
};

}  // namespace Statistics::Categorical
