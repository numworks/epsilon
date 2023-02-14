#ifndef POINCARE_NTH_ROOT_LAYOUT_NODE_H
#define POINCARE_NTH_ROOT_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout.h>

namespace Poincare {

class NthRootLayout;

class NthRootLayoutNode final : public LayoutNode {
  friend class NthRootLayout;
public:
  constexpr static KDCoordinate k_leftRadixHeight = 9;
  constexpr static KDCoordinate k_leftRadixWidth = 5;

  NthRootLayoutNode(bool hasIndex) :
    LayoutNode(),
    m_hasIndex(hasIndex)
  {}

  bool isSquareRoot() const;

  // Layout
  Type type() const override { return Type::NthRootLayout; }

  // LayoutNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int indexOfNextChildToPointToAfterHorizontalCursorMove(OMG::HorizontalDirection direction, int currentIndex, bool * shouldRedrawLayout) override;
  int indexOfNextChildToPointToAfterVerticalCursorMove(OMG::VerticalDirection direction, int currentIndex, PositionInLayout positionAtCurrentIndex, bool * shouldRedrawLayout) override;
  DeletionMethod deletionMethodForCursorLeftOfChild(int childIndex) const override;
  bool hasUpperLeftIndex() const override { return m_hasIndex; }

  // TreeNode
  size_t size() const override { return sizeof(NthRootLayoutNode); }
  int numberOfChildren() const override { return m_hasIndex ? 2 : 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "NthRootLayout";
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode * child, KDFont::Size font) override;
private:
  constexpr static KDCoordinate k_heightMargin = 2;
  constexpr static KDCoordinate k_widthMargin = 2;
  constexpr static KDCoordinate k_radixLineThickness = 1;
  KDSize adjustedIndexSize(KDFont::Size font);
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) override;
  bool protectedIsIdenticalTo(Layout l) override;

  constexpr static int k_radicandLayoutIndex = 0;
  constexpr static int k_indexLayoutIndex = 1;
  LayoutNode * radicandLayout() { return childAtIndex(k_radicandLayoutIndex); }
  LayoutNode * indexLayout() { return m_hasIndex ? childAtIndex(k_indexLayoutIndex) : nullptr; }
  bool m_hasIndex;
};

class NthRootLayout final : public Layout {
public:
  NthRootLayout() = delete;
  static NthRootLayout Builder(Layout child);
  static NthRootLayout Builder(Layout radicand, Layout index);
};

}

#endif
