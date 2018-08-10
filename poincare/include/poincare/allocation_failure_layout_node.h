#ifndef POINCARE_ALLOCATION_FAILURE_LAYOUT_NODE_H
#define POINCARE_ALLOCATION_FAILURE_LAYOUT_NODE_H

#include <poincare/allocation_failure_node.h>
#include <poincare/layout_node.h>
#include <poincare/layout_reference.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

template <typename T>
class AllocationFailureLayoutNode : public AllocationFailureNode<T> {
public:
  // LayoutNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    assert(false);
    return 0;
    /*
    int descriptionLength = strlen(description()) + 1;
    return strlcpy(buffer, description(), bufferSize < descriptionLength ? bufferSize : descriptionLength);
    */
  }
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override {}
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override {}
  LayoutCursor equivalentCursor(LayoutCursor * cursor) override { return LayoutCursor(); }
  void deleteBeforeCursor(LayoutCursor * cursor) override {}

  // TreeNode
  size_t size() const override { return sizeof(AllocationFailureLayoutNode<T>); }
#if TREE_LOG
  const char * description() const override { return "AllocationFailureLayout";  }
#endif

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

/*
template <typename T>
class AllocationFailureLayoutRef : public LayoutReference {
public:
  AllocationFailedLayoutRef() : LayoutReference(TreePool::sharedPool()->createTreeNode<AllocationFailureLayoutNode<T>>()) {}
};
*/

}

#endif
