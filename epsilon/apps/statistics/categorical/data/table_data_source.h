#pragma once

// TODO remove useless
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/highlight_cell.h>
#include <escher/solid_color_cell.h>
#include <escher/table_view_data_source.h>
#include <shared/buffer_function_title_cell.h>

#include "table_data.h"

namespace Statistics::Categorical {

// Why float here ?
using EvenOddBufferCell = Escher::FloatEvenOddBufferTextCell<
    Poincare::Preferences::ShortNumberOfSignificantDigits>;
using EvenOddEditableCell = Escher::EvenOddEditableTextCell<
    Poincare::Preferences::ShortNumberOfSignificantDigits>;

class TableViewDataSource : public Escher::TableViewDataSource {
 public:
  TableViewDataSource(Escher::Responder* editableCellsParentResponder,
                      Escher::TextFieldDelegate* textFieldDelegate)
      : m_topLeftCell(Escher::Palette::WallScreenDark) {
    for (EvenOddEditableCell& cell : m_editableCells) {
      cell.setParentResponder(editableCellsParentResponder);
      cell.editableTextCell()->textField()->setDelegate(textFieldDelegate);
    }
  }

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

  virtual void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                        int row) = 0;
  virtual TableData* tableData() = 0;

  int innerRow(int globalRow) const { return globalRow - 1; }
  int innerCol(int globalCol) const { return globalCol - 1; }

 protected:
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

  constexpr static int k_minRowOrCol = 3;
  int m_numberOfRows = k_minRowOrCol;
  int m_numberOfColumns = k_minRowOrCol;

 private:
  void prepareHeaderCell(Escher::HighlightCell* cell, int column);

  constexpr static int k_maxNumberOfHeaderCells = 5;
  constexpr static int k_maxNumberOfVerticalHeaderCells = 9;
  constexpr static int k_maxNumberOfReusableCells = 50;

  std::array<Shared::BufferFunctionTitleCell, k_maxNumberOfHeaderCells>
      m_headerCells;
  std::array<EvenOddBufferCell, k_maxNumberOfVerticalHeaderCells>
      m_verticalHeaderCells;
  std::array<EvenOddEditableCell, k_maxNumberOfReusableCells> m_editableCells;
  Escher::SolidColorCell m_topLeftCell;
};

}  // namespace Statistics::Categorical
