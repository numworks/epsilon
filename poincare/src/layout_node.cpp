#include <escher/metric.h>
#include <ion/display.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/layout_node.h>
#include <poincare/old_expression.h>
#include <poincare_junior/src/layout/rack_layout.h>

namespace Poincare {

#define Layout OLayout

bool LayoutNode::isIdenticalTo(const Layout l, bool makeEditable) const {
  if (makeEditable) {
    assert(false);
  }
  if (l.isUninitialized()) {
    return false;
  }
  if (identifier() == l.identifier()) {
    return true;
  }
  return protectedIsIdenticalTo(l);
}

KDSize LayoutNode::layoutSize(KDFont::Size font,
                              PoincareJ::LayoutCursor *cursor) const {
  if (!m_flags.m_sized || m_flags.m_sizeFontSize != font) {
    PoincareJ::RackLayout::s_layoutCursor = cursor;
    KDSize size = computeSize(font);

    /* This method will raise an exception if the size of the layout that is
     * passed is beyond k_maxLayoutSize.
     * The purpose of this hack is to avoid overflowing KDCoordinate when
     * drawing a layout.
     *
     * Currently, the only layouts that can overflow KDCoordinate without
     * overflowing the pool are:
     *  - the derivative layouts (if multiple derivative layouts are nested
     *    inside the variable layout or the order layout)
     *  - the horizontal layouts (when a very long list is generated through a
     *    sequence and each child is large).
     * This two sepific cases are handled in their own computeSize methods but
     * we still do this check for other layouts.
     *
     * Raising an exception might not be the best option though. We could just
     * handle the max size better and simply crop the layout (which is not that
     * easy to implement), instead of raising an exception.
     *
     * The following solutions were also explored but deemed too complicated for
     * the current state of the issue:
     *  - Make all KDCoordinate int32 and convert at the last moment into int16,
     * only when stored as m_variable.
     *  - Rewrite KDCoordinate::operator+ so that KDCOORDINATE_MAX is returned
     * if the + overflows.
     *  - Forbid insertion of a large layout as the child of another layout.
     *  - Check for an overflow before each translation of p in the render
     * methods*/
    if (size.height() >= k_maxLayoutSize || size.width() >= k_maxLayoutSize) {
      ExceptionCheckpoint::Raise();
    }

    m_size = size;
    m_flags.m_sized = true;
    m_flags.m_sizeFontSize = font;
  }
  return m_size;
}

KDCoordinate LayoutNode::baseline(KDFont::Size font,
                                  PoincareJ::LayoutCursor *cursor) const {
  if (!m_flags.m_baselined || m_flags.m_baselineFontSize != font) {
    PoincareJ::RackLayout::s_layoutCursor = cursor;
    m_baseline = computeBaseline(font);
    m_flags.m_baselined = true;
    m_flags.m_baselineFontSize = font;
  }
  return m_baseline;
}

void LayoutNode::invalidAllSizesPositionsAndBaselines() {
  m_flags.m_sized = false;
  m_flags.m_baselined = false;
}

}  // namespace Poincare
