#ifndef APPS_PROBABILITY_MODELS_RESULTS_DATA_SOURCE_H
#define APPS_PROBABILITY_MODELS_RESULTS_DATA_SOURCE_H

#include <apps/shared/button_with_separator.h>
#include <escher/list_view_data_source.h>
#include <escher/memoized_list_view_data_source.h>
#include <escher/message_table_cell_with_buffer.h>
#include <escher/responder.h>

#include "probability/abstract/button_delegate.h"
#include "probability/models/statistic/statistic.h"

namespace Probability {

class ResultsDataSource : public Escher::MemoizedListViewDataSource {
public:
  ResultsDataSource(Escher::Responder * parent, Statistic * statistic,
                    ButtonDelegate * delegate);
  int numberOfRows() const override {
    return 2 + m_statistic->hasDegreeOfFreedom() + 1;
  };
  KDCoordinate cellWidth() override { return 290; /* TODO */ };
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int i) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtIndex(int index) override;

private:
  constexpr static int k_indexOfZ = 0;
  constexpr static int k_indexOfP = 1;
  constexpr static int k_indexOfDegrees = 2;
  constexpr static int k_indexOfButton = 3;
  constexpr static int k_resultCellType = 0;
  constexpr static int k_buttonCellType = 1;
  Statistic * m_statistic;
  Escher::MessageTableCellWithBuffer m_resultCells[5];
  Shared::ButtonWithSeparator m_next;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_RESULTS_DATA_SOURCE_H */
