#pragma once

#include <apps/i18n.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/metric.h>
#include <escher/solid_color_cell.h>

#include "categorical_table_view_data_source.h"
#include "table_dimensions.h"

namespace Inference {

/* This class wraps a TableViewDataSource by adding two Column headers & one Row
 * header around it. */
class TwoHeaderColumnsOneHeaderRowTableDataSource
    : public CategoricalTableViewDataSource {
 public:
  TwoHeaderColumnsOneHeaderRowTableDataSource();

  // TableViewDataSource
  int reusableCellCount(int type) const override;
  int typeAtLocation(int column, int row) const override;
  Escher::HighlightCell* reusableCell(int i, int type) override;

  bool canSelectCellAtLocation(int column, int row) override {
    return typeAtLocation(column, row) != k_typeOfTopLeftCell;
  }

  KDCoordinate separatorBeforeColumn(int column) const override;

  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;

 protected:
  constexpr static int k_typeOfTopLeftCell = k_typeOfHeaderCells + 1;

  int numberOfHeaderRows() const override { return 1; }
  int numberOfHeaderColumns() const override { return 2; }

  virtual ReusableCellCounts reusableCellCounts() const = 0;

  virtual Escher::HighlightCell* reusableHeaderCell(int i) = 0;
  virtual Escher::HighlightCell* reusableInnerCell(int i) = 0;

  void fillHeaderCellForLocation(Escher::HighlightCell* cell, int column,
                                 int row);

  virtual void fillColumnTitleForLocation(Escher::HighlightCell* cell,
                                          int innerColumn) = 0;
  virtual void fillRowTitleForLocation(Escher::HighlightCell* cell,
                                       int innerRow) = 0;
  virtual void fillRowSymbolForLocation(Escher::HighlightCell* cell,
                                        int innerRow) = 0;

  virtual void fillInnerCellForLocation(Escher::HighlightCell* cell,
                                        int innerColumn, int innerRow) = 0;

  Escher::SolidColorCell m_topLeftCell;
};

}  // namespace Inference
