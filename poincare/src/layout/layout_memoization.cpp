#include "layout_memoization.h"

#include <poincare/old/exception_checkpoint.h>
#include <poincare/src/layout/layout_cursor.h>

#include "rack_layout.h"
#include "render.h"

namespace Poincare::Internal {

KDSize LayoutMemoization::layoutSize(KDFont::Size font,
                                     LayoutCursor* cursor) const {
  if (!m_flags.m_sized || m_flags.m_sizeFontSize != font) {
    KDSize size = computeSize(font, cursor);

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
    if (size.height() >= Render::k_maxLayoutSize ||
        size.width() >= Render::k_maxLayoutSize) {
      ExceptionCheckpoint::Raise();
    }

    m_size = size;
    m_flags.m_sized = true;
    m_flags.m_sizeFontSize = font;
  }
  return m_size;
}

KDCoordinate LayoutMemoization::baseline(KDFont::Size font,
                                         LayoutCursor* cursor) const {
  if (!m_flags.m_baselined || m_flags.m_baselineFontSize != font) {
    m_baseline = computeBaseline(font, cursor);
    m_flags.m_baselined = true;
    m_flags.m_baselineFontSize = font;
  }
  return m_baseline;
}

void LayoutMemoization::invalidAllSizesPositionsAndBaselines() {
  m_flags.m_sized = false;
  m_flags.m_baselined = false;
}

}  // namespace Poincare::Internal
