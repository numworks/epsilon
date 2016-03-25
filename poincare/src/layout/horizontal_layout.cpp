extern "C" {
#include <assert.h>
#include <kandinsky.h>
#include <stdlib.h>
}
#include "horizontal_layout.h"
#include "string_layout.h"

HorizontalLayout::HorizontalLayout(ExpressionLayout ** children_layouts, int number_of_children) :
  ExpressionLayout(), m_number_of_children(number_of_children), m_children_layouts(children_layouts) {
  assert(number_of_children > 0);
  for (int i=0; i<m_number_of_children; i++) {
    m_children_layouts[i]->setParent(this);
  }
}

HorizontalLayout::~HorizontalLayout() {
  for (int i=0; i<m_number_of_children; i++) {
    delete m_children_layouts[i];
  }
  free(m_children_layouts);
}

void HorizontalLayout::render(KDPoint point) { }

KDSize HorizontalLayout::computeSize() {
  KDSize size = (KDSize){.width = 0, .height = 0};
  int i = 0;
  while (ExpressionLayout * c = child(i++)) {
    KDSize childSize = c->size();
    size.width += childSize.width;
    if (childSize.height > size.height) {
      size.height = childSize.height;
    }
  }
  return size;
}

ExpressionLayout * HorizontalLayout::child(uint16_t index) {
  assert(index <= m_number_of_children);
  if (index < m_number_of_children) {
    return m_children_layouts[index];
  } else {
    return nullptr;
  }
}

KDPoint HorizontalLayout::positionOfChild(ExpressionLayout * child) {
  KDPoint position = (KDPoint){.x = 0, .y = 0};
  uint16_t index = 0;
  for (int i=0;i<m_number_of_children;i++) {
    if (m_children_layouts[i] == child) {
      index = i;
      break;
    }
  }
  if (index > 0) {
    ExpressionLayout * previousChild = m_children_layouts[index-1];
    assert(previousChild != nullptr);
    position.x = previousChild->origin().x + previousChild->size().width;
  }
  position.y = (size().height - child->size().height)/2;
  return position;
}
