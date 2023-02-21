#include <escher/view.h>
#include <kandinsky/ion_context.h>

extern "C" {
#include <assert.h>
}

namespace Escher {

const Window *View::window() const {
  if (m_superview == nullptr) {
    return nullptr;
  } else {
    return m_superview->window();
  }
}

void View::markRectAsDirty(KDRect rect) {
  m_dirtyRect = m_dirtyRect.unionedWith(rect);
}

KDRect View::redraw(KDRect rect, KDRect forceRedrawRect) {
  /* View::redraw recursively redraws the rectangle 'rect' of the view and all
   * its subviews.
   * To optimize the function, we redraw only the union of the current dirty
   * rectangle with a rectangle forced to be redrawn (forceRedrawRect). This
   * rectangle is initially empty and recursively expands by unioning with the
   * rectangles that are redrawn. This process handles the case when several
   * sister views are overlapping (provided that the sister views are indexed in
   * the right order).
   */
  if (window() == nullptr) {
    /* That view (and all of its subviews) is offscreen. That means so are all
     * of its subviews. So there's no point in drawing them. */
    return KDRectZero;
  }

  /* First, for the current view, the rectangle to redraw is the union of the
   * dirty rectangle and the rectangle forced to be redrawn. The rectangle to
   * redraw must also be included in the current view bounds and in the
   * rectangle rect. */
  KDRect rectNeedingRedraw =
      rect.intersectedWith(m_dirtyRect)
          .unionedWith(forceRedrawRect.intersectedWith(bounds()));

  // This redraws the rectNeedingRedraw calling drawRect.
  if (!rectNeedingRedraw.isEmpty()) {
    KDPoint absOrigin = absoluteOrigin();
    KDRect absRect = rectNeedingRedraw.translatedBy(absOrigin);
    KDRect absClippingRect = absoluteVisibleFrame().intersectedWith(absRect);
    KDContext *ctx = KDIonContext::SharedContext;
    ctx->setOrigin(absOrigin);
    ctx->setClippingRect(absClippingRect);
    this->drawRect(ctx, rectNeedingRedraw);
  }
  // This initializes the area that has been redrawn.
  KDRect redrawnArea = rectNeedingRedraw;

  // Then, let's recursively draw our children over ourself
  uint8_t subviewsNumber = numberOfSubviews();
  for (uint8_t i = 0; i < subviewsNumber; i++) {
    View *subview = this->subview(i);
    if (subview == nullptr) {
      continue;
    }
    assert(subview->m_superview == this);

    // We transpose rect and forcedRedrawArea in the subview coordinates.
    KDRect intersectionInSubview =
        rect.intersectedWith(subview->m_frame)
            .translatedBy(subview->m_frame.origin().opposite());
    KDRect forcedRedrawAreaInSubview =
        redrawnArea.translatedBy(subview->m_frame.origin().opposite());

    // We redraw the current subview by passing the rectangle previously redrawn
    // (by the parent view or previous sister views) as forced to be redraw.
    KDRect subviewRedrawnArea =
        subview->redraw(intersectionInSubview, forcedRedrawAreaInSubview);

    // We expand the redrawn area to include the area just drawn.
    redrawnArea = redrawnArea.unionedWith(
        subviewRedrawnArea.translatedBy(subview->m_frame.origin()));
  }
  // Eventually, mark that we don't need to be redrawn
  m_dirtyRect = KDRectZero;

  // The function returns the total area that have been redrawn.
  return redrawnArea;
}

View *View::subview(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  View *subview = subviewAtIndex(index);
  if (subview != nullptr) {
    subview->m_superview = this;
  }
  return subview;
}

void View::setSize(KDSize size) {
  setFrame(KDRect(m_frame.origin(), size), false);
}

void View::setFrame(KDRect frame, bool force) {
  if (frame == m_frame && !force) {
    return;
  }
  /* CAUTION: This code is not resilient to multiple consecutive setFrame()
   * calls without intermediate redraw() calls. */

  if (m_superview != nullptr) {
    /* We will move this view. This will leave a blank spot in its superview
     * where it previously was.
     * At this point, we know that the only area that needs to be redrawn in the
     * superview is the old frame minus the part covered by the new frame.*/
    m_superview->markRectAsDirty(m_frame.differencedWith(frame));
  }

  m_frame = frame;

  /* Now that we have moved, we have also dirtied our new absolute frame.
   * There are two ways to declare this, which are semantically equivalent: we
   * can either mark an area of our superview as dirty, or mark our whole frame
   * as dirty. We pick the second option because it is more efficient. */
  markRectAsDirty(bounds());
  // FIXME: m_dirtyRect = bounds(); would be more correct (in case the view is
  // being shrinked)

  if (!m_frame.isEmpty()) {
    layoutSubviews(force);
  }
}

KDPoint View::pointFromPointInView(View *view, KDPoint point) {
  return point.translatedBy(
      view->absoluteOrigin().translatedBy(absoluteOrigin().opposite()));
}

KDRect View::bounds() const { return m_frame.movedTo(KDPointZero); }

KDPoint View::absoluteOrigin() const {
  if (m_superview == nullptr) {
    assert(this == (View *)window());
    return m_frame.origin();
  } else {
    return m_frame.origin().translatedBy(m_superview->absoluteOrigin());
  }
}

KDRect View::absoluteVisibleFrame() const {
  if (m_superview == nullptr) {
    assert(this == (View *)window());
    return m_frame;
  } else {
    KDRect parentDrawingArea = m_superview->absoluteVisibleFrame();

    KDRect absoluteFrame = m_frame.movedTo(absoluteOrigin());

    return absoluteFrame.intersectedWith(parentDrawingArea);
  }
}

#if ESCHER_VIEW_LOGGING
const char *View::className() const { return "View"; }

void View::logAttributes(std::ostream &os) const {
  os << " address=\"" << this << "\"";
  os << " frame=\"" << m_frame.x() << "," << m_frame.y() << ","
     << m_frame.width() << "," << m_frame.height() << "\"";
}

std::ostream &operator<<(std::ostream &os, View &view) {
  // TODO find something better than this static variable, like a custom stream
  static int indentColumn = 0;
  constexpr static int bufferSize = 200;
  ;
  char indent[bufferSize];
  for (int i = 0; i < indentColumn; i++) {
    indent[i] = ' ';
  }
  indent[indentColumn] = 0;
  os << indent << "<" << view.className();
  view.logAttributes(os);
  if (view.numberOfSubviews()) {
    os << ">\n";
    for (int i = 0; i < view.numberOfSubviews(); i++) {
      indentColumn += 2;
      assert(view.subview(i)->m_superview == &view);
      os << *view.subview(i) << '\n';
      indentColumn -= 2;
    }
    os << indent << "</" << view.className() << ">";
  } else {
    os << "/>";
  }
  return os;
}

void View::log() const {
  // TODO << should use a const View but it requires a const subview
  std::cout << *const_cast<View *>(this);
}
#endif

}  // namespace Escher
