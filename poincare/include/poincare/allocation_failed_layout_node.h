#ifndef POINCARE_ALLOCATION_FAILED_LAYOUT_NODE_H
#define POINCARE_ALLOCATION_FAILED_LAYOUT_NODE_H

#include "layout_node.h"
#include "layout_reference.h"
#include "layout_cursor.h"

namespace Poincare {

class AllocationFailedLayoutNode : public LayoutNode {
public:
  using GhostLayoutNode::GhostLayoutNode;
  // TreeNode
  bool isAllocationFailure() const override { return true; }
  size_t size() const override { return sizeof(AllocationFailedLayoutNode); }
#if TREE_LOG
  const char * description() const override { return "AllocationFailedLayout";  }
#endif
  int numberOfChildren() const override { return 0; }
  // LayoutNode
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    assert(false);
    int descriptionLength = strlen(description()) + 1;
    return strlcpy(buffer, description(), bufferSize < descriptionLength ? bufferSize : descriptionLength);
  }
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override {}
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override {}
  LayoutCursor equivalentCursor(LayoutCursor * cursor) override { return LayoutCursor(); }
  void deleteBeforeCursor(LayoutCursor * cursor) override {}

protected:
  // LayoutNode
  KDSize computeSize() override { return KDSizeZero; }
  KDCoordinate computeBaseline() override { return 0; }
  KDPoint positionOfChild(LayoutNode * child) override {
    assert(false);
    return KDPointZero;
  }

private:
  bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override { return false; }
  bool willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor, bool force) override { return false; }
  bool willRemoveChild(LayoutNode * l, LayoutCursor * cursor, bool force) override { return false; }

  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override {}
};

class AllocationFailedLayoutRef : public LayoutReference {
public:
  AllocationFailedLayoutRef() : LayoutReference(TreePool::sharedPool()->createTreeNode<AllocationFailedLayoutNode>(), true) {}
};

}

#endif
