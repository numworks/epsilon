#include <poincare/dynamic_layout_hierarchy.h>
#include "empty_visible_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

DynamicLayoutHierarchy::DynamicLayoutHierarchy() :
  ExpressionLayout(),
  m_children(nullptr),
  m_numberOfChildren(0)
{
}

DynamicLayoutHierarchy::DynamicLayoutHierarchy(const ExpressionLayout * const * children, int numberOfChildren, bool cloneChildren) :
  ExpressionLayout(),
  m_numberOfChildren(numberOfChildren)
{
  assert(children != nullptr);
  m_children = new const ExpressionLayout * [numberOfChildren];
  for (int i=0; i<numberOfChildren; i++) {
    assert(children[i] != nullptr);
    if (cloneChildren) {
      m_children[i] = children[i]->clone();
    } else {
      m_children[i] = children[i];
    }
    const_cast<ExpressionLayout *>(m_children[i])->setParent(this);
  }
}

DynamicLayoutHierarchy::~DynamicLayoutHierarchy() {
  if (m_children != nullptr) {
    for (int i = 0; i < m_numberOfChildren; i++) {
      if (m_children[i] != nullptr) {
        delete m_children[i];
      }
    }
  }
  delete[] m_children;
}

void DynamicLayoutHierarchy::mergeChildrenAtIndex(DynamicLayoutHierarchy * eL, int index, bool removeEmptyChildren) {
  int indexOfEL = indexOfChild(eL);
  if (indexOfEL >= 0) {
    removeChildAtIndex(indexOfEL, false);
  }
  addChildrenAtIndex(eL->children(), eL->numberOfChildren(), index, removeEmptyChildren);
  eL->detachChildren();
  delete eL;
}

void DynamicLayoutHierarchy::addChildrenAtIndex(const ExpressionLayout * const * operands, int numberOfOperands, int indexForInsertion, bool removeEmptyChildren) {
  assert(numberOfOperands > 0);
  const ExpressionLayout ** newOperands = new const ExpressionLayout * [m_numberOfChildren+numberOfOperands];
  int currentIndex = 0;
  assert(indexForInsertion <= m_numberOfChildren);
  for (int i=0; i<indexForInsertion; i++) {
    newOperands[currentIndex++] = m_children[i];
  }
  for (int i=0; i<numberOfOperands; i++) {
    if (!removeEmptyChildren
        || !operands[i]->isEmpty()
        || (i < numberOfOperands-1 && operands[i+1]->mustHaveLeftBrother()))
    {
      const_cast<ExpressionLayout *>(operands[i])->setParent(this);
      newOperands[currentIndex++] = operands[i];
    }
  }
  for (int i=indexForInsertion; i<m_numberOfChildren; i++) {
    newOperands[currentIndex++] = m_children[i];
  }
  delete[] m_children;
  m_children = newOperands;
  m_numberOfChildren = currentIndex;
}

bool DynamicLayoutHierarchy::addChildAtIndex(ExpressionLayout * child, int index) {
  assert(index >= 0 && index <= m_numberOfChildren);
  const ExpressionLayout ** newChildren = new const ExpressionLayout * [m_numberOfChildren+1];
  int j = 0;
  for (int i=0; i<=m_numberOfChildren; i++) {
    if (i == index) {
      child->setParent(this);
      newChildren[i] = child;
    } else {
      newChildren[i] = m_children[j++];
    }
  }
  delete[] m_children;
  m_children = newChildren;
  m_numberOfChildren += 1;
  m_sized = false;
  m_positioned = false;
  m_baselined = false;
  return true;
}

void DynamicLayoutHierarchy::removeChildAtIndex(int index, bool deleteAfterRemoval) {
  if (deleteAfterRemoval) {
    delete m_children[index];
  } else {
    const_cast<ExpressionLayout *>(m_children[index])->setParent(nullptr);
  }
  m_numberOfChildren--;
  for (int j=index; j<m_numberOfChildren; j++) {
    m_children[j] = m_children[j+1];
  }
}

bool DynamicLayoutHierarchy::isEmpty() const {
  if (m_numberOfChildren == 0
      || (m_numberOfChildren == 1&& child(0)->isEmpty()))
  {
    return true;
  }
  return false;
}

}
