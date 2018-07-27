#ifndef POINCARE_NTH_ROOT_LAYOUT_NODE_H
#define POINCARE_NTH_ROOT_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout_node.h>
#include <poincare/layout_reference.h>

namespace Poincare {

class NthRootLayoutNode : public LayoutNode {
public:
  constexpr static KDCoordinate k_leftRadixHeight = 8;
  constexpr static KDCoordinate k_leftRadixWidth = 5;

  NthRootLayoutNode() :
    LayoutNode(),
    m_numberOfChildren(0)
  {}

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override;
  bool shouldCollapseSiblingsOnRight() const override { return true; }
  bool hasUpperLeftIndex() const override { return numberOfChildren() > 1; }

  // TreeNode
  void incrementNumberOfChildren(int increment = 1) override {
    assert(increment == 1);
    m_numberOfChildren+= increment;
  }
  void decrementNumberOfChildren(int decrement = 1) override {
    assert(m_numberOfChildren >= decrement);
    m_numberOfChildren-= decrement;
  }
  size_t size() const override { return sizeof(NthRootLayoutNode); }
  int numberOfChildren() const override { return m_numberOfChildren; }
#if TREE_LOG
  const char * description() const override {
    return "NthRootLayout";
  }
#endif

protected:
  // LayoutNode
  void computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;
private:
  constexpr static KDCoordinate k_rightRadixHeight = 2;
  constexpr static KDCoordinate k_radixHorizontalOverflow = 2;
  constexpr static KDCoordinate k_indexHeight = 4;
  constexpr static KDCoordinate k_heightMargin = 2;
  constexpr static KDCoordinate k_widthMargin = 2;
  constexpr static KDCoordinate k_radixLineThickness = 1;
  KDSize adjustedIndexSize();
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  LayoutNode * radicandLayout() {
    assert(numberOfChildren() > 0);
    return childAtIndex(0);
  }
  LayoutNode * indexLayout() {
    assert(numberOfChildren() == 1 || numberOfChildren() == 2);
    return numberOfChildren() == 2 ? childAtIndex(1) : nullptr;
  }
  int m_numberOfChildren;
};

class NthRootLayoutRef : public LayoutReference {
public:
  NthRootLayoutRef(TreeNode * t) : LayoutReference(t) {}

  NthRootLayoutRef(LayoutRef radicand) : NthRootLayoutRef() {
    addChildTreeAtIndex(radicand, 0, 0);
  }

  NthRootLayoutRef(LayoutRef radicand, LayoutRef index) : NthRootLayoutRef() {
    addChildTreeAtIndex(radicand, 0, 0);
    addChildTreeAtIndex(index, 1, 1);
  }

private:
  NthRootLayoutRef() : LayoutReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<NthRootLayoutNode>();
    m_identifier = node->identifier();
  }
};

}

#endif
