#ifndef POINCARE_ALLOCATION_FAILED_LAYOUT_NODE_H
#define POINCARE_ALLOCATION_FAILED_LAYOUT_NODE_H

#include "layout_node.h"
#include "layout_reference.h"
#include "layout_cursor.h"

namespace Poincare {

class AllocationFailedLayoutNode : public LayoutNode {
public:
  // LayoutNode
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    assert(false);
    return 0;
  }
  LayoutCursor equivalentCursor(LayoutCursor * cursor) override { return LayoutCursor(); }
  void removeChildAndMoveCursor(LayoutNode * l, LayoutCursor * cursor) override { }
  void deleteBeforeCursor(LayoutCursor * cursor) override { }

  // TreeNode
  size_t size() const override { return sizeof(AllocationFailedLayoutNode); }
  const char * description() const override { return "Allocation Failed";  }
  int numberOfChildren() const override { return 0; }
  bool isAllocationFailure() const override { return true; }

protected:
  // LayoutNode
  void computeSize() override { m_sized = true; }
  void computeBaseline() override { m_baselined = true; }
  KDPoint positionOfChild(LayoutNode * child) override {
    assert(false);
    return KDPointZero;
  }

private:
  void privateAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override {}
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override {}
};

class AllocationFailedLayoutRef : public LayoutReference<AllocationFailedLayoutNode> {
public:
  AllocationFailedLayoutRef() : LayoutReference<AllocationFailedLayoutNode>() {}
};

}

#endif
