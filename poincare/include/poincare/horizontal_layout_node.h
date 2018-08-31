#ifndef POINCARE_HORIZONTAL_LAYOUT_NODE_H
#define POINCARE_HORIZONTAL_LAYOUT_NODE_H

#include <poincare/layout_reference.h>
#include <poincare/layout_node.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

/* WARNING: A HorizontalLayout should never have a HorizontalLayout child. For
 * instance, use addOrMergeChildAtIndex to add a LayoutNode safely. */

class HorizontalLayoutNode : public LayoutNode {
  friend class LayoutReference;
public:

  // AllocationFailure
  static HorizontalLayoutNode * FailedAllocationStaticNode();
  HorizontalLayoutNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  HorizontalLayoutNode() :
    LayoutNode(),
    m_numberOfChildren(0)
  {}

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  LayoutCursor equivalentCursor(LayoutCursor * cursor) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  bool isHorizontal() const override { return true; }
  bool isEmpty() const override { return m_numberOfChildren == 1 && const_cast<HorizontalLayoutNode *>(this)->childAtIndex(0)->isEmpty(); }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override { return m_numberOfChildren != 0; }
  bool hasText() const override;

  // TreeNode
  size_t size() const override { return sizeof(HorizontalLayoutNode); }
  int numberOfChildren() const override { return m_numberOfChildren; }
  void incrementNumberOfChildren(int increment = 1) override { m_numberOfChildren+= increment; }
  void decrementNumberOfChildren(int decrement = 1) override {
    assert(m_numberOfChildren >= decrement);
    m_numberOfChildren-= decrement;
  }
  void eraseNumberOfChildren() override { m_numberOfChildren = 0; }
  void childAtIndexWillBeStolen(int index) override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "HorizontalLayout";
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * l) override;

private:
  bool willAddChildAtIndex(LayoutNode * l, int * index, int * currentNumberOfChildren, LayoutCursor * cursor) override;
  bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override;
  bool willRemoveChild(LayoutNode * l, LayoutCursor * cursor, bool force) override;
  void didRemoveChildAtIndex(int index, LayoutCursor * cursor, bool force) override;
  bool willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor, bool force) override;
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override {}
  int m_numberOfChildren;
};

class HorizontalLayoutRef : public LayoutReference {
  friend class HorizontalLayoutNode;
public:
  HorizontalLayoutRef(HorizontalLayoutNode * n) : LayoutReference(n) {}
  HorizontalLayoutRef() : LayoutReference(TreePool::sharedPool()->createTreeNode<HorizontalLayoutNode>()) {}

  HorizontalLayoutRef(LayoutRef l) : HorizontalLayoutRef() {
    addChildAtIndexInPlace(l, 0, 0);
  }

  HorizontalLayoutRef(LayoutRef l1, LayoutRef l2) : HorizontalLayoutRef() {
    addChildAtIndexInPlace(l1, 0, 0);
    addChildAtIndexInPlace(l2, 1, 1);
  }
  HorizontalLayoutRef(LayoutRef l1, LayoutRef l2, LayoutRef l3) : HorizontalLayoutRef() {
    addChildAtIndexInPlace(l1, 0, 0);
    addChildAtIndexInPlace(l2, 1, 1);
    addChildAtIndexInPlace(l3, 2, 2);
  }
  HorizontalLayoutRef(LayoutRef l1, LayoutRef l2, LayoutRef l3, LayoutRef l4) : HorizontalLayoutRef() {
    addChildAtIndexInPlace(l1, 0, 0);
    addChildAtIndexInPlace(l2, 1, 1);
    addChildAtIndexInPlace(l3, 2, 2);
    addChildAtIndexInPlace(l4, 3, 3);
  }
  void addChildAtIndex(LayoutReference l, int index, int currentNumberOfChildren, LayoutCursor * cursor) override {
    LayoutReference::addChildAtIndex(l, index, currentNumberOfChildren, cursor);
  }
  // Remove puts a child at the end of the pool
  void removeChild(LayoutReference l, LayoutCursor * cursor, bool force = false) override {
    LayoutReference::removeChild(l, cursor, force);
  }
  void removeChildAtIndex(int index, LayoutCursor * cursor, bool force = false) override {
    LayoutReference::removeChildAtIndex(index, cursor, force);
  }
  void addOrMergeChildAtIndex(LayoutRef l, int index, bool removeEmptyChildren, LayoutCursor * cursor = nullptr);
  void mergeChildrenAtIndex(HorizontalLayoutRef h, int index, bool removeEmptyChildren, LayoutCursor * cursor = nullptr);
private:
  void removeEmptyChildBeforeInsertionAtIndex(int * index, int * currentNumberOfChildren, bool shouldRemoveOnLeft, LayoutCursor * cursor = nullptr);
};

}

#endif
