extern "C" {
#include <assert.h>
}
#include <escher/view.h>

View::View(KDRect frame) :
  m_frame(frame),
  m_superview(nullptr),
  m_numberOfSubviews(0)
{
}

View::~View() {
}

void View::drawRect(KDRect rect) {
}

void View::draw() {
  setOriginAndDrawRect(m_frame);
  for (uint8_t i=0; i<m_numberOfSubviews; i++) {
    View * subview = m_subviews[i];
    subview->draw();
  }
}

void View::addSubview(View * subview) {
  assert(m_numberOfSubviews < k_maxNumberOfSubviews);
  subview->m_superview = this;
  m_subviews[m_numberOfSubviews++] = subview;
}

void View::removeFromSuperview() {
  assert(m_superview != nullptr);
  assert(false); // FIXME: Unimplemented
}

void View::setFrame(KDRect frame) {
 // TODO: Return if frame is equal to m_frame
 if (m_superview != nullptr) {
   // We're moving this view
   // So let's ask its superview to redraw itself where that view previously was
   m_superview->setOriginAndDrawRect(m_frame);
 }
 m_frame = frame;
 if (m_superview != nullptr) {
   // We've been moved
   // Draw ourself at the new location
   setOriginAndDrawRect(bounds());
 }
}

KDRect View::bounds() {
  KDRect bounds = m_frame;
  bounds.x = 0;
  bounds.y = 0;
  return bounds;
}

void View::setOriginAndDrawRect(KDRect rect) {
  KDSetOrigin(absoluteOrigin());
  this->drawRect(rect);
}

KDPoint View::absoluteOrigin() {
  if (m_superview == nullptr) {
    return m_frame.origin;
  } else {
    KDPoint parentOrigin = m_superview->absoluteOrigin();
    KDPoint myOrigin = m_frame.origin;
    KDPoint result;
    result.x = parentOrigin.x + myOrigin.x;
    result.y = parentOrigin.y + myOrigin.y;
    return result;
  }
}
