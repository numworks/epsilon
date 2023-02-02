#ifndef ESCHER_EXPRESSION_VIEW_H
#define ESCHER_EXPRESSION_VIEW_H

#include <escher/view.h>
#include <kandinsky/color.h>
#include <poincare/layout.h>
#include <poincare/layout_cursor.h>

namespace Escher {

/* This class does not handle the expression layout as the size of the layout is
 * needed to compute the size of table cells hosting the expression. As the size
 * of this cell is determined before we set the expression in the expression
 * view, we cannot use minimalSizeForOptimalDisplay to assess the required
 * size. */

class ExpressionView : public View {
public:
  ExpressionView(float horizontalAlignment = KDContext::k_alignLeft,
                 float verticalAlignment = KDContext::k_alignCenter,
                 KDColor textColor = KDColorBlack,
                 KDColor backgroundColor = KDColorWhite,
                 KDFont::Size font = KDFont::Size::Large,
                 Poincare::LayoutCursor * cursor = nullptr);
  Poincare::Layout layout() const { return m_layout; }
  bool setLayout(Poincare::Layout layout);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setBackgroundColor(KDColor backgroundColor);
  void setTextColor(KDColor textColor);
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  void setHorizontalMargin(KDCoordinate horizontalMargin) { m_horizontalMargin = horizontalMargin; }
  int numberOfLayouts() const;
  KDSize minimalSizeForOptimalDisplay() const override;
  KDPoint drawingOrigin() const;
  KDPoint absoluteDrawingOrigin() const;
  bool layoutHasNode() const { return Poincare::TreeNode::IsValidIdentifier(m_layout.identifier()) && !m_layout.wasErasedByException(); }
  KDFont::Size font() const { return m_font; }
  void setFont(KDFont::Size font) { m_font = font; }
protected:
  /* Warning: we do not need to delete the previous expression layout when
   * deleting object or setting a new expression layout. Indeed, the expression
   * layout is always possessed by a controller which only gives a pointer to
   * the expression view (without cloning it). The named controller is then
   * responsible for freeing the expression layout when required. */
  // TODO find better way to have minimalSizeForOptimalDisplay const
  mutable Poincare::Layout m_layout;
  Poincare::LayoutCursor * m_cursor;
  KDColor m_textColor;
  KDColor m_backgroundColor;
private:
  float m_horizontalAlignment;
  float m_verticalAlignment;
  KDCoordinate m_horizontalMargin;
  KDFont::Size m_font;
};

}

#endif
