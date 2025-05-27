#ifndef POINCARE_HELPERS_SEQUENCE_H
#define POINCARE_HELPERS_SEQUENCE_H

#include <poincare/expression.h>
#include <poincare/src/expression/sequence.h>

namespace Poincare {

namespace SequenceHelper {
constexpr static const char* k_sequenceNames[] = {"u", "v", "w"};

// Layout
bool IsSequenceInsideRack(Layout l);
Layout ExtractExpressionAtRow(Layout l, int row = 0);
void SetFirstRank(Layout l, uint8_t firstRank);

// Expression
bool MainExpressionContainsForbiddenTerms(UserExpression e, const char* name,
                                          Internal::Sequence::Type type,
                                          int initialRank, bool recursion,
                                          bool systemSymbol,
                                          bool otherSequences);

}  // namespace SequenceHelper

}  // namespace Poincare

#endif
