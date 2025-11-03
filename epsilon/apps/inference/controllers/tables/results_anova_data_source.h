#pragma once

#include <apps/i18n.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/solid_color_cell.h>

#include "anova_table_dimensions.h"
#include "categorical_table_view_data_source.h"
#include "dynamic_cells_data_source.h"
#include "inference/models/anova_test.h"

namespace Inference {

// TODO: factorize with HomogeneityResults

/* This class wraps a TableViewDataSource by adding a Row & Column header around
 * it. */
class ResultsANOVADataSource
    : public CategoricalTableViewDataSource,
      public DynamicCellsDataSource<Escher::SmallFontEvenOddBufferTextCell>,
      public DynamicCellsDataSourceDelegate<
          Escher::SmallFontEvenOddBufferTextCell> {
 public:
  ResultsANOVADataSource();

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

  void createCells() override;

  constexpr static int k_numberOfInnerColumns =
      ANOVATableDimensions::k_numberOfResultInnerColumns;
  constexpr static int k_numberOfInnerRows =
      ANOVATableDimensions::k_numberOfResultInnerRows;

 protected:
  KDCoordinate nonMemoizedColumnWidth(int column) override {
    return column == 0 ? ANOVATableDimensions::k_resultTitleColumnWidth
                       : ANOVATableDimensions::k_columnWidth;
  }
  virtual int innerNumberOfRows() const = 0;
  virtual int innerNumberOfColumns() const = 0;
  virtual void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                        int row) = 0;
  virtual Escher::HighlightCell* innerCell(int i) = 0;

 private:
  constexpr static int k_typeOfTopLeftCell = k_typeOfHeaderCells + 1;

  Escher::SolidColorCell m_topLeftCell;
};

}  // namespace Inference
