#ifndef POINCARE_EXPRESSION_SEQUENCE_H
#define POINCARE_EXPRESSION_SEQUENCE_H

#include <poincare/old/context.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

class Sequence {
 public:
  constexpr static int k_nameIndex = 0;
  constexpr static int k_mainExpressionIndex = 1;
  constexpr static int k_firstRankIndex = 2;
  constexpr static int k_firstInitialConditionIndex = 3;
  constexpr static int k_secondInitialConditionIndex = 4;

  static bool IsSequenceName(const char* name);
  static int InitialRank(const Tree* sequence);

  static Tree* PushMainExpressionName(const Tree* sequence);
  static Tree* PushInitialConditionName(const Tree* sequence,
                                        bool isFirstCondition = true);

  static bool MainExpressionContainsForbiddenTerms(
      const Tree* e, Context* ctx, const char* name, Type type, int initialRank,
      bool recursion, bool systemSymbol, bool otherSequences);
};

}  // namespace Poincare::Internal
#endif
