#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_TABLE_VIEW_DATA_SOURCE_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_TABLE_VIEW_DATA_SOURCE_H

#include <escher/even_odd_cell.h>
#include "inference/statistic/chi_square_and_slope/dynamic_size_table_view_data_source.h"
#include "inference/statistic/chi_square_and_slope/bordered_table_view_data_source.h"
#include "inference/models/statistic/table.h"
#include "inference/text_helpers.h"

namespace Inference {

class CategoricalTableViewDataSource : public BorderedTableViewDataSource {
public:
  // TableViewDataSource
  template <typename TextHolder> void willDisplayValueCellAtLocation(TextHolder * textHolder, Escher::EvenOddCell * evenOddCell, int column, int row, Table * tableModel) {
    double p = tableModel->parameterAtPosition(row, column);
    PrintValueInTextHolder(p, textHolder);
    evenOddCell->setEven(row % 2 == 1);
  }
  KDCoordinate rowHeight(int j) override { return k_rowHeight; }

  // BorderedTableViewDataSource
  KDCoordinate verticalBorderWidth() override { return k_borderBetweenColumns; }

  constexpr static int k_rowHeight = Escher::Metric::SmallEditableCellHeight;
  constexpr static int k_maxNumberOfReusableRows = Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(k_rowHeight, Escher::Metric::StackTitleHeight);
  constexpr static int k_typeOfInnerCells = 0;
protected:
  constexpr static int k_typeOfHeaderCells = k_typeOfInnerCells + 1;

  constexpr static int k_borderBetweenColumns = 1;
  constexpr static int k_columnWidth = (Ion::Display::Width - 2 * Escher::Metric::CommonLeftMargin - k_borderBetweenColumns) / 2;
};

}  // namespace Inference

#endif /* INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_TABLE_VIEW_DATA_SOURCE_H */
