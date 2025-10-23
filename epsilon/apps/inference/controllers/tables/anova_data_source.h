#pragma once

#include <apps/i18n.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/solid_color_cell.h>

#include <algorithm>

#include "categorical_table_view_data_source.h"
#include "dynamic_cells_data_source.h"
#include "inference/models/anova_test.h"

namespace Inference {

namespace ANOVATableDimensions {
// TODO: shared value with ChiSquare / Homogeneity
constexpr static int k_columnWidth = 82;
// TODO: is this valid?
//  We take into account the potential "Total" column
constexpr static int k_numberOfReusableColumns =
    std::min(Ion::Display::Width / k_columnWidth + 2,
             ANOVATest::k_maxNumberOfColumns + 1);
// TODO: is this valid?
// We take into account the potential "Total" row
constexpr static int k_maxNumberOfReusableRows =
    std::min(CategoricalTableViewDataSource::k_maxNumberOfReusableRows,
             ANOVATest::k_maxNumberOfRows + 1);
// 5 * 10
constexpr static int k_numberOfInnerReusableCells =
    k_maxNumberOfReusableRows * k_numberOfReusableColumns;

constexpr static int k_numberOfHeaderReusableCells =
    k_numberOfReusableColumns + k_maxNumberOfReusableRows - 1;
}  // namespace ANOVATableDimensions

using ANOVAHeaderCellsDataSource =
    DynamicCellsDataSource<InferenceEvenOddBufferCell,
                           ANOVATableDimensions::k_numberOfHeaderReusableCells>;

using InputANOVAInnerCellsDataSource =
    DynamicCellsDataSource<InferenceEvenOddEditableCell,
                           ANOVATableDimensions::k_numberOfInnerReusableCells>;

/* This class wraps a TableViewDataSource by adding a Row & Column header around
 * it. Specifically meant for InputHomogeneity and HomogeneityResults. */
class ANOVATableDataSource
    : public CategoricalTableViewDataSource,
      public ANOVAHeaderCellsDataSource,
      public DynamicCellsDataSourceDelegate<InferenceEvenOddBufferCell> {
 public:
  ANOVATableDataSource();

  // TableViewDataSource
  int numberOfRows() const override { return innerNumberOfRows() + 1; }
  int numberOfColumns() const override { return innerNumberOfColumns() + 1; }
  int reusableCellCount(int type) const override;
  int typeAtLocation(int column, int row) const override;
  Escher::HighlightCell* reusableCell(int i, int type) override;
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;
  bool canSelectCellAtLocation(int column, int row) override {
    return typeAtLocation(column, row) != k_typeOfTopLeftCell;
  }

  // DynamicCellsDataSource
  void initCell(InferenceEvenOddBufferCell, void* cell, int index) override;

  constexpr static int k_columnWidth = ANOVATableDimensions::k_columnWidth;
  constexpr static int k_numberOfReusableColumns =
      ANOVATableDimensions::k_numberOfReusableColumns;
  constexpr static int k_maxNumberOfReusableRows =
      ANOVATableDimensions::k_maxNumberOfReusableRows;

 protected:
  constexpr static int k_maxNumberOfColumns = ANOVATest::k_maxNumberOfColumns;
  constexpr static int k_maxNumberOfRows = ANOVATest::k_maxNumberOfRows;

  KDCoordinate nonMemoizedColumnWidth(int column) override {
    return k_columnWidth;
  }
  virtual int innerNumberOfRows() const = 0;
  virtual int innerNumberOfColumns() const = 0;
  virtual void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                        int row) = 0;
  virtual Escher::HighlightCell* innerCell(int i) = 0;

 private:
  constexpr static int k_typeOfTopLeftCell = k_typeOfHeaderCells + 1;
  constexpr static int k_headerTranslationBuffer = 20;

  I18n::Message m_headerPrefix;
  Escher::SolidColorCell m_topLeftCell;
};

}  // namespace Inference
