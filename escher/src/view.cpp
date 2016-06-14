extern "C" {
#include <assert.h>
}
#include <escher/view.h>

View::View() :
  m_superview(nullptr),
  m_frame(KDRectZero),
  m_needsRedraw(true)
{
}

void View::drawRect(KDRect rect) const {
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

void View::markAsNeedingRedraw() {
  // Let's mark ourself as needing redraw
  m_needsRedraw = true;

  /* And let's mark our parents as needing redraw too. The alternative would be
   * to have a recursive getter, which would be more resilient to superview
   * modification, but much slower too. As long as we don't change the view
   * hierarchy, this way is easier. */
  if (m_superview) {
    m_superview->markAsNeedingRedraw();
  }
}

void View::redraw(KDRect rect) {
  /* CAUTION: do NOT call redraw directly.
   * This may seem to work, but will not. Namely, it won't clip.
   * Example : our superview is smaller than we are. If we redraw ourself, we
   * will overflow our superview. */

  if (window() == nullptr || !m_needsRedraw) {
    return;
  }

  // First, let's draw our own content by calling drawRect
  KDSetDrawingArea(absoluteDrawingArea());
  this->drawRect(rect);

  // Then, let's recursively draw our children over ourself
  for (uint8_t i=0; i<numberOfSubviews(); i++) {
    View * subview = this->subview(i);
    if (subview == nullptr) {
      continue;
    }
    if (KDRectIntersect(rect, subview->m_frame)) {
      KDRect intersection = KDRectIntersection(rect, subview->m_frame);
      // Let's express intersection in subview's coordinates
      intersection.x -= subview->m_frame.x;
      intersection.y -= subview->m_frame.y;
      subview->redraw(intersection);
    }
  }

  // Eventually, mark that we don't need to be redrawn
  m_needsRedraw = false;
}

void View::setSubview(View * view, int index) {
  view->m_superview = this;
  storeSubviewAtIndex(view, index);
  assert(subview(index) == view);
  view->markAsNeedingRedraw();
}

/*
void View::addSubview(View * subview) {
  // Let's find a spot for that subview
  uint8_t i = 0;
  while (m_subviews[i] != nullptr) {
    i++;
    assert(i<k_maxNumberOfSubviews); // No room left!
  }

  subview->m_superview = this;
  m_subviews[i] = subview;

  // That subview needs to be drawn
  subview->redraw();
}
*/

/*
void View::removeFromSuperview() {
  assert(m_superview != nullptr);
  // First, remove the view from its parent hierarchy
  for (int i=0; i<m_superview->numberOfSubviews(); i++) {
    if (m_superview->subview(i) == this) {
      m_superview->storeSubviewAtIndex(nullptr, i);
      break;
    }
  }
  // Then, redraw the parent
  m_superview->redraw(m_frame);
  // Eventually clear the superview ivar
  m_superview = nullptr;
}
*/

void View::setFrame(KDRect frame) {
  // TODO: Return if frame is equal to m_frame
  m_frame = frame;
  if (m_superview != nullptr) {
    /* We have moved this view. This left a blank spot in its superview were it
     * previously was.
     * At this point, we know that the only area that really needs to be redrawn
     * in the superview is the value of m_frame at the start of that method.
     * However, let's not try to optimize too early, and let's simply mark the
     * whole superview as needing redraw. */
    m_superview->markAsNeedingRedraw();
  }
  layoutSubviews();
  markAsNeedingRedraw();
}

KDRect View::bounds() const {
  KDRect bounds = m_frame;
  bounds.x = 0;
  bounds.y = 0;
  return bounds;
}

KDRect View::absoluteDrawingArea() const {
  if (m_superview == nullptr) {
    return m_frame;
  } else {
    KDRect parentDrawingArea = m_superview->absoluteDrawingArea();

    KDRect absoluteFrame = m_frame;
    absoluteFrame.x += parentDrawingArea.x;
    absoluteFrame.y += parentDrawingArea.y;

    return KDRectIntersection(absoluteFrame, parentDrawingArea);
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

