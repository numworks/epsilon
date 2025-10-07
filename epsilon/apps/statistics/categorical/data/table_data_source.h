#pragma once

// TODO remove useless
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/highlight_cell.h>
#include <escher/solid_color_cell.h>
#include <escher/table_view_data_source.h>

namespace Statistics::Categorical {

// Why float here ?
using EvenOddBufferCell = Escher::FloatEvenOddBufferTextCell<
    Poincare::Preferences::ShortNumberOfSignificantDigits>;
using EvenOddEditableCell = Escher::EvenOddEditableTextCell<
    Poincare::Preferences::ShortNumberOfSignificantDigits>;

class TableViewDataSource : public Escher::TableViewDataSource {
 public:
  TableViewDataSource() : m_topLeftCell(Escher::Palette::WallScreenDark) {}

  int reusableCellCount(int type) const override;
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;
  virtual void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                        int row) = 0;
  bool canSelectCellAtLocation(int column, int row) override {
    return typeAtLocation(column, row) != k_typeOfTopLeftCell;
  }

  int typeAtLocation(int column, int row) const override;
  Escher::HighlightCell* reusableCell(int i, int type) override;

  int numberOfRows() const override { return m_numberOfRows; }
  int numberOfColumns() const override { return m_numberOfColumns; }

 protected:
  constexpr static int k_rowHeight = Escher::Metric::SmallEditableCellHeight;
  KDCoordinate nonMemoizedRowHeight(int row) override { return k_rowHeight; }
  constexpr static int k_columnWidth = 82;
  KDCoordinate nonMemoizedColumnWidth(int column) override {
    return k_columnWidth;
  }
  constexpr static int k_typeOfInnerCells = 0;
  constexpr static int k_typeOfHeaderCells = k_typeOfInnerCells + 1;
  constexpr static int k_typeOfVerticalHeaderCells = k_typeOfHeaderCells + 1;
  constexpr static int k_typeOfTopLeftCell = k_typeOfVerticalHeaderCells + 1;

  constexpr static int k_maxNumberOfHeaderCells = 5;
  constexpr static int k_maxNumberOfVerticalHeaderCells = 9;

  // Escher::EvenOddBufferTextCell<
  //     Poincare::Preferences::ShortNumberOfSignificantDigits>
  //     m_titleCells[k_maxNumberOfDisplayableColumns];
  constexpr static int k_maxNumberOfReusableCells = 50;

  /* NOTE: we use 2 distinct types for the two header (even though they are the
   * same) as it's makes the preface work seamlessly */
  std::array<EvenOddBufferCell, k_maxNumberOfHeaderCells> m_headerCells;
  std::array<EvenOddBufferCell, k_maxNumberOfVerticalHeaderCells>
      m_verticalHeaderCells;
  std::array<EvenOddEditableCell, k_maxNumberOfReusableCells> m_editableCells;

  constexpr static int k_minRowOrCol = 3;
  int m_numberOfRows = k_minRowOrCol;
  int m_numberOfColumns = k_minRowOrCol;

 private:
  Escher::SolidColorCell m_topLeftCell;
};

}  // namespace Statistics::Categorical
