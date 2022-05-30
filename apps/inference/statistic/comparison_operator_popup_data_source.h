#ifndef INFERENCE_STATISTIC_COMPARISON_OPERATOR_POPUP_DATA_SOURCE_H
#define INFERENCE_STATISTIC_COMPARISON_OPERATOR_POPUP_DATA_SOURCE_H

#include <escher/list_view_data_source.h>

#include <escher/buffer_text_highlight_cell.h>
#include "inference/constants.h"
#include "inference/models/statistic/test.h"

namespace Inference {

class ComparisonOperatorPopupDataSource : public Escher::ListViewDataSource {
public:
  ComparisonOperatorPopupDataSource(Test * test) : m_test(test) {}
  int numberOfRows() const override { return k_numberOfOperators; }
  KDCoordinate rowHeight(int r) override {
    assert(false); /* Not needed because DropdownPopupController takes care of it */
    return 0;
  }
  int reusableCellCount(int type) override { return k_numberOfOperators; }
  Escher::BufferTextHighlightCell * reusableCell(int i, int type) override { return &m_cells[i]; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

private:
  constexpr static int k_cellBufferSize = 7 /* μ1-μ2 */ + 3 /* op ≠ */ +
                                          Constants::k_shortFloatNumberOfChars /* float */ +
                                          1 /* \0 */;
  constexpr static int k_numberOfOperators = 3;
  Escher::BufferTextHighlightCell m_cells[k_numberOfOperators];
  Test * m_test;
};

}  // namespace Inference

#endif /* INFERENCE_STATISTIC_COMPARISON_OPERATOR_POPUP_DATA_SOURCE_H */
