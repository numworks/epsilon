#ifndef POINCARE_COMBINED_CODEPOINT_LAYOUT_NODE_H
#define POINCARE_COMBINED_CODEPOINT_LAYOUT_NODE_H

#include <poincare/code_point_layout.h>
#include <poincare/print_float.h>

namespace Poincare {

class CombinedCodePointLayoutNode final : public CodePointLayoutNode {
public:
  CombinedCodePointLayoutNode(CodePoint mainCodePoint, CodePoint combinedCodePoint, const KDFont * font = k_defaultFont) :
    CodePointLayoutNode(mainCodePoint, font),
    m_combinedCodePoint(combinedCodePoint)
  {}

  // Layout
  Type type() const override { return Type::CombinedCodePointLayout; }

  // CodePointLayout
  CodePoint combinedCodePoint() const { return m_combinedCodePoint; }

  // LayoutNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // TreeNode
  size_t size() const override { return sizeof(CombinedCodePointLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "CombinedCodePointLayout";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    constexpr int bufferSize = 2 * CodePoint::MaxCodePointCharLength + 1;
    char buffer[bufferSize];
    serialize(buffer, bufferSize, Preferences::PrintFloatMode::Decimal, PrintFloat::k_floatNumberOfSignificantDigits);
    stream << " CombinedCodePoint=\"" << buffer << "\"";
  }
#endif
private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
  bool protectedIsIdenticalTo(Layout l) override;

  CodePoint m_combinedCodePoint;
};

class CombinedCodePointLayout final : public CodePointLayout {
public:
  CombinedCodePointLayout(const CodePointLayoutNode * n) : CodePointLayout(n) {}
  static CombinedCodePointLayout Builder(CodePoint mainCodePoint, CodePoint combinedCodePoint, const KDFont * font = KDFont::LargeFont);
  CodePoint combinedCodePoint() const { return const_cast<CombinedCodePointLayout *>(this)->node()->combinedCodePoint(); }
private:
  CombinedCodePointLayoutNode * node() { return static_cast<CombinedCodePointLayoutNode *>(Layout::node()); }
};

}

#endif
