#ifndef PROBABILITY_GUI_SLOPE_TABLE_CELL_H
#define PROBABILITY_GUI_SLOPE_TABLE_CELL_H

#include "inference/statistic/chi_square_and_slope/categorical_table_cell.h"
#include "inference/models/statistic/slope_t_statistic.h"
#include "shared/buffer_function_title_cell.h"

namespace Inference {

class SlopeTableCell : public DoubleColumnTableCell {
public:
  SlopeTableCell(Escher::Responder * parentResponder, DynamicSizeTableViewDataSourceDelegate * dynamicSizeTableViewDataSourceDelegate, Escher::SelectableTableViewDelegate * selectableTableViewDelegate, Statistic * statistic);

  constexpr static int k_numberOfReusableCells = SlopeTStatistic::k_maxNumberOfColumns * k_maxNumberOfReusableRows;

private:
  Escher::HighlightCell * headerCell(int index) override { return &m_header[index]; }

  // ClearColumnHelper
  // TODO
  int fillColumnName(int column, char * buffer) override { return 0; }

  Shared::BufferFunctionTitleCell m_header[SlopeTStatistic::k_maxNumberOfColumns];
};

}  // namespace Inference

#endif /* PROBABILITY_GUI_CATEGORICAL_TABLE_CELL_H */
