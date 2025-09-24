#pragma once

#include <poincare/src/memory/tree.h>
#include <poincare/symbol_context.h>

namespace Poincare::Internal {

class Sequence {
 public:
  constexpr static int k_nameIndex = 0;
  constexpr static int k_mainExpressionIndex = 1;
  constexpr static int k_firstRankIndex = 2;
  constexpr static int k_firstInitialConditionIndex = 3;
  constexpr static int k_secondInitialConditionIndex = 4;

  static int InitialRank(const Tree* sequence);

  static Tree* PushInitialConditionName(const Tree* sequence,
                                        bool isFirstCondition = true);

  static bool MainExpressionContainsForbiddenTerms(
      const Tree* e, const SymbolContext& symbolContext, const char* name,
      Type type, int initialRank, bool recursion, bool systemSymbol,
      bool otherSequences);
};

}  // namespace Poincare::Internal
