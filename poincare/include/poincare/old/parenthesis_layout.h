#ifndef POINCARE_PARENTHESIS_LAYOUT_H
#define POINCARE_PARENTHESIS_LAYOUT_H

#include "autocompleted_bracket_pair_layout.h"
#include "layout_helper.h"

namespace Poincare {

class ParenthesisLayoutNode : public AutocompletedBracketPairLayoutNode {
 public:
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override {
    return serializeWithSymbol('(', ')', buffer, bufferSize, floatDisplayMode,
                               numberOfSignificantDigits);
  }

}  // namespace Poincare

#endif
