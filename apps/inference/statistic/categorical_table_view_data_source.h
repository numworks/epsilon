#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_TABLE_VIEW_DATA_SOURCE_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_TABLE_VIEW_DATA_SOURCE_H

#include <escher/even_odd_cell.h>
#include <escher/table_view_data_source.h>

#include "inference/models/statistic/table.h"
#include "inference/text_helpers.h"

namespace Inference {

class CategoricalTableViewDataSource : public Escher::TableViewDataSource {
 public:
  // TableViewDataSource
  template <typename TextHolder>
  void fillValueCellForLocation(TextHolder* textHolder,
                                Escher::EvenOddCell* evenOddCell, int column,
                                int row, Table* tableModel) {
    double p = tableModel->valueAtPosition(row, column);
    PrintValueInTextHolder(p, textHolder);
    evenOddCell->setEven(row % 2 == 1);
  }
  bool canStoreCellAtLocation(int column, int row) override { return row > 0; }

  constexpr static int k_rowHeight = Escher::Metric::SmallEditableCellHeight;
  constexpr static int k_maxNumberOfReusableRows =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          k_rowHeight, Escher::Metric::StackTitleHeight);
  constexpr static int k_typeOfInnerCells = 0;

  // TableViewDataSource
  KDCoordinate separatorBeforeColumn(int column) const override {
    return column > 0 ? k_borderBetweenColumns : 0;
  }

 protected:
  constexpr static int k_typeOfHeaderCells = k_typeOfInnerCells + 1;

  constexpr static int k_borderBetweenColumns = 1;
  constexpr static int k_columnWidth =
      (Ion::Display::Width - 2 * Escher::Metric::CommonMargins.left() -
       k_borderBetweenColumns) /
      2;

  KDCoordinate nonMemoizedRowHeight(int row) override { return k_rowHeight; }
};

}  // namespace Inference

#endif
