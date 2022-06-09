#ifndef POINCARE_EMPTY_LAYOUT_NODE_H
#define POINCARE_EMPTY_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout.h>

namespace Poincare {

class EmptyLayoutNode /*final*/ : public LayoutNode {
public:
  enum class Color {
    Yellow,
    Gray
  };

  // Layout
  Type type() const override { return Type::EmptyLayout; }

  EmptyLayoutNode(Color color = Color::Yellow, bool visible = true, const KDFont * font = KDFont::LargeFont, bool margins = true) :
    LayoutNode(),
    m_isVisible(visible),
    m_color(color),
    m_font(font),
    m_margins(margins)
  {}

  // EmptyLayout
  Color color() const { return m_color; }
  void setColor(Color color) { m_color = color; }
  bool isVisible() const { return m_isVisible; }
  void setVisible(bool visible) { m_isVisible = visible; }

  // LayoutNode
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  bool isEmpty() const override { return true; }
  bool hasText() const override { return false; }

  // TreeNode
  size_t size() const override { return sizeof(EmptyLayoutNode); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "EmptyLayout";
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override {
    assert(false);
    return KDPointZero;
  }
private:
  constexpr static KDCoordinate k_marginWidth = 1;
  constexpr static KDCoordinate k_marginHeight = 3;
  constexpr static KDCoordinate k_lineThickness = 1;
  KDCoordinate height() const { return m_font->glyphSize().height() - 2 * k_marginHeight; }
  KDCoordinate width() const { return m_font->glyphSize().width() - 2 * k_marginWidth; }

  // LayoutNode
  void moveCursorVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) override;
  bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override;
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
  bool protectedIsIdenticalTo(Layout l) override;

  bool m_isVisible;
  Color m_color;
  const KDFont * m_font;
  bool m_margins;
};

class EmptyLayout final : public Layout {
public:
  EmptyLayout(const EmptyLayoutNode * n);
  static EmptyLayout Builder(EmptyLayoutNode::Color color = EmptyLayoutNode::Color::Yellow, bool visible = true, const KDFont * font = KDFont::LargeFont, bool margins = true);
  void setVisible(bool visible) {
    node()->setVisible(visible);
  }

  EmptyLayoutNode::Color color() const { return node()->color(); }
  void setColor(EmptyLayoutNode::Color color) {
    node()->setColor(color);
  }
private:
  EmptyLayoutNode * node() const { return static_cast<EmptyLayoutNode *>(Layout::node()); }
};

}

#endif
