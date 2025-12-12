#pragma once

#include <poincare/src/expression/k_tree.h>
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

  static bool ExpressionHasValidDimension(const Tree* e,
                                          const SymbolContext& symbolContext);
  static bool MainExpressionContainsForbiddenTerms(
      const Tree* e, const SymbolContext& symbolContext, const char* name,
      Type type, int initialRank, bool shiftedNotation, bool recursion,
      bool systemSymbol, bool otherSequences);
  static bool IsOfValidDimension(const Tree* e,
                                 const SymbolContext& symbolContext);
  static Tree* InitialExpression(const char* name, Type type,
                                 bool shiftedNotation);
  static void UpdateMainExpressionForNotation(Tree* e, Type type,
                                              bool toShiftedNotation);

 private:
  constexpr static const Tree* k_defaultRank = KUnknownSymbol;
  constexpr static const Tree* k_firstFollowingRank = KAdd(KUnknownSymbol, 1_e);
  constexpr static const Tree* k_secondFollowingRank =
      KAdd(KUnknownSymbol, 2_e);
  constexpr static const Tree* k_firstPreviousRank = KSub(KUnknownSymbol, 1_e);
  constexpr static const Tree* k_secondPreviousRank = KSub(KUnknownSymbol, 2_e);
};

}  // namespace Poincare::Internal
