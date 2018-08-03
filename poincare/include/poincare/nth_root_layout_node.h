#ifndef POINCARE_NTH_ROOT_LAYOUT_NODE_H
#define POINCARE_NTH_ROOT_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout_node.h>
#include <poincare/layout_reference.h>

namespace Poincare {

class NthRootLayoutRef;

class NthRootLayoutNode : public LayoutNode {
  friend class NthRootLayoutRef;
public:
  constexpr static KDCoordinate k_leftRadixHeight = 8;
  constexpr static KDCoordinate k_leftRadixWidth = 5;

  NthRootLayoutNode() :
    LayoutNode(),
    m_hasIndex(false)
  {}

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  bool shouldCollapseSiblingsOnRight() const override { return true; }
  bool hasUpperLeftIndex() const override { return m_hasIndex; }

  // TreeNode
  size_t size() const override { return sizeof(NthRootLayoutNode); }
  int numberOfChildren() const override { return m_hasIndex ? 2 : 1; }
#if TREE_LOG
  const char * description() const override { return "NthRootLayout"; }
#endif

protected:
  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;
private:
  constexpr static KDCoordinate k_rightRadixHeight = 2;
  constexpr static KDCoordinate k_radixHorizontalOverflow = 2;
  constexpr static KDCoordinate k_indexHeight = 4;
  constexpr static KDCoordinate k_heightMargin = 2;
  constexpr static KDCoordinate k_widthMargin = 2;
  constexpr static KDCoordinate k_radixLineThickness = 1;
  void setNumberOfChildren(int number) {
    assert(number == 1 || number == 2);
    if (number == 1) {
      m_hasIndex = false;
    } else {
      m_hasIndex = true;
    }
  }
  KDSize adjustedIndexSize();
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  LayoutNode * radicandLayout() { return childAtIndex(0); }
  LayoutNode * indexLayout() { return m_hasIndex ? childAtIndex(1) : nullptr; }
  bool m_hasIndex;
};

class NthRootLayoutRef : public LayoutReference {
public:
  NthRootLayoutRef(TreeNode * n) : LayoutReference(n) {}

  NthRootLayoutRef(LayoutRef radicand) : NthRootLayoutRef() {
    addChildAtIndex(radicand, 0, 0);
    if (!node()->isAllocationFailure()) {
      static_cast<NthRootLayoutNode *>(node())->setNumberOfChildren(1);
    }
  }

  NthRootLayoutRef(LayoutRef radicand, LayoutRef index) : NthRootLayoutRef() {
    addChildAtIndex(radicand, 0, 0);
    if (node()->isAllocationFailure()) {
      return;
    }
    static_cast<NthRootLayoutNode *>(node())->setNumberOfChildren(1);
    addChildAtIndex(index, 1, 1);
    if (!node()->isAllocationFailure()) {
      static_cast<NthRootLayoutNode *>(node())->setNumberOfChildren(2);
    }
  }

private:
  NthRootLayoutRef() : LayoutReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<NthRootLayoutNode>();
    m_identifier = node->identifier();
  }
};

}

#endif
