#pragma once

#include <apps/i18n.h>
#include <escher/even_odd_buffer_text_cell.h>

#include "categorical_table_view_data_source.h"
#include "table_dimensions.h"

namespace Inference {

// This class wraps a TableViewDataSource by adding a Column header above it.
class OneHeaderRowTableDataSource : public CategoricalTableViewDataSource {
 public:
  // TableViewDataSource
  int numberOfRows() const override { return innerNumberOfRows() + 1; }
  int numberOfColumns() const override { return innerNumberOfColumns(); }
  int reusableCellCount(int type) const override;
  int typeAtLocation(int column, int row) const override;
  Escher::HighlightCell* reusableCell(int i, int type) override;

  virtual int maxNumberOfRows() const = 0;
  virtual int maxNumberOfColumn() const = 0;

 protected:
  virtual ReusableCellCounts reusableCellCounts() const = 0;

  virtual Escher::HighlightCell* reusableHeaderCell(int i) = 0;
  virtual Escher::HighlightCell* reusableInnerCell(int i) = 0;
};

}  // namespace Inference
