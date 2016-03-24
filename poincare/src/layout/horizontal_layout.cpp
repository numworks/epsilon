extern "C" {
#include <kandinsky.h>
#include <assert.h>
}
#include "horizontal_layout.h"
#include "string_layout.h"

HorizontalLayout::HorizontalLayout(ExpressionLayout * parent, int number_of_operands, Expression ** operands, char symbol) {
  m_number_of_operands = number_of_operands;
  // FIXME: This implementation is not optimal as the operator layout is created and stored a lot of times.
  // The reason for this is how the layouts are drawn.
  m_children_layouts = (ExpressionLayout **)malloc((2*m_number_of_operands-)*sizeof(ExpressionLayout *));
  m_operator_layout = new StringLayout(this, string, 1);
  char string[2] = {symbol, '\0'};
  for (int i=1; i<m_number_of_operands; i++) {
    m_operands[2*i-1] = m_operator_layout;
    m_operands[2*i] = operands[i]->createLayout();
  }
}

HorizontalLayout::~HorizontalLayout() {
  for (int i(0); i<m_numberOfOperands; i++) {
    delete m_operands[2*i];
  }
  free(m_operands);
  delete m_operator_layout;
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
  if (index >= 2*m_number_of_operands) {
    return nullptr;
  }
  return m_children_layouts[index];
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
