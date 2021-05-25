#ifndef POINCARE_CODEPOINT_LAYOUT_NODE_H
#define POINCARE_CODEPOINT_LAYOUT_NODE_H

#include <ion/unicode/code_point.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

/* TODO: Make several code point classes depending on codepoint size?
 * (m_codePoint sometimes fits in a char, no need for a whole CodePoint */

class CodePointLayoutNode final : public LayoutNode {
public:
  static constexpr const KDFont * k_defaultFont = KDFont::LargeFont;
  CodePointLayoutNode(CodePoint c = UCodePointNull, const KDFont * font = k_defaultFont) :
    LayoutNode(),
    m_codePoint(c),
    m_displayType(DisplayType::None),
    m_font(font)
  {}

  // Layout
  Type type() const override { return Type::CodePointLayout; }

  // CodePointLayout
  CodePoint codePoint() const { return m_codePoint; }
  const KDFont * font() const { return m_font; }

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
  virtual void logAttributes(std::ostream & stream) const override {
    constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
    char buffer[bufferSize];
    SerializationHelper::CodePoint(buffer, bufferSize, m_codePoint);
    stream << " CodePoint=\"" << buffer << "\"";
  }
#endif

  enum class DisplayType : uint8_t {
    None,
    /* Add a thin margin to the right of the code point. Used to separate
     * groups of three digits. */
    Thousand,
    /* Add a thick margin before the code point, to separate two factors. */
    Implicit,
    /* Add a thick margin on each side of the code point. */
    Operator
  };

  DisplayType displayType() const { return m_displayType; }
  void setDisplayType(DisplayType type) { m_displayType = type; }

protected:
  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override {
    assert(false);
    return KDPointZero;
  }

private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
  bool isMultiplicationCodePoint() const;
  bool protectedIsIdenticalTo(Layout l) override;

  CodePoint m_codePoint;
  DisplayType m_displayType;
  const KDFont * m_font;
};

class CodePointLayout final : public Layout {
public:
  static void StripDisplayTypeFromCodePoints(Layout l);
  static void DistributeThousandDisplayType(Layout l, int start, int stop);

  CodePointLayout(const CodePointLayoutNode * n) : Layout(n) {}
  static CodePointLayout Builder(CodePoint c, const KDFont * font = KDFont::LargeFont);
  const KDFont * font() const { return const_cast<CodePointLayout *>(this)->node()->font(); }
  CodePoint codePoint() const { return const_cast<CodePointLayout *>(this)->node()->codePoint(); }
private:
  using Layout::node;
  CodePointLayoutNode * node() { return static_cast<CodePointLayoutNode *>(Layout::node()); }
};

}

#endif
