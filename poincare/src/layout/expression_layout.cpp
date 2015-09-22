#include <assert.h>
#include <stdlib.h>
#include "string_layout.h"

ExpressionLayout::ExpressionLayout(ExpressionLayout * parent) :
  m_parent(parent),
  m_sized(false),
  m_positioned(false) {
    m_frame = (KDRect){
      .x = 0,
      .y = 0,
      .width = 0,
      .height = 0
    };
}

KDPoint ExpressionLayout::origin() {
  if (m_parent == nullptr) {
    return absoluteOrigin();
  } else {
    return KDPOINT(absoluteOrigin().x-m_parent->absoluteOrigin().x, absoluteOrigin().y-m_parent->absoluteOrigin().y);
  }
}

void ExpressionLayout::draw(KDPoint point) {
  int i = 0;
  while (ExpressionLayout * c = child(i++)) {
    c->draw(point);
  }
  render(KDPointTranslate(absoluteOrigin(), point));
#if 0
  KDPoint topLeft = KDPointTranslate(absoluteOrigin(), point);
  KDPoint bottomRight = KDPointTranslate(KDPointTranslate(absoluteOrigin(), KDPOINT(size().width, 0 /*size().height*/)), point);
  KDDrawLine(topLeft, bottomRight);
#endif
}

KDPoint ExpressionLayout::absoluteOrigin() {
  if (!m_positioned) {
    if (m_parent != nullptr) {
      m_frame.origin = KDPointTranslate(m_parent->absoluteOrigin(), m_parent->positionOfChild(this));
    } else {
      m_frame.origin = KDPOINT(0,0);
    }
    m_positioned = true;
  }
  return m_frame.origin;
}

KDSize ExpressionLayout::size() {
  if (!m_sized) {
    m_frame.size = computeSize();
    m_sized = true;
  }
  return m_frame.size;
}
