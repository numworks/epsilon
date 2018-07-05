#ifndef POINCARE_HORIZONTAL_LAYOUT_NODE_H
#define POINCARE_HORIZONTAL_LAYOUT_NODE_H

#include "layout_reference.h"
#include "layout_node.h"
#include "layout_cursor.h"

namespace Poincare {

/* WARNING: A HorizontalLayout should never have a HorizontalLayout child. For
 * instance, use addOrMergeChildAtIndex to add a LayoutNode safely. */

class HorizontalLayoutNode : public LayoutNode {
public:
  HorizontalLayoutNode() :
    LayoutNode(),
    m_numberOfChildren(0)
  {}

  // Tree navigation
  LayoutCursor equivalentCursor(LayoutCursor * cursor) override;

  // Tree modification
  void addOrMergeChildAtIndex(LayoutNode * l, int index, bool removeEmptyChildren);
  void mergeChildrenAtIndex(HorizontalLayoutNode * horizontalLayout, int index, bool removeEmptyChildren);
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  void removeChildAndMoveCursor(LayoutNode * l, LayoutCursor * cursor) override;

  // LayoutNode
  bool isHorizontal() const override { return true; }
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;

  // TreeNode
  size_t size() const override { return sizeof(HorizontalLayoutNode); }
  int numberOfChildren() const override { return m_numberOfChildren; }
  void incrementNumberOfChildren(int increment = 1) override { m_numberOfChildren+= increment; }
  void decrementNumberOfChildren(int decrement = 1) override {
    assert(m_numberOfChildren >= decrement);
    m_numberOfChildren-= decrement;
  }
  void eraseNumberOfChildren() override { m_numberOfChildren = 0; }
#if TREE_LOG
  const char * description() const override {
    return "Horizontal Layout";
  }
#endif

protected:
  // LayoutNode
  void computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * l) override;

private:
  void privateAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override;
  void privateRemoveChildAtIndex(int index, bool forceRemove);
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override {}
  int removeEmptyChildBeforeInsertionAtIndex(int index, bool shouldRemoveOnLeft);
  int m_numberOfChildren;
};

class HorizontalLayoutRef : public LayoutReference<HorizontalLayoutNode> {
public:
  HorizontalLayoutRef() : LayoutReference<HorizontalLayoutNode>() {}
  HorizontalLayoutRef(LayoutRef l) : LayoutReference<HorizontalLayoutNode>() {
    addChild(l);
  }

  HorizontalLayoutRef(LayoutRef l1, LayoutRef l2) : LayoutReference<HorizontalLayoutNode>() {
    addChild(l2);
    addChild(l1);
  }
};

}

#endif
