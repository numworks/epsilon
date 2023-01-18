#ifndef POINCARE_COMBINED_CODEPOINT_LAYOUT_NODE_H
#define POINCARE_COMBINED_CODEPOINT_LAYOUT_NODE_H

#include <poincare/code_point_layout.h>
#include <poincare/print_float.h>

namespace Poincare {

class CombinedCodePointsLayoutNode final : public CodePointLayoutNode {
public:
  CombinedCodePointsLayoutNode(CodePoint mainCodePoint, CodePoint CombinedCodePoints) :
    CodePointLayoutNode(mainCodePoint),
    m_CombinedCodePoints(CombinedCodePoints)
  {}

  // Layout
  Type type() const override { return Type::CombinedCodePointsLayout; }

  // CodePointLayout
  CodePoint CombinedCodePoints() const { return m_CombinedCodePoints; }

  // LayoutNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // TreeNode
  size_t size() const override { return sizeof(CombinedCodePointsLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "CombinedCodePointsLayout";
  }
  void logAttributes(std::ostream & stream) const override {
    constexpr int bufferSize = 2 * CodePoint::MaxCodePointCharLength + 1;
    char buffer[bufferSize];
    serialize(buffer, bufferSize, Preferences::PrintFloatMode::Decimal, PrintFloat::k_floatNumberOfSignificantDigits);
    stream << " CombinedCodePoints=\"" << buffer << "\"";
  }
#endif

  bool isNotEqualOperator() const { return m_codePoint == '=' && m_CombinedCodePoints == UCodePointCombiningLongSolidusOverlay;}
private:
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
  bool protectedIsIdenticalTo(Layout l) override;

  CodePoint m_CombinedCodePoints;
};

class CombinedCodePointsLayout final : public CodePointLayout {
public:
  CombinedCodePointsLayout(const CodePointLayoutNode * n) : CodePointLayout(n) {}
  static CombinedCodePointsLayout Builder(CodePoint mainCodePoint, CodePoint CombinedCodePoints);
  CodePoint CombinedCodePoints() const { return const_cast<CombinedCodePointsLayout *>(this)->node()->CombinedCodePoints(); }
  CombinedCodePointsLayoutNode * node() { return static_cast<CombinedCodePointsLayoutNode *>(Layout::node()); }
};

}

#endif
