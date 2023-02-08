#ifndef ESCHER_EDITABLE_FUNCTION_CELL_H
#define ESCHER_EDITABLE_FUNCTION_CELL_H

#include "function_cell.h"
#include <escher/expression_field.h>


namespace Graph {

class EditableFunctionCell : public FunctionCell {
public:
  EditableFunctionCell(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandler, Escher::LayoutFieldDelegate * layoutFieldDelegate) : m_expressionField(parentResponder, inputEventHandler, layoutFieldDelegate) {}
  Escher::ExpressionField * expressionField() { return &m_expressionField; }
  KDSize minimalSizeForOptimalDisplay() const override;

private:
  int numberOfSubviews() const override { return 2; }
  Escher::View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  Escher::ExpressionField m_expressionField;
};

}

#endif
