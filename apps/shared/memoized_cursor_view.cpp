#include "memoized_cursor_view.h"

#include <kandinsky/ion_context.h>

#include "dots.h"

namespace Shared {

void MemoizedCursorView::drawRect(KDContext* ctx, KDRect rect) const {
  KDRect r = bounds();
  /* Beware that only the pixels of the intersection of rect with KDContext's
   * clipping rect are pulled. All other pixels are left unaltered. Indeed
   * nothing outside the clipping rect should be redrawn and hence was not
   * dirty.
   */
  ctx->getPixels(r, underneathPixelBuffer());
  m_underneathPixelBufferLoaded = true;
  drawCursor(ctx, rect);
}

KDSize MemoizedCursorView::minimalSizeForOptimalDisplay() const {
  return KDSize(size(), size());
}

void MemoizedCursorView::setColor(KDColor color) {
  m_color = color;
  eraseCursorIfPossible();
  markRectAsDirty(bounds());
}

void MemoizedCursorView::setCursorFrame(View* parent, KDRect f, bool force) {
  /* TODO This is quite dirty (we are out of the dirty tracking and we assume
   * the cursor is the upmost view) but it works well. */
  if (parent->relativeChildFrame(this) == f && !force) {
    return;
  }
  /* We want to avoid drawing the curve just because the cursor has been
   * repositioned, as it is very slow for non cartesian curves.*/
  if (eraseCursorIfPossible()) {
    // Set the frame
    m_frame = f.translatedBy(parent->absoluteOrigin());
    markRectAsDirty(bounds());
    return;
  }
  CursorView::setCursorFrame(parent, f, force);
}

void MemoizedCursorView::markRectAsDirty(KDRect rect) {
  /* The CursorView class inherits from TransparentView, so does
   * MemoizedCursorView. The method markRectAsDirty is thus overriden to avoid
   * marking as dirty the backgmemoized of the MemoizedCursorView in its
   * superview.
   */
  View::markRectAsDirty(rect);
}

bool MemoizedCursorView::eraseCursorIfPossible() {
  if (!m_underneathPixelBufferLoaded) {
    return false;
  }
  const KDRect currentFrame = absoluteFrame();
  if (currentFrame.isEmpty()) {
    return false;
  }
  // Erase the cursor
  KDColor cursorWorkingBuffer[size() * size()];
  KDContext* ctx = KDIonContext::SharedContext;
  KDPoint previousOrigin = ctx->origin();
  KDRect previousClippingRect = ctx->clippingRect();
  ctx->setOrigin(absoluteOrigin());
  ctx->setClippingRect(currentFrame);
  KDSize cursorSize = KDSize(size(), size());
  ctx->fillRectWithPixels(KDRect(0, 0, cursorSize), underneathPixelBuffer(),
                          cursorWorkingBuffer);
  ctx->setOrigin(previousOrigin);
  ctx->setClippingRect(previousClippingRect);
  return true;
}

void MemoizedCursorView::redrawCursor(KDRect rect) {
  KDContext* ctx = KDIonContext::SharedContext;
  KDPoint previousOrigin = ctx->origin();
  KDRect previousClippingRect = ctx->clippingRect();
  redraw(rect);
  ctx->setOrigin(previousOrigin);
  ctx->setClippingRect(previousClippingRect);
}

}  // namespace Shared
