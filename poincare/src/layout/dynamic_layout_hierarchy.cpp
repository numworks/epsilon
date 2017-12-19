#include <poincare/dynamic_layout_hierarchy.h>
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

/*bool DynamicLayoutHierarchy::addChildAtIndex(ExpressionLayout * child, int index) {
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
  return true;
}*/ //TODO

}
