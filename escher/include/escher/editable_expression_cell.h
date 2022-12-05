#ifndef ESCHER_EDITABLE_EXPRESSION_CELL_H
#define ESCHER_EDITABLE_EXPRESSION_CELL_H

#include <escher/bordered.h>
#include <escher/responder.h>
#include <escher/highlight_cell.h>
#include <escher/metric.h>
#include <escher/text_field_delegate.h>
#include <escher/layout_field_delegate.h>
#include <escher/expression_field.h>
#include <poincare/print_float.h>

namespace Escher {

class EditableExpressionCell : public Bordered, public HighlightCell, public Responder {
public:
  EditableExpressionCell(Responder * parentResponder = nullptr,
                         InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr,
                         TextFieldDelegate * textDelegate = nullptr,
                         LayoutFieldDelegate * layoutDelegate = nullptr);
  ExpressionField * expressionField() { return &m_expressionField; }
  void setHighlighted(bool highlight) override;
  Responder * responder() override {
    return this;
  }
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int index) override { return &m_expressionField; }
  void layoutSubviews(bool force = false) override;
  void didBecomeFirstResponder() override;
  KDSize minimalSizeForOptimalDisplay() const override { return m_expressionField.minimalSizeForOptimalDisplay(); }
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_separatorThickness = Metric::CellSeparatorThickness;
  ExpressionField m_expressionField;
  char m_expressionBody[Poincare::PrintFloat::k_maxFloatCharSize];
};

}

#endif
