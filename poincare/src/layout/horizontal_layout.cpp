extern "C" {
#include <assert.h>
#include <kandinsky.h>
#include <stdlib.h>
}
#include "horizontal_layout.h"
#include "string_layout.h"

HorizontalLayout::HorizontalLayout(ExpressionLayout * parent, Expression ** operands,int number_of_operands, char symbol) : ExpressionLayout(parent) {
  assert(number_of_operands > 0);
  m_number_of_children = 2*number_of_operands-1;
  m_children_layouts = (ExpressionLayout **)malloc(m_number_of_children*sizeof(ExpressionLayout *));
  char string[2] = {symbol, '\0'};
  m_children_layouts[0] = operands[0]->createLayout(this);
  for (int i=1; i<number_of_operands; i++) {
    m_children_layouts[2*i-1] = new StringLayout(this, string, 1);
    m_children_layouts[2*i] = operands[i]->createLayout(this);
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
