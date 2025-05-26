#ifndef POINCARE_EXPRESSION_SEQUENCE_H
#define POINCARE_EXPRESSION_SEQUENCE_H

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

class Sequence {
 public:
  enum class Type : uint8_t {
    Explicit = 0,
    SingleRecurrence = 1,
    DoubleRecurrence = 2
  };

  constexpr static int k_nameIndex = 0;
  constexpr static int k_mainExpressionIndex = 1;
  constexpr static int k_firstRankIndex = 2;
  constexpr static int k_firstInitialConditionIndex = 3;
  constexpr static int k_secondInitialConditionIndex = 4;

  constexpr static const char* k_sequenceNames[] = {"u", "v", "w"};

  static bool IsSequenceName(const char* name);
  static Type GetType(const Tree* sequence);
  static int InitialRank(const Tree* sequence);

  static Tree* PushMainExpressionName(const Tree* sequence);
  static Tree* PushInitialConditionName(const Tree* sequence,
                                        bool isFirstCondition = true);

  static bool MainExpressionContainsForbiddenTerms(
      const Tree* e, const char* name, Type type, int initialRank,
      bool recursion, bool systemSymbol, bool otherSequences);
};

}  // namespace Poincare::Internal
#endif
