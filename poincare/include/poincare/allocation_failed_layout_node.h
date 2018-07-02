#ifndef POINCARE_ALLOCATION_FAILED_LAYOUT_NODE_H
#define POINCARE_ALLOCATION_FAILED_LAYOUT_NODE_H

#include "layout_node.h"
#include "layout_reference.h"

namespace Poincare {

class AllocationFailedLayoutNode : public LayoutNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(AllocationFailedLayoutNode); }
  const char * description() const override { return "Allocation Failed";  }
  int numberOfChildren() const override { return 0; }
  bool isAllocationFailure() const override { return true; }
};

class AllocationFailedLayoutRef : public LayoutReference<AllocationFailedLayoutNode> {
public:
  AllocationFailedLayoutRef() : LayoutReference<AllocationFailedLayoutNode>() {}
};

}

#endif
