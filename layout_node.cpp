#include "layout_node.h"
#include "allocation_failed_layout_node.h"
#include "layout_reference.h"

TreeNode * LayoutNode::staticFailedAllocationStaticNode() {
  return LayoutRef::staticFailedAllocationStaticNode();
}

void LayoutNode::draw() {
  for (LayoutNode * child : children()) {
    child->draw();
  }
  render();
}

int LayoutNode::origin() {
  LayoutNode * parentLayout = parent();
  if (parentLayout == nullptr) {
    return absoluteOrigin();
  } else {
    return 1; //KDPoint(absoluteOrigin().x() - parentLayout->absoluteOrigin().x(), absoluteOrigin().y() - parentLayout->absoluteOrigin().y());
  }
  return 0;
}

int LayoutNode::absoluteOrigin() {
  /*if (!m_positioned) {
    LayoutNode * parentLayout = parent();
    if (parentLayout != nullptr) {
      m_frame.setOrigin(parentLayout->absoluteOrigin().translatedBy(parentLayout->positionOfChild(this)));
    } else {
      m_frame.setOrigin(KDPointZero);
    }
    m_positioned = true;
  }
  return m_frame.origin();*/
  return 1;
}
