#ifndef INFERENCE_STATISTIC_COMPARISON_OPERATOR_POPUP_DATA_SOURCE_H
#define INFERENCE_STATISTIC_COMPARISON_OPERATOR_POPUP_DATA_SOURCE_H

#include <escher/buffer_text_highlight_cell.h>
#include <escher/explicit_list_view_data_source.h>

#include "inference/constants.h"
#include "inference/models/statistic/test.h"

namespace Inference {

class ComparisonOperatorPopupDataSource
    : public Escher::ExplicitListViewDataSource {
 public:
  static Poincare::ComparisonNode::OperatorType OperatorTypeForRow(int row);

  ComparisonOperatorPopupDataSource(Test* test) : m_test(test) {}
  int numberOfRows() const override { return k_numberOfOperators; }
  Escher::HighlightCell* cell(int row) override { return &m_cells[row]; }
  void updateMessages();

 private:
  constexpr static int k_numberOfOperators = 3;
  Escher::SmallBufferTextHighlightCell m_cells[k_numberOfOperators];
  Test* m_test;
};

}  // namespace Inference

#endif
