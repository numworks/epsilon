#ifndef LETTER_C_WITH_SUB_AND_SUPERSCRIPT_LAYOUT_NODE_H
#define LETTER_C_WITH_SUB_AND_SUPERSCRIPT_LAYOUT_NODE_H

#include "layout_helper.h"
#include "letter_with_sub_and_superscript_layout.h"

namespace Poincare {

class LetterCWithSubAndSuperscriptLayoutNode final
    : public LetterWithSubAndSuperscriptLayoutNode {
 public:
  // Layout
  Type otype() const override {
    return Type::LetterCWithSubAndSuperscriptLayout;
  }

  // SerializableNode
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "LetterCWithSubAndSuperscriptLayout";
  }
#endif

 private:
  void renderLetter(KDContext* ctx, KDPoint p, KDColor expressionColor,
                    KDColor backgroundColor) override;
};

class LetterCWithSubAndSuperscriptLayout final
    : public LayoutTwoChildren<LetterCWithSubAndSuperscriptLayout,
                               LetterCWithSubAndSuperscriptLayoutNode> {
 public:
  LetterCWithSubAndSuperscriptLayout() = delete;
};

}  // namespace Poincare

#endif
