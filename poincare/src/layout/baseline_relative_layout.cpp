#include "baseline_relative_layout.h"
#include <string.h>
#include <assert.h>

namespace Poincare {

BaselineRelativeLayout::BaselineRelativeLayout(ExpressionLayout * baseLayout, ExpressionLayout * indiceLayout, Type type) :
  ExpressionLayout(),
  m_baseLayout(baseLayout),
  m_indiceLayout(indiceLayout),
  m_type(type)
{
  m_baseLayout->setParent(this);
  m_indiceLayout->setParent(this);
  m_baseline = type == Type::Subscript ? m_baseLayout->baseline() :
    m_indiceLayout->size().height() + m_baseLayout->baseline() - k_indiceHeight;
}

BaselineRelativeLayout::~BaselineRelativeLayout() {
  delete m_baseLayout;
  delete m_indiceLayout;
}

ExpressionLayout * BaselineRelativeLayout::baseLayout() {
  return m_baseLayout;
}

ExpressionLayout * BaselineRelativeLayout::indiceLayout() {
  return m_indiceLayout;
}

void BaselineRelativeLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // There is nothing to draw for a subscript/superscript, only the position of the children matters
}

KDSize BaselineRelativeLayout::computeSize() {
  KDSize baseSize = m_baseLayout->size();
  KDSize indiceSize = m_indiceLayout->size();
  return KDSize(baseSize.width() + indiceSize.width(), baseSize.height() + indiceSize.height() - k_indiceHeight);
}

ExpressionLayout * BaselineRelativeLayout::child(uint16_t index) {
  switch (index) {
    case 0:
      return m_baseLayout;
    case 1:
      return m_indiceLayout;
    default:
      return nullptr;
  }
}

KDPoint BaselineRelativeLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == m_baseLayout && m_type == Type::Superscript) {
    x = 0;
    y = m_indiceLayout->size().height() - k_indiceHeight;
  }
  if (child == m_indiceLayout) {
    x = m_baseLayout->size().width();
    y =  m_type == Type::Superscript ? 0 : m_baseLayout->size().height() - k_indiceHeight;
  }
  return KDPoint(x,y);
}

}

