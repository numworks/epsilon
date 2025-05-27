#include <poincare/helpers/sequence.h>
#include <poincare/src/layout/grid.h>
#include <poincare/src/layout/sequence.h>

namespace Poincare {

bool SequenceHelper::IsSequenceInsideRack(Layout l) {
  return l.tree()->isRackLayout() && l.tree()->child(0)->isSequenceLayout();
}

Layout SequenceHelper::ExtractExpressionAtRow(Layout l, int row) {
  assert(IsSequenceInsideRack(l));
  const Internal::Grid* grid = Internal::Grid::From(l.tree()->child(0));
  assert(row < grid->numberOfRows());
  return grid->childAt(row, 1);
}

void SequenceHelper::SetFirstRank(Layout l, uint8_t firstRank) {
  assert(IsSequenceInsideRack(l));
  Internal::SequenceLayout::SetFirstRank(l.tree()->child(0), firstRank);
}

bool SequenceHelper::MainExpressionContainsForbiddenTerms(
    UserExpression e, const char* name, Internal::Sequence::Type type,
    int initialRank, bool recursion, bool systemSymbol, bool otherSequences) {
  return Internal::Sequence::MainExpressionContainsForbiddenTerms(
      e.tree(), name, type, initialRank, recursion, systemSymbol,
      otherSequences);
}

}  // namespace Poincare
