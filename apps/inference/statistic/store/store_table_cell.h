#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_SLOPE_TABLE_CELL_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_SLOPE_TABLE_CELL_H

#include "inference/models/statistic/slope_t_statistic.h"
#include "inference/statistic/categorical_table_cell.h"
#include "shared/buffer_function_title_cell.h"
#include "shared/column_helper.h"

namespace Inference {

class InputStoreController;

class StoreTableCell : public DoubleColumnTableCell,
                       public Shared::StoreColumnHelper {
 public:
  StoreTableCell(Escher::Responder *parentResponder, Statistic *statistic,
                 Poincare::Context *parentContext,
                 InputStoreController *inputStoreController);

  constexpr static int k_numberOfReusableCells =
      Table::k_maxNumberOfStoreColumns * k_maxNumberOfReusableRows;

  void fillColumnsNames();

  // StoreColumnHelper
  SlopeTStatistic *store() override {
    return static_cast<SlopeTStatistic *>(tableModel());
  }
  const SlopeTStatistic *store() const {
    return const_cast<StoreTableCell *>(this)->store();
  }

 private:
  Escher::HighlightCell *headerCell(int index) override {
    return &m_header[index];
  }

  // ClearColumnHelper
  size_t fillColumnName(int column, char *buffer) override {
    return fillColumnNameFromStore(
        k_maxNumberOfColumns * store()->series() + column, buffer);
  }
  Escher::InputViewController *inputViewController() override;
  void reload() override;
  CategoricalController *categoricalController() override;

  Shared::BufferFunctionTitleCell m_header[Table::k_maxNumberOfStoreColumns];
  InputStoreController *m_inputStoreController;
};

}  // namespace Inference

#endif
