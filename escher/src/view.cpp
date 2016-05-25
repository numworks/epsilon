extern "C" {
#include <assert.h>
}
#include <escher/view.h>

View::View() :
  m_superview(nullptr),
  m_frame(KDRectZero)
{
  /*
  for (uint8_t i=0; i<k_maxNumberOfSubviews; i++) {
    m_subviews[i] = nullptr;
  }
  */
}

void View::drawRect(KDRect rect) {
  // By default, a view doesn't do anything
  // It's transparent!
}

bool View::isOnScreen() {
  if (m_superview == nullptr) {
    return false;
  } else {
    return m_superview->isOnScreen();
  }
}

void View::redraw() {
  redraw(bounds());
}

void View::redraw(KDRect rect) {
  if (!isOnScreen()) {
    return;
  }
  // Fisrt, let's draw our own content by calling drawRect
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
}

  void setSubview(View * v, int index);


void View::setSubview(View * view, int index) {
  view->m_superview = this;
  storeSubviewAtIndex(view, index);
  assert(subview(index) == view);
  layoutSubviews();
  redraw();
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

void View::removeFromSuperview() {
  assert(m_superview != nullptr);
  // First, remove the view from its parent hierarchy
  assert(false); // FIXME: Unimplemented
  // Then, redraw the parent
  m_superview->redraw(m_frame);
}
*/

void View::setFrame(KDRect frame) {
 // TODO: Return if frame is equal to m_frame
 KDRect previousFrame = m_frame;
 m_frame = frame;
 if (m_superview != nullptr) {
   /* We have moved this view. This left a blank spot in its superview were it
    * previously was. So let's redraw that part of the superview. */
   m_superview->redraw(previousFrame);
 }
 layoutSubviews();
 // The view now needs to redraw itself entirely
 redraw();
}

KDRect View::bounds() {
  KDRect bounds = m_frame;
  bounds.x = 0;
  bounds.y = 0;
  return bounds;
}

KDRect View::absoluteDrawingArea() {
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
