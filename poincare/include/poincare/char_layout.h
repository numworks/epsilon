#ifndef POINCARE_CHAR_LAYOUT_NODE_H
#define POINCARE_CHAR_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout.h>
#include <ion/charset.h>

namespace Poincare {

class CharLayoutNode : public LayoutNode {
public:
  CharLayoutNode(char c = Ion::Charset::Empty, KDText::FontSize fontSize = KDText::FontSize::Large) :
    LayoutNode(),
    m_char(c),
    m_fontSize(fontSize)
  {}

  // CharLayout
  virtual void setChar(char c) { m_char = c; }
  KDText::FontSize fontSize() const { return m_fontSize; }
  void setFontSize(KDText::FontSize fontSize) { m_fontSize = fontSize; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;

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
  KDText::FontSize m_fontSize;
};

class CharLayout : public Layout {
public:
  CharLayout(char c, KDText::FontSize fontSize = KDText::FontSize::Large);
  KDText::FontSize fontSize() const { return const_cast<CharLayout *>(this)->node()->fontSize(); }
private:
  using Layout::node;
  CharLayoutNode * node() { return static_cast<CharLayoutNode *>(Layout::node());}
};

}

#endif
