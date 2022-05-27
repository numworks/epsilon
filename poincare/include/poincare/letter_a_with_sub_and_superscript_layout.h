#ifndef LETTER_A_WITH_SUB_AND_SUPERSCRIPT_LAYOUT_NODE_H
#define LETTER_A_WITH_SUB_AND_SUPERSCRIPT_LAYOUT_NODE_H

#include <poincare/letter_with_sub_and_superscript_layout.h>

namespace Poincare {

class LetterAWithSubAndSuperscriptLayoutNode final : public LetterWithSubAndSuperscriptLayoutNode {
public:
  // Layout
  Type type() const override { return Type::LetterAWithSubAndSuperscriptLayout; }

  // SerializableNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "LetterAWithSubAndSuperscriptLayout";
  }
#endif

private:
  void renderLetter(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class LetterAWithSubAndSuperscriptLayout final : public Layout {
public:
  static LetterAWithSubAndSuperscriptLayout Builder(Layout child0, Layout child1) { return TreeHandle::FixedArityBuilder<LetterAWithSubAndSuperscriptLayout, LetterAWithSubAndSuperscriptLayoutNode>({child0, child1}); }
  LetterAWithSubAndSuperscriptLayout() = delete;
};

}

#endif
