#ifndef POINCARE_NTH_ROOT_LAYOUT_NODE_H
#define POINCARE_NTH_ROOT_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout.h>

namespace Poincare {

class NthRootLayout;

class NthRootLayoutNode /*final*/ : public LayoutNode {
  friend class NthRootLayout;
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
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  bool shouldCollapseSiblingsOnRight() const override { return true; }
  bool hasUpperLeftIndex() const override { return m_hasIndex; }

  // TreeNode
  size_t size() const override { return sizeof(NthRootLayoutNode); }
  int numberOfChildren() const override { return m_hasIndex ? 2 : 1; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "NthRootLayout";
  }
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

class NthRootLayout final : public Layout {
public:
  static NthRootLayout Builder(Layout radicand) { return NthRootLayout(radicand); }
  static NthRootLayout Builder(Layout radicand, Layout index) {
    NthRootLayout n(radicand);
    n.addChildAtIndexInPlace(index, 1, 1);
    static_cast<NthRootLayoutNode *>(n.node())->setNumberOfChildren(2);
    return n;
  }
private:
  NthRootLayout(Layout radicand) : Layout(TreePool::sharedPool()->createTreeNode<NthRootLayoutNode>()) {
    replaceChildAtIndexInPlace(0, radicand);
  }
};

}

#endif
