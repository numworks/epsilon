#ifndef POINCARE_CHAR_LAYOUT_NODE_H
#define POINCARE_CHAR_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout_node.h>
#include <poincare/layout_reference.h>

namespace Poincare {

class CharLayoutNode : public LayoutNode {
public:
  CharLayoutNode() :
    LayoutNode(),
    m_char('a'),
    m_fontSize(KDText::FontSize::Large)
  {}
  void setChar(char c) { m_char = c; }
  void setFontSize(KDText::FontSize fontSize) { m_fontSize = fontSize; }

  // LayoutNode
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;

  // TreeNode
  size_t size() const override { return sizeof(CharLayoutNode); }
  int numberOfChildren() const override { return 0; }
#if TREE_LOG
  const char * description() const override {
    static char Description[] = "Char a";
    Description[5] = m_char;
    return Description;
  }
#endif

protected:
  // LayoutNode
  void computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override {
    assert(false);
    return KDPointZero;
  }

private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  char m_char;
  KDText::FontSize m_fontSize;
};

class CharLayoutRef : public LayoutReference<CharLayoutNode> {
public:
  CharLayoutRef(char c, KDText::FontSize fontSize = KDText::FontSize::Large) :
    LayoutReference<CharLayoutNode>()
  {
    if (!(this->node()->isAllocationFailure())) {
      this->typedNode()->setChar(c);
      this->typedNode()->setFontSize(fontSize);
    }
  }
};

}

#endif
