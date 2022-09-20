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
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  bool shouldCollapseSiblingsOnRight() const override { return true; }
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
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
  bool protectedIsIdenticalTo(Layout l) override;
  LayoutNode * radicandLayout() { return childAtIndex(0); }
  LayoutNode * indexLayout() { return m_hasIndex ? childAtIndex(1) : nullptr; }
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
