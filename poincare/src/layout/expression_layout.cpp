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

void ExpressionLayout::draw(KDPoint point) {
  int i = 0;
  while (ExpressionLayout * c = child(i++)) {
    c->draw(point);
  }
  render(KDPointTranslate(origin(), point));
}

KDPoint ExpressionLayout::origin() {
  if (!m_positioned) {
    if (m_parent != nullptr) {
      m_frame.origin = KDPointTranslate(m_parent->origin(), m_parent->positionOfChild(this));
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

#if 0
void ExpressionLayout::computeLayout(ExpressionLayout * parent, uint16_t childIndex) {
  assert(parent == nullptr || parent->child(childIndex) == this);

  /* Positionning should be done before the recursion. This way, when a child is
   * being positionned, its parent is already positionned. */
  if (!m_positioned) {
    if (parent == nullptr) {
      m_frame.origin = KDPOINT(0,0);
    } else {
      m_frame.origin = KDPointTranslate(parent->origin(), parent->positionOfChild(childIndex));
    }
    m_positioned = true;
  }

  int i=0;
  while (ExpressionLayout * c = child(i)) {
    c->computeLayout(this, i);
    i++;
  }

  /* Sizing should be done after the recursion. This way, when a parent is being
   * sized, all its children are already sized up. */
  if (!m_sized) {
    m_frame.size = computeSize();
    m_sized = true;
  }
}
#endif
