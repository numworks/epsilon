#include "memoized_cursor_view.h"

#include <kandinsky/ion_context.h>
#include "dots.h"

namespace Shared {

template <KDCoordinate Size>
void MemoizedCursorView<Size>::drawRect(KDContext * ctx, KDRect rect) const {
  KDRect r = bounds();
  /* Beware that only the pixels of the intersection of rect with KDContext's
   * clipping rect are pulled. All other pixels are left unaltered. Indeed
   * nothing outside the clipping rect should be redrawn and hence was not
   * dirty.
   */
  ctx->getPixels(r, m_underneathPixelBuffer);
  m_underneathPixelBufferLoaded = true;
  drawCursor(ctx, rect);
}

template <KDCoordinate Size>
KDSize MemoizedCursorView<Size>::minimalSizeForOptimalDisplay() const {
  return KDSize(Size, Size);
}

template <KDCoordinate Size>
void MemoizedCursorView<Size>::setColor(KDColor color) {
  m_color = color;
  eraseCursorIfPossible();
  markRectAsDirty(bounds());
}

template <KDCoordinate Size>
void MemoizedCursorView<Size>::setCursorFrame(KDRect f, bool force) {
  /* TODO This is quite dirty (we are out of the dirty tracking and we assume
   * the cursor is the upmost view) but it works well. */
  if (m_frame == f && !force) {
    return;
  }
  /* We want to avoid drawing the curve just because the cursor has been
   * repositioned, as it is very slow for non cartesian curves.*/
  if (eraseCursorIfPossible()) {
    // Set the frame
    m_frame = f;
    markRectAsDirty(bounds());
    return;
  }
  CursorView::setCursorFrame(f, force);
}

template <KDCoordinate Size>
void MemoizedCursorView<Size>::markRectAsDirty(KDRect rect) {
  /* The CursorView class inherits from TransparentView, so does
   * MemoizedCursorView. The method markRectAsDirty is thus overriden to avoid
   * marking as dirty the backgmemoized of the MemoizedCursorView in its superview.
   */
  View::markRectAsDirty(rect);
}

template <KDCoordinate Size>
bool MemoizedCursorView<Size>::eraseCursorIfPossible() {
  if (!m_underneathPixelBufferLoaded) {
    return false;
  }
  const KDRect currentFrame = absoluteVisibleFrame();
  if (currentFrame.isEmpty()) {
    return false;
  }
  // Erase the cursor
  KDColor cursorWorkingBuffer[Size * Size];
  KDContext * ctx = KDIonContext::SharedContext();
  ctx->setOrigin(absoluteOrigin());
  ctx->setClippingRect(currentFrame);
  KDSize cursorSize = KDSize(Size, Size);
  ctx->fillRectWithPixels(KDRect(0, 0, cursorSize), m_underneathPixelBuffer, cursorWorkingBuffer);
  // TODO Restore the context to previous values?
  return true;
}

template class MemoizedCursorView<Dots::LargeDotDiameter>;
template class MemoizedCursorView<Dots::LargeRingDiameter>;
}

