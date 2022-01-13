#ifndef PROBABILITY_GUI_COMPARISON_OPERATOR_POPUP_DATA_SOURCE_H
#define PROBABILITY_GUI_COMPARISON_OPERATOR_POPUP_DATA_SOURCE_H

#include <escher/list_view_data_source.h>

#include "buffer_text_highlight_cell.h"
#include "probability/constants.h"
#include "probability/models/hypothesis_params.h"

namespace Probability {

class ComparisonOperatorPopupDataSource : public Escher::ListViewDataSource {
public:
  ComparisonOperatorPopupDataSource(HypothesisParams * hypothesisParams) :
        m_hypothesisParams(hypothesisParams) {}
  int numberOfRows() const override { return k_numberOfOperators; }
  KDCoordinate rowHeight(int r) override {
    assert(false); /* Not needed because DropdownPopupController takes care of it */
    return 0;
  }
  int reusableCellCount(int type) override { return k_numberOfOperators; }
  BufferTextHighlightCell * reusableCell(int i, int type) override { return &m_cells[i]; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

private:
  constexpr static int k_cellBufferSize = 7 /* μ1-μ2 */ + 3 /* op ≠ */ +
                                          Constants::k_shortFloatNumberOfChars /* float */ +
                                          1 /* \0 */;
  constexpr static int k_numberOfOperators = 3;
  BufferTextHighlightCell m_cells[k_numberOfOperators];
  HypothesisParams * m_hypothesisParams;
};

}  // namespace Probability

#endif /* PROBABILITY_GUI_COMPARISON_OPERATOR_POPUP_DATA_SOURCE_H */
