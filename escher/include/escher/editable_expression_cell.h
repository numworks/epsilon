#ifndef ESCHER_EDITABLE_EXPRESSION_CELL_H
#define ESCHER_EDITABLE_EXPRESSION_CELL_H

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
                         LayoutFieldDelegate * layoutDelegate = nullptr,
                         KDFont::Size font = KDFont::Size::Large,
                         float horizontalAlignment = KDContext::k_alignLeft,
                         float verticalAlignment = KDContext::k_alignCenter,
                         KDColor expressionColor = KDColorBlack,
                         KDColor = KDColorWhite,
                         KDCoordinate topMargin = 0,
                         KDCoordinate rightMargin = 0,
                         KDCoordinate bottomMargin = 0,
                         KDCoordinate leftMargin = 0);
  ExpressionField * expressionField();
  void setMargins(KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0, KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0);
  void setHighlighted(bool highlight) override;
  Responder * responder() override {
    return this;
  }
  // const char * expression() const override {
    // if (!m_expressionField.isEditing()) {
      // return m_expressionField.expression();
    // }
    // return nullptr;
  // }
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  void didBecomeFirstResponder() override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_separatorThickness = Metric::CellSeparatorThickness;
  ExpressionField m_expressionField;
  char m_expressionBody[Poincare::PrintFloat::k_maxFloatCharSize];
  KDCoordinate m_topMargin;
  KDCoordinate m_rightMargin;
  KDCoordinate m_bottomMargin;
  KDCoordinate m_leftMargin;
};

}

#endif
