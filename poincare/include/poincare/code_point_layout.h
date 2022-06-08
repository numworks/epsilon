#ifndef POINCARE_CODEPOINT_LAYOUT_NODE_H
#define POINCARE_CODEPOINT_LAYOUT_NODE_H

#include <ion/unicode/code_point.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/string_format.h>

namespace Poincare {

/* TODO: Make several code point classes depending on codepoint size?
 * (m_codePoint sometimes fits in a char, no need for a whole CodePoint */

class CodePointLayoutNode : public LayoutNode, public StringFormat {
public:
  CodePointLayoutNode(CodePoint c = UCodePointNull, const KDFont * font = StringFormat::k_defaultFont) :
    LayoutNode(),
    StringFormat(font),
    m_codePoint(c)
  {}

  // Layout
  Type type() const override { return Type::CodePointLayout; }

  // CodePointLayout
  CodePoint codePoint() const { return m_codePoint; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;
  bool canBeOmittedMultiplicationLeftFactor() const override;
  bool canBeOmittedMultiplicationRightFactor() const override;

  // TreeNode
  size_t size() const override { return sizeof(CodePointLayoutNode); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "CodePointLayout";
  }
  void logAttributes(std::ostream & stream) const override {
    constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
    char buffer[bufferSize];
    SerializationHelper::CodePoint(buffer, bufferSize, m_codePoint);
    stream << " CodePoint=\"" << buffer << "\"";
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
  bool protectedIsIdenticalTo(Layout l) override;
  CodePoint m_codePoint;

private:
  static constexpr const int k_middleDotWidth = 5;
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
  bool isMultiplicationCodePoint() const;
};

class CodePointLayout : public Layout {
public:
  CodePointLayout(const CodePointLayoutNode * n) : Layout(n) {}
  static CodePointLayout Builder(CodePoint c, const KDFont * font = StringFormat::k_defaultFont);
  const KDFont * font() const { return const_cast<CodePointLayout *>(this)->node()->font(); }
  CodePoint codePoint() const { return const_cast<CodePointLayout *>(this)->node()->codePoint(); }
  void setFont(const KDFont * font) { node()->setFont(font); }

private:
  using Layout::node;
  CodePointLayoutNode * node() { return static_cast<CodePointLayoutNode *>(Layout::node()); }
};

}

#endif
