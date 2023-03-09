#ifndef ESCHER_EDITABLE_EXPRESSION_MODEL_CELL_H
#define ESCHER_EDITABLE_EXPRESSION_MODEL_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/expression_field.h>

namespace Escher {

// Copy-pasted from EvenOddExpressionCell waiting for templatisation

/* This cell is the editable counterpart of EvenOddExpressionCell, except that
 * it doesn't need to be even-odd since the background is always white.
 * Not be confused with EditableExpressionCell that is compatible with
 * toolbox-like menus and not expression models lists. */

class EditableExpressionModelCell : public HighlightCell {
 public:
  EditableExpressionModelCell(
      Responder* parentResponder,
      InputEventHandlerDelegate* inputEventHandlerDelegate,
      LayoutFieldDelegate* layoutDelegate,
      float horizontalAlignment = KDContext::k_alignLeft,
      float verticalAlignment = KDContext::k_alignCenter,
      KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite,
      KDFont::Size font = KDFont::Size::Large);
  void setLayout(Poincare::Layout layout) {
    m_expressionField.setLayout(layout);
  }
  void setTextColor(KDColor textColor) {
    m_expressionField.expressionView()->setTextColor(textColor);
  }
  void setFont(KDFont::Size font) {
    m_expressionField.expressionView()->setFont(font);
  }
  KDSize minimalSizeForOptimalDisplay() const override;
  void setAlignment(float horizontalAlignment, float verticalAlignment) {
    m_expressionField.expressionView()->setAlignment(horizontalAlignment,
                                                     verticalAlignment);
  }
  void setMargins(KDCoordinate leftMargin, KDCoordinate rightMargin);
  KDPoint drawingOrigin() const {
    return m_expressionField.expressionView()->drawingOrigin();
  }
  Poincare::Layout layout() const override {
    return m_expressionField.layout();
  }
  KDFont::Size font() const {
    return m_expressionField.expressionView()->font();
  }
  void drawRect(KDContext* ctx, KDRect rect) const override;
  ExpressionField* expressionField() { return &m_expressionField; }

 protected:
  int numberOfSubviews() const override { return 1; }
  View* subviewAtIndex(int index) override { return &m_expressionField; }
  void layoutSubviews(bool force = false) override;
  ExpressionField m_expressionField;
  KDCoordinate m_leftMargin;
  KDCoordinate m_rightMargin;
};

}  // namespace Escher

#endif
