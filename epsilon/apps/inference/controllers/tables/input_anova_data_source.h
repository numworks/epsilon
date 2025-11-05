#pragma once

#include <apps/i18n.h>
#include <escher/even_odd_buffer_text_cell.h>

#include "anova_table_dimensions.h"
#include "categorical_table_view_data_source.h"
#include "inference/models/anova_test.h"

namespace Inference {

// This class wraps a TableViewDataSource by adding a Column header around it.
class InputANOVADataSource : public CategoricalTableViewDataSource {
 public:
  InputANOVADataSource() = default;

  // TableViewDataSource
  int numberOfRows() const override { return innerNumberOfRows() + 1; }
  int numberOfColumns() const override { return innerNumberOfColumns(); }
  int reusableCellCount(int type) const override;
  int typeAtLocation(int column, int row) const override;
  Escher::HighlightCell* reusableCell(int i, int type) override;

  constexpr static int k_columnWidth = ANOVATableDimensions::k_columnWidth;
  constexpr static int k_numberOfReusableColumns =
      ANOVATableDimensions::k_numberOfInputColumns;
  constexpr static int k_maxNumberOfReusableRows =
      ANOVATableDimensions::k_numberOfInputInnerRows;

 protected:
  constexpr static int k_maxNumberOfColumns = ANOVATest::k_maxNumberOfColumns;
  constexpr static int k_maxNumberOfInnerRows = ANOVATest::k_maxNumberOfRows;
  constexpr static int k_maxNumberOfRows = k_maxNumberOfInnerRows + 1;

  KDCoordinate nonMemoizedColumnWidth(int column) override {
    assert(column >= 0 && column < k_maxNumberOfColumns);
    return k_columnWidth;
  }

  virtual Escher::HighlightCell* reusableHeaderCell(int i) = 0;
  virtual Escher::HighlightCell* reusableInnerCell(int i) = 0;
};

}  // namespace Inference
