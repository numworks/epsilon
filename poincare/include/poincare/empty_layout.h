#ifndef POINCARE_EMPTY_LAYOUT_NODE_H
#define POINCARE_EMPTY_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout.h>

namespace Poincare {

class EmptyLayoutNode final : public LayoutNode {
public:
  enum class Color : bool {
    Yellow,
    Gray
  };
  enum class Visibility : uint8_t {
    On,
    Off,
    Never
  };

  // Layout
  Type type() const override { return Type::EmptyLayout; }

  EmptyLayoutNode(Color color = Color::Yellow, Visibility visible = Visibility::On, bool margins = true) :
    LayoutNode(),
    m_visibility(visible),
    m_color(color),
    m_margins(margins)
  {}

  // EmptyLayout
  Color color() const { return m_color; }
  void setColor(Color color) { m_color = color; }
  bool isVisible() const { return m_visibility == Visibility::On; }
  void setVisible(bool visible);
  void enableToBeVisible() { m_visibility = Visibility::On; }

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
  void logAttributes(std::ostream & stream) const override {
    stream << " color=\"" << (m_color == Color::Yellow ? "yellow" : "gray") << "\"";
    stream << " visible=\"" << (m_visibility == Visibility::On ? "true" : (m_visibility == Visibility::Off ? "false" : "never")) << "\"";
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode * child, KDFont::Size font) override {
    assert(false);
    return KDPointZero;
  }
private:
  constexpr static KDCoordinate k_marginWidth = 1;
  constexpr static KDCoordinate k_marginHeight = 3;
  constexpr static KDCoordinate k_lineThickness = 1;
  KDCoordinate height(KDFont::Size font) const { return KDFont::GlyphHeight(font) - 2 * k_marginHeight; }
  KDCoordinate width(KDFont::Size font) const { return KDFont::GlyphWidth(font) - 2 * k_marginWidth; }

  // LayoutNode
  void moveCursorVertically(OMG::NewVerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) override;
  bool willAddSibling(LayoutCursor * cursor, Layout * sibling, bool moveCursor) override;
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
  bool protectedIsIdenticalTo(Layout l) override;

  Visibility m_visibility;
  Color m_color;
  bool m_margins;
};

class EmptyLayout final : public Layout {
public:
  EmptyLayout(const EmptyLayoutNode * n);
  static EmptyLayout Builder(EmptyLayoutNode::Color color = EmptyLayoutNode::Color::Yellow, EmptyLayoutNode::Visibility visible = EmptyLayoutNode::Visibility::On, bool margins = true);
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
