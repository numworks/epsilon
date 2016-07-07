extern "C" {
#include <assert.h>
}
#include <escher/view.h>

View::View() :
  m_superview(nullptr),
  m_frame(KDRectZero),
  m_dirtyRect(KDRectZero)
{
}

void View::drawRect(KDContext * ctx, KDRect rect) const {
  // By default, a view doesn't do anything
  // It's transparent!
}

const Window * View::window() const {
  if (m_superview == nullptr) {
    return nullptr;
  } else {
    return m_superview->window();
  }
}

void View::markRectAsDirty(KDRect rect) {
  m_dirtyRect = m_dirtyRect.unionedWith(rect);
}

void View::redraw(KDRect rect) {
  if (window() == nullptr) {
    /* That view (and all of its subviews) is offscreen. That means so are all
     * of its subviews. So there's no point in drawing them. */
    return;
  }

  // First, let's draw our own content by calling drawRect
  KDRect rectNeedingRedraw = rect.intersectedWith(m_dirtyRect);
  if (!rectNeedingRedraw.isEmpty()) {
    KDPoint absOrigin = absoluteOrigin();
    KDRect absRect = rectNeedingRedraw.translatedBy(absOrigin);
    KDRect absClippingRect = absoluteVisibleFrame().intersectedWith(absRect);
    KDContext * ctx = KDIonContext::sharedContext();
    ctx->setOrigin(absOrigin);
    ctx->setClippingRect(absClippingRect);
    this->drawRect(ctx, rectNeedingRedraw);
  }

  // Then, let's recursively draw our children over ourself
  for (uint8_t i=0; i<numberOfSubviews(); i++) {
    View * subview = this->subview(i);
    if (subview == nullptr) {
      continue;
    }
    assert(subview->m_superview == this);

    if (rect.intersects(subview->m_frame)) {
      KDRect intersection = rect.intersectedWith(subview->m_frame);
      // Let's express intersection in subview's coordinates
      intersection = intersection.translatedBy(subview->m_frame.origin().opposite());
      subview->redraw(intersection);
    }
  }

  // Eventually, mark that we don't need to be redrawn
  m_dirtyRect = KDRectZero;
}

View * View::subview(int index) {
  assert(index < numberOfSubviews());
  View * subview = subviewAtIndex(index);
  if (subview != nullptr) {
    subview->m_superview = this;
  }
  return subview;
}

void View::setFrame(KDRect frame) {
  /* CAUTION: This code is not resilient to multiple consecutive setFrame()
   * calls without intermediate redraw() calls. */

  // TODO: Return if frame is equal to m_frame
  if (m_superview != nullptr) {
    /* We will move this view. This will leave a blank spot in its superview
     * were it previously was.
     * At this point, we know that the only area that really needs to be redrawn
     * in the superview is the value of m_frame at the start of that method. */
    m_superview->markRectAsDirty(m_frame);
  }

  m_frame = frame;

  /* Now that we have moved, we have also dirtied our new absolute frame.
   * There are two ways to declare this, which are semantically equivalent: we
   * can either mark an area of our superview as dirty, or mark our whole frame
   * as dirty. We pick the second option because it is more efficient. */
  markRectAsDirty(bounds());

  layoutSubviews();
}

KDRect View::bounds() const {
  return m_frame.movedTo(KDPointZero);
}

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
const char * View::className() const {
  return "View";
}

void View::logAttributes(std::ostream &os) const {
  os << " address=\"" << this << "\"";
  os << " frame=\"" << m_frame.x << "," << m_frame.y << "," << m_frame.width << "," << m_frame.height << "\"";
}

std::ostream &operator<<(std::ostream &os, View &view) {
  os << "<" << view.className();
  view.logAttributes(os);
  os << ">";
  for (int i=0; i<view.numberOfSubviews(); i++) {
    assert(view.subview(i)->m_superview == &view);
    os << *view.subview(i);
  }
  os << "</" << view.className() << ">";
  return os;
}
#endif

