#ifndef APPS_PROBABILITY_GUI_COMPARISON_OPERATOR_POPUP_DATA_SOURCE_H
#define APPS_PROBABILITY_GUI_COMPARISON_OPERATOR_POPUP_DATA_SOURCE_H

#include <escher/list_view_data_source.h>

#include "probability/constants.h"
#include "probability/models/hypothesis_params.h"
#include "text_highlight_view.h"

namespace Probability {

class ComparisonOperatorPopupDataSource : public Escher::ListViewDataSource {
public:
  ComparisonOperatorPopupDataSource(HypothesisParams * hypothesisParams) :
      m_hypothesisParams(hypothesisParams) {}
  int numberOfRows() const override { return k_numberOfOperators; }
  KDCoordinate rowHeight(int r) override { return 0; }
  int reusableCellCount(int type) override { return k_numberOfOperators; }
  TextHighlightView * reusableCell(int i, int type) override { return &m_cells[i]; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

private:
  constexpr static int k_cellBufferSize = 5 /* p1-p2 */ + 2 /* op */ +
                                          Constants::k_shortBufferSize /* float */;
  constexpr static int k_numberOfOperators = 3;
  TextHighlightView m_cells[k_numberOfOperators];
  HypothesisParams * m_hypothesisParams;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_COMPARISON_OPERATOR_POPUP_DATA_SOURCE_H */
