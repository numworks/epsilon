#ifndef ESCHER_EDITABLE_FUNCTION_CELL_H
#define ESCHER_EDITABLE_FUNCTION_CELL_H

#include <escher/layout_field.h>

#include "function_cell.h"

namespace Graph {

class EditableFunctionCell : public AbstractFunctionCell {
 public:
  EditableFunctionCell(Escher::Responder* parentResponder,
                       Escher::InputEventHandlerDelegate* inputEventHandler,
                       Escher::LayoutFieldDelegate* layoutFieldDelegate);
  Escher::LayoutField* expressionField() { return &m_expressionField; }

 private:
  constexpr static KDCoordinate k_expressionMargin = 5;
  int numberOfSubviews() const override { return 2; }
  void layoutSubviews(bool force = false) override;
  void updateSubviewsBackgroundAfterChangingState() override;
  const Escher::ExpressionView* expressionView() const override {
    return m_expressionField.expressionView();
  }
  Escher::ExpressionView* expressionView() override {
    return m_expressionField.expressionView();
  }
  Escher::View* mainView() override { return &m_expressionField; }
  Escher::LayoutField m_expressionField;
};

}  // namespace Graph

#endif
