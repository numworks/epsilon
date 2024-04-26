#ifndef LETTER_A_WITH_SUB_AND_SUPERSCRIPT_LAYOUT_NODE_H
#define LETTER_A_WITH_SUB_AND_SUPERSCRIPT_LAYOUT_NODE_H

#include "layout_helper.h"
#include "letter_with_sub_and_superscript_layout.h"

namespace Poincare {

class LetterAWithSubAndSuperscriptLayoutNode final
    : public LetterWithSubAndSuperscriptLayoutNode {
 public:
  // Layout
  Type otype() const override {
    return Type::LetterAWithSubAndSuperscriptLayout;
  }

  // SerializableNode
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "LetterAWithSubAndSuperscriptLayout";
  }
#endif

 private:
  constexpr static KDCoordinate k_barHeight = 6;
  void renderLetter(KDContext* ctx, KDPoint p, KDColor expressionColor,
                    KDColor backgroundColor) override;
};

class LetterAWithSubAndSuperscriptLayout final
    : public LayoutTwoChildren<LetterAWithSubAndSuperscriptLayout,
                               LetterAWithSubAndSuperscriptLayoutNode> {
 public:
  LetterAWithSubAndSuperscriptLayout() = delete;
};

}  // namespace Poincare

#endif
