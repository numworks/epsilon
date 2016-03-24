extern "C" {
#include <kandinsky.h>
#include <assert.h>
}
#include "function_layout.h"
#include "string_layout.h"

// This code seems like a duplicate of the horizontal layout but it isn't, indeed the horizontal
// layout is used to print the same operation applied to different expressions such that:
//   "expr_1 + epr_2 + expr_3 + expr_4".
// Here we want the pattern
//   FUNCTION_NAME(expr).
// Thus the code in horizontal layer is not really reusable.

FunctionLayout::FunctionLayout(ExpressionLayout * parent, char* function_name, Expression * argument) :
ExpressionLayout(parent) {
  m_children[0] = new StringLayout(this, function_name, 1);

  char string[2] = {'(', '\0'};
  m_children[1] = new StringLayout(this, string, 1);

  m_children[2] = argument->createLayout(this);

  string[0] = ')';
  m_children[3] = new StringLayout(this, string, 1);
}

FunctionLayout::~FunctionLayout() {
  for (int i(0); i<4; i++) {
    delete m_children[i];
  }
}

void FunctionLayout::render(KDPoint point) { }

KDSize FunctionLayout::computeSize() {
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

ExpressionLayout * FunctionLayout::child(uint16_t index) {
  if (index >= 4) {
    return nullptr;
  }
  return m_children[index];
}

KDPoint FunctionLayout::positionOfChild(ExpressionLayout * child) {
  KDPoint position = (KDPoint){.x = 0, .y = 0};
  uint16_t index = 0;
  for (int i=0;i<4;i++) {
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

