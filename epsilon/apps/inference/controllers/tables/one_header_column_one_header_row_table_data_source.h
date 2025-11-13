#pragma once

#include <apps/i18n.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/solid_color_cell.h>

#include "categorical_table_view_data_source.h"
#include "table_dimensions.h"

namespace Inference {

/* This class wraps a TableViewDataSource by adding a Row & Column header around
 * it. */
class OneHeaderColumnOneHeaderRowTableDataSource
    : public CategoricalTableViewDataSource {
 public:
  OneHeaderColumnOneHeaderRowTableDataSource();

  // TableViewDataSource
  int numberOfRows() const override { return innerNumberOfRows() + 1; }
  int numberOfColumns() const override { return innerNumberOfColumns() + 1; }
  int reusableCellCount(int type) const override;
  int typeAtLocation(int column, int row) const override;
  Escher::HighlightCell* reusableCell(int i, int type) override;

  bool canSelectCellAtLocation(int column, int row) override {
    return typeAtLocation(column, row) != k_typeOfTopLeftCell;
  }

 protected:
  constexpr static int k_typeOfTopLeftCell = k_typeOfHeaderCells + 1;

  virtual ReusableCellCounts reusableCellCounts() const = 0;

  virtual Escher::HighlightCell* reusableHeaderCell(int i) = 0;
  virtual Escher::HighlightCell* reusableInnerCell(int i) = 0;

  Escher::SolidColorCell m_topLeftCell;
};

}  // namespace Inference
