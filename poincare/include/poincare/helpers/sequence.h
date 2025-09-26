#pragma once

#include <poincare/layout.h>
#include <poincare/user_expression.h>

namespace Poincare {

namespace SequenceHelper {

enum class RecursiveNotation {
  Default = 0,  // u(n+2) = u(n+1)+u(n)
  Shifted = 1,  // u(n) = u(n-1)+u(n-2)
};

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
bool MainExpressionContainsForbiddenTerms(
    UserExpression e, const SymbolContext& symbolContext, const char* name,
    Type type, int initialRank, RecursiveNotation notation, bool recursion,
    bool systemSymbol, bool otherSequences);

}  // namespace SequenceHelper

}  // namespace Poincare
