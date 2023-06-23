#ifndef ESCHER_EDITABLE_FUNCTION_CELL_H
#define ESCHER_EDITABLE_FUNCTION_CELL_H

#include <escher/editable_expression_model_cell.h>

#include "function_cell.h"

namespace Graph {

class EditableFunctionCell
    : public TemplatedFunctionCell<Shared::WithEditableExpressionCell> {
 public:
  EditableFunctionCell(Escher::Responder* parentResponder,
                       Escher::LayoutFieldDelegate* layoutFieldDelegate);

  void updateSubviewsBackgroundAfterChangingState() override {
    m_expressionBackground = backgroundColor();
  }

 private:
  int numberOfSubviews() const override { return 2; }
  void layoutSubviews(bool force = false) override;
  static constexpr KDCoordinate k_expressionMargin = 5;
};

}  // namespace Graph

#endif
