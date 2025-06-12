#ifndef POINCARE_HELPERS_SEQUENCE_H
#define POINCARE_HELPERS_SEQUENCE_H

#include <poincare/expression.h>

namespace Poincare {

namespace SequenceHelper {

enum class Type : uint8_t {
  Explicit = 0,
  SingleRecurrence = 1,
  DoubleRecurrence = 2
};

constexpr static const char* k_sequenceNames[] = {"u", "v", "w"};

// Layout
bool IsSequenceInsideRack(Layout l);
Layout ExtractExpressionAtRow(Layout l, int row = 0);
void SetFirstRank(Layout l, uint8_t firstRank);

// Expression
bool MainExpressionContainsForbiddenTerms(UserExpression e, Context* ctx,
                                          const char* name, Type type,
                                          int initialRank, bool recursion,
                                          bool systemSymbol,
                                          bool otherSequences);

}  // namespace SequenceHelper

}  // namespace Poincare

#endif
