#ifndef ESCHER_LAYOUT_VIEW_H
#define ESCHER_LAYOUT_VIEW_H

#include <escher/glyphs_view.h>
#include <poincare/layout.h>
#include <poincare/layout_cursor.h>
#include <poincare_junior/src/layout/layout_cursor.h>

namespace Escher {

/* This class does not handle the layout as the size of the layout is needed to
 * compute the size of table cells hosting the layout. As the size of this cell
 * is determined before we set the layout in the layout view, we cannot use
 * minimalSizeForOptimalDisplay to assess the required size. */

class LayoutView : public GlyphsView {
 public:
  using TitleType = Poincare::Layout;

  LayoutView(KDGlyph::Format format = {})
      : GlyphsView(format), m_horizontalMargin(0) {}

  Poincare::Layout layout() const override { return m_layout; }
  bool setLayout(Poincare::Layout layout);
  void drawRect(KDContext* ctx, KDRect rect) const override;

  void setHorizontalMargin(KDCoordinate horizontalMargin) {
    m_horizontalMargin = horizontalMargin;
  }
  int numberOfLayouts() const;
  KDSize minimalSizeForOptimalDisplay() const override;
  KDPoint drawingOrigin() const;
  bool layoutHasNode() const {
    return Poincare::TreeNode::IsValidIdentifier(m_layout.identifier()) &&
           !m_layout.wasErasedByException();
  }

 protected:
  /* Warning: we do not need to delete the previous layout when deleting object
   * or setting a new layout. Indeed, the layout is always possessed by a
   * controller which only gives a pointer to the layout view (without cloning
   * it). The named controller is then responsible for freeing the layout when
   * required. */
  // TODO find better way to have draw const
  mutable Poincare::Layout m_layout;

 private:
  virtual PoincareJ::LayoutCursor* cursor() const { return nullptr; }
  KDCoordinate m_horizontalMargin;
};

class LayoutViewWithCursor : public LayoutView {
 public:
  LayoutViewWithCursor(PoincareJ::LayoutBufferCursor* cursor,
                       KDGlyph::Format format = {})
      : LayoutView(format), m_cursor(cursor) {
    assert(cursor);
  }

 private:
  Poincare::Layout layout() const override { return m_cursor->layoutBuffer(); }
  PoincareJ::LayoutCursor* cursor() const override { return m_cursor; }
  PoincareJ::LayoutBufferCursor* m_cursor;
};

}  // namespace Escher

#endif
