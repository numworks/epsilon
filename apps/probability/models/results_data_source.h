#ifndef APPS_PROBABILITY_MODELS_RESULTS_DATA_SOURCE_H
#define APPS_PROBABILITY_MODELS_RESULTS_DATA_SOURCE_H

#include <apps/shared/button_with_separator.h>
#include <escher/memoized_list_view_data_source.h>

#include "probability/abstract/button_delegate.h"
#include "probability/gui/layout_cell_with_buffer_with_message.h"
#include "probability/models/statistic/statistic.h"

namespace Probability {

/* A ResultsDataSource is a TableViewDataSource which is meant to represent
 * data provided (and computed) by a Statistic.
 */
class ResultsDataSource : public Escher::MemoizedListViewDataSource {
public:
  ResultsDataSource(Escher::Responder * parent, Statistic * statistic, ButtonDelegate * delegate);
  int numberOfRows() const override;
  KDCoordinate cellWidth() override { return 290; /* TODO */ };
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int i) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtIndex(int index) override;

private:
  enum TestCellOrder { Z, PValue, TestDegree };
  enum IntervalCellOrder { Estimate, Critical, SE, ME, IntervalDegree };
  constexpr static int k_resultCellType = 0;
  constexpr static int k_buttonCellType = 1;
  constexpr static int k_numberOfReusableCells = 5;
  Statistic * m_statistic;
  LayoutCellWithBufferWithMessage m_resultCells[k_numberOfReusableCells];
  Shared::ButtonWithSeparator m_next;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_RESULTS_DATA_SOURCE_H */
