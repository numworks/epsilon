extern "C" {
#include <assert.h>
#include <kandinsky.h>
#include <stdlib.h>
}
#include "horizontal_layout.h"
#include "string_layout.h"

namespace Poincare {

HorizontalLayout::HorizontalLayout(ExpressionLayout ** children_layouts, int number_of_children) :
  ExpressionLayout(), m_number_of_children(number_of_children) {
  assert(number_of_children > 0);
  m_children_layouts = new ExpressionLayout *[number_of_children];
  for (int i=0; i<m_number_of_children; i++) {
    m_children_layouts[i] = children_layouts[i];
    m_children_layouts[i]->setParent(this);
    if (m_children_layouts[i]->baseline() > m_baseline) {
      m_baseline = m_children_layouts[i]->baseline();
    }
  }
}

HorizontalLayout::~HorizontalLayout() {
  for (int i=0; i<m_number_of_children; i++) {
    delete m_children_layouts[i];
  }
  delete[] m_children_layouts;
}

void HorizontalLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
}

KDSize HorizontalLayout::computeSize() {
  KDCoordinate totalWidth = 0;
  int i = 0;
  KDCoordinate max_under_baseline = 0;
  KDCoordinate max_above_baseline = 0;
  while (ExpressionLayout * c = child(i++)) {
    KDSize childSize = c->size();
    totalWidth += childSize.width();
    if (childSize.height() - c->baseline() > max_under_baseline) {
      max_under_baseline = childSize.height() - c->baseline() ;
    }
    if (c->baseline() > max_above_baseline) {
      max_above_baseline = c->baseline();
    }
  }
  return KDSize(totalWidth, max_under_baseline + max_above_baseline);
}

ExpressionLayout * HorizontalLayout::child(uint16_t index) {
  assert(index <= (unsigned int) m_number_of_children);
  if (index < (unsigned int) m_number_of_children) {
    return m_children_layouts[index];
  } else {
    return nullptr;
  }
}

KDPoint HorizontalLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
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
    x = previousChild->origin().x() + previousChild->size().width();
  }
  y = m_baseline - child->baseline();
  return KDPoint(x, y);
}

}
