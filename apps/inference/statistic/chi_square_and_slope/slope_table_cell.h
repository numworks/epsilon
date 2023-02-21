#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_SLOPE_TABLE_CELL_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_SLOPE_TABLE_CELL_H

#include "inference/models/statistic/slope_t_statistic.h"
#include "inference/statistic/chi_square_and_slope/categorical_table_cell.h"
#include "shared/buffer_function_title_cell.h"
#include "shared/column_helper.h"

namespace Inference {

class SlopeTableCell : public DoubleColumnTableCell,
                       public Shared::StoreColumnHelper {
 public:
  SlopeTableCell(
      Escher::Responder *parentResponder,
      DynamicSizeTableViewDataSourceDelegate
          *dynamicSizeTableViewDataSourceDelegate,
      Escher::SelectableTableViewDelegate *selectableTableViewDelegate,
      Statistic *statistic, Poincare::Context *parentContext);

  constexpr static int k_numberOfReusableCells =
      SlopeTStatistic::k_maxNumberOfColumns * k_maxNumberOfReusableRows;

  void willAppear();

 private:
  Escher::HighlightCell *headerCell(int index) override {
    return &m_header[index];
  }

  // ClearColumnHelper
  int fillColumnName(int column, char *buffer) override {
    return fillColumnNameFromStore(column, buffer);
  }
  // StoreColumnHelper
  Shared::DoublePairStore *store() override {
    return static_cast<SlopeTStatistic *>(tableModel());
  }
  Escher::InputViewController *inputViewController() override;
  void reload() override;

  Shared::BufferFunctionTitleCell
      m_header[SlopeTStatistic::k_maxNumberOfColumns];
};

}  // namespace Inference

#endif
