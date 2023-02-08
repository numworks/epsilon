#ifndef ESCHER_EDITABLE_FUNCTION_CELL_H
#define ESCHER_EDITABLE_FUNCTION_CELL_H

#include "function_cell.h"
#include <escher/expression_field.h>


namespace Graph {

class EditableFunctionCell : public AbstractFunctionCell {
public:
  EditableFunctionCell(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandler, Escher::LayoutFieldDelegate * layoutFieldDelegate);
  Escher::ExpressionField * expressionField() { return &m_expressionField; }

private:
  constexpr static KDCoordinate k_expressionMargin = 5;
  int numberOfSubviews() const override { return 2; }
  void layoutSubviews(bool force = false) override;
  const Escher::ExpressionView * expressionView() const override { return m_expressionField.expressionView(); }
  Escher::ExpressionView * expressionView() override { return m_expressionField.expressionView(); }
  Escher::View * mainView() override { return &m_expressionField; }
  Escher::ExpressionField m_expressionField;
};

}

#endif
