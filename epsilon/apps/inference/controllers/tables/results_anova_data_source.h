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
      public DynamicCellsDataSource<InferenceEvenOddBufferCell>,
      public DynamicCellsDataSourceDelegate<InferenceEvenOddBufferCell> {
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

  void initCell(InferenceEvenOddBufferCell, void* cell, int index) override;

  constexpr static int k_columnWidth = ANOVATableDimensions::k_columnWidth;

  constexpr static int k_numberOfReusableColumns =
      ANOVATableDimensions::k_numberOfInputColumns;
  // TODO: ANOVATableDimensions::k_numberOfResultInnerRows?
  constexpr static int k_maxNumberOfReusableRows =
      ANOVATableDimensions::k_numberOfInputInnerRows;

 protected:
  constexpr static int k_maxNumberOfColumns = ANOVATest::k_maxNumberOfColumns;
  // TODO: ANOVATableDimensions::k_numberOfResultInnerRows?
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
