#ifndef ESCHER_EVEN_ODD_EDITABLE_EXPRESSION_CELL_H
#define ESCHER_EVEN_ODD_EDITABLE_EXPRESSION_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/expression_field.h>

namespace Escher {

// Copy-pasted from EvenOddExpressionCell waiting for templatisation

class EvenOddEditableExpressionCell : public EvenOddCell {
public:
  EvenOddEditableExpressionCell(
    Responder * parentResponder = nullptr,
    InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr,
    LayoutFieldDelegate * layoutDelegate = nullptr,
    float horizontalAlignment = KDContext::k_alignLeft,
    float verticalAlignment = KDContext::k_alignCenter,
    KDColor textColor = KDColorBlack,
    KDColor backgroundColor = KDColorWhite,
    KDFont::Size font = KDFont::Size::Large);
  void setLayout(Poincare::Layout layout);
  void setBackgroundColor(KDColor backgroundColor);
  void setTextColor(KDColor textColor);
  void setFont(KDFont::Size font) { m_expressionField.expressionView()->setFont(font); }
  KDSize minimalSizeForOptimalDisplay() const override;
  void setAlignment(float horizontalAlignment, float verticalAlignment) { m_expressionField.expressionView()->setAlignment(horizontalAlignment, verticalAlignment); }
  void setLeftMargin(KDCoordinate margin);
  void setRightMargin(KDCoordinate margin);
  KDPoint drawingOrigin() const { return m_expressionField.expressionView()->drawingOrigin(); }
  Poincare::Layout layout() const override { return m_expressionField.layout(); }
  KDFont::Size font() const { return m_expressionField.expressionView()->font(); }
  void drawRect(KDContext * ctx, KDRect rect) const override;
  ExpressionField * expressionField() { return &m_expressionField; }

protected:
  void updateSubviewsBackgroundAfterChangingState() override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  ExpressionField m_expressionField;
  KDCoordinate m_leftMargin;
  KDCoordinate m_rightMargin;
};

}

#endif
