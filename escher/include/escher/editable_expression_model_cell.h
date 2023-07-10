#ifndef ESCHER_EDITABLE_EXPRESSION_MODEL_CELL_H
#define ESCHER_EDITABLE_EXPRESSION_MODEL_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/layout_field.h>
#include <kandinsky/margins.h>

namespace Escher {

// Copy-pasted from EvenOddExpressionCell waiting for templatisation

/* This cell is the editable counterpart of EvenOddExpressionCell, except that
 * it doesn't need to be even-odd since the background is always white.
 * Not be confused with EditableExpressionCell that is compatible with
 * toolbox-like menus and not expression models lists. */

class EditableExpressionModelCell : public HighlightCell {
 public:
  EditableExpressionModelCell(Responder* parentResponder = nullptr,
                              LayoutFieldDelegate* layoutDelegate = nullptr,
                              KDGlyph::Format format = {});
  void setLayout(Poincare::Layout layout) { m_layoutField.setLayout(layout); }
  void setTextColor(KDColor textColor) {
    m_layoutField.layoutView()->setTextColor(textColor);
  }
  void setFont(KDFont::Size font) { m_layoutField.layoutView()->setFont(font); }
  KDSize minimalSizeForOptimalDisplay() const override;
  void setAlignment(float horizontalAlignment, float verticalAlignment) {
    m_layoutField.layoutView()->setAlignment(horizontalAlignment,
                                             verticalAlignment);
  }
  void setMargins(KDHorizontalMargins margins);
  KDPoint drawingOrigin() const {
    return m_layoutField.layoutView()->drawingOrigin();
  }
  Poincare::Layout layout() const override { return m_layoutField.layout(); }
  KDFont::Size font() const { return m_layoutField.layoutView()->font(); }
  void drawRect(KDContext* ctx, KDRect rect) const override;
  LayoutField* layoutField() { return &m_layoutField; }

 protected:
  int numberOfSubviews() const override { return 1; }
  View* subviewAtIndex(int index) override { return &m_layoutField; }
  void layoutSubviews(bool force = false) override;
  LayoutField m_layoutField;
  KDHorizontalMargins m_margins;
};

}  // namespace Escher

#endif
