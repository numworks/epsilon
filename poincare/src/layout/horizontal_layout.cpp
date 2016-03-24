extern "C" {
#include <kandinsky.h>
#include <assert.h>
}
#include "horizontal_layout.h"
#include "string_layout.h"

HorizontalLayout::HorizontalLayout(ExpressionLayout * parent, Expression * left_expression, char symbol, Expression * right_expression) :
ExpressionLayout(parent) {
  m_children[0] = left_expression->createLayout(this);

  char string[2] = {symbol, '\0'};
  m_children[1] = new StringLayout(this, string, 1);

  m_children[2] = right_expression->createLayout(this);
}

HorizontalLayout::~HorizontalLayout() {
  delete m_children[2];
  delete m_children[1];
  delete m_children[0];
}

void HorizontalLayout::render(KDPoint point) {
  // Nothing to render "per se"
}

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
  if (index >= 3) {
    return nullptr;
  }
  return m_children[index];
}

KDPoint HorizontalLayout::positionOfChild(ExpressionLayout * child) {
  KDPoint position = (KDPoint){.x = 0, .y = 0};
  uint16_t index = 0;
  for (int i=0;i<3;i++) {
    if (m_children[i] == child) {
      index = i;
      break;
    }
  }
  if (index > 0) {
    ExpressionLayout * previousChild = m_children[index-1];
    assert(previousChild != nullptr);
    position.x = previousChild->origin().x + previousChild->size().width;
  }
  position.y = (size().height - child->size().height)/2;
  return position;
}
