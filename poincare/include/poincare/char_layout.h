#ifndef POINCARE_CHAR_LAYOUT_NODE_H
#define POINCARE_CHAR_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout.h>
#include <ion/charset.h>

namespace Poincare {

class CharLayoutNode final : public LayoutNode {
public:
  static constexpr const KDFont * k_defaultFont = KDFont::LargeFont;
  CharLayoutNode(char c = Ion::Charset::Empty, const KDFont * font = k_defaultFont) :
    LayoutNode(),
    m_char(c),
    m_font(font)
  {}

  // CharLayout
  virtual void setChar(char c) { m_char = c; }
  char character() const { return m_char; }
  const KDFont * font() const { return m_font; }
  void setFont(const KDFont * font) { m_font = font; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  bool isChar() const override { return true; }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;
  bool canBeOmittedMultiplicationRightFactor() const override;

  // TreeNode
  size_t size() const override { return sizeof(CharLayoutNode); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "CharLayout";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " char=\"" << m_char << "\"";
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
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  char m_char;
  const KDFont * m_font;
};

class CharLayout final : public Layout {
public:
  CharLayout(const CharLayoutNode * n) : Layout(n) {}
  CharLayout(char c, const KDFont * font = KDFont::LargeFont);
  const KDFont * font() const { return const_cast<CharLayout *>(this)->node()->font(); }
  char character() const {return const_cast<CharLayout *>(this)->node()->character();}
private:
  using Layout::node;
  CharLayoutNode * node() { return static_cast<CharLayoutNode *>(Layout::node());}
};

}

#endif
