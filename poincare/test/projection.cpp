#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/simplification.h>

#include "helper.h"
#include "poincare/test/helpers/symbol_store.h"

using namespace Poincare::Internal;
using Poincare::AngleUnit;
using Poincare::ComplexFormat;
using Poincare::Strategy;

QUIZ_CASE(pcj_projection) {
  TreeRef ref(KCos(KSin(KPow(
      KPow(KPow(e_e, KLogBase(KLogBase(KLog(π_e), 2_e), e_e)), π_e), 3_e))));
  ProjectionContext ctx;
  ctx.m_complexFormat = ComplexFormat::Cartesian;
  ctx.m_strategy = Strategy::Default;
  ctx.m_angleUnit = AngleUnit::Radian;
  Simplification::ToSystem(ref, &ctx);
  assert_trees_are_equal(
      ref,
      KTrig(KTrig(KPow(KPow(KExp(KLn(KMult(
                                KLn(KMult(KLn(π_e), KPow(KLn(10_e), -1_e))),
                                KPow(KLn(2_e), -1_e)))),
                            π_e),
                       3_e),
                  1_e),
            0_e));

  CloneTreeOverTree(ref, KAdd(KCos(KSub(2065_e, 2065_e)), KPow("x"_e, 2_e)));
  CloneTreeOverTree(ref, KAdd(KCos(KSub(2065_e, 2065_e)), KPow(2_e, "x"_e),
                              KPow(KLn(e_e), KDiv(1_e, 10_e))));
  ctx.m_complexFormat = ComplexFormat::Cartesian;
  ctx.m_strategy = Strategy::ApproximateToFloat;
  ctx.m_angleUnit = AngleUnit::Radian;
  Simplification::ToSystem(ref, &ctx);
  assert_trees_are_equal(ref, KAdd(KPow(2_de, "x"_e), 2_de));

  ctx.m_complexFormat = ComplexFormat::Cartesian;
  ctx.m_strategy = Strategy::Default;
  ctx.m_angleUnit = AngleUnit::Degree;
  CloneTreeOverTree(ref, KCos(100_e));
  Simplification::ToSystem(ref, &ctx);
  assert_trees_are_equal(ref, KTrig(KMult(100_e, 1_e / 180_e, π_e), 0_e));

  ctx.m_complexFormat = ComplexFormat::Cartesian;
  ctx.m_strategy = Strategy::Default;
  ctx.m_angleUnit = AngleUnit::Radian;
  CloneTreeOverTree(ref, KSqrt(π_e));
  Simplification::ToSystem(ref, &ctx);
  assert_trees_are_equal(ref, KPow(π_e, 1_e / 2_e));

  ctx.m_complexFormat = ComplexFormat::Real;
  ctx.m_strategy = Strategy::Default;
  ctx.m_angleUnit = AngleUnit::Radian;
  CloneTreeOverTree(ref, KSqrt(π_e));
  Simplification::ToSystem(ref, &ctx);
  assert_trees_are_equal(ref, KPowReal(π_e, 1_e / 2_e));

  CloneTreeOverTree(ref, KACos(KASin(1_e / 2_e)));
  Simplification::ToSystem(ref, &ctx);
  assert_trees_are_equal(ref, KATrig(KATrig(1_e / 2_e, 1_e), 0_e));

  ref->removeTree();
}

void replaces_to(const Tree* input, const Tree* output,
                 const Poincare::Context& context,
                 Poincare::SymbolicComputation symbolicPolicy) {
  Tree* e = input->cloneTree();
  Poincare::Internal::Projection::DeepReplaceUserNamed(e, context,
                                                       symbolicPolicy);
  quiz_assert(e->treeIsIdenticalTo(output));
  e->removeTree();
}

QUIZ_CASE(pcj_projection_variable_replacement) {
  PoincareTest::SymbolStore symbolStore;
  symbolStore.setExpressionForUserNamed(3_e, "x"_e);
  replaces_to(KMult("x"_e, "y"_e), KMult("x"_e, "y"_e), symbolStore,
              Poincare::SymbolicComputation::KeepAllSymbols);
  replaces_to(KMult("x"_e, "y"_e), KMult(3_e, "y"_e), symbolStore,
              Poincare::SymbolicComputation::ReplaceDefinedSymbols);
  replaces_to(KMult("x"_e, "y"_e), KMult(3_e, KNotDefined), symbolStore,
              Poincare::SymbolicComputation::ReplaceAllSymbols);
  replaces_to(KMult("x"_e, "y"_e), KMult(KNotDefined, KNotDefined), symbolStore,
              Poincare::SymbolicComputation::ReplaceAllSymbolsWithUndefined);

  symbolStore.setExpressionForUserNamed(KPow(KUnknownSymbol, 2_e),
                                        KFun<"f">(KUnknownSymbol));
  replaces_to(KFun<"f">(KAdd("x"_e, "y"_e)), KFun<"f">(KAdd("x"_e, "y"_e)),
              symbolStore, Poincare::SymbolicComputation::KeepAllSymbols);
  replaces_to(KFun<"f">(KAdd("x"_e, "y"_e)), KPow(KAdd("x"_e, "y"_e), 2_e),
              symbolStore,
              Poincare::SymbolicComputation::ReplaceDefinedFunctions);
  replaces_to(KFun<"f">(KAdd("x"_e, "y"_e)), KPow(KAdd(3_e, "y"_e), 2_e),
              symbolStore,
              Poincare::SymbolicComputation::ReplaceDefinedSymbols);
  replaces_to(KFun<"f">(KAdd("x"_e, "y"_e)), KPow(KAdd(3_e, KNotDefined), 2_e),
              symbolStore, Poincare::SymbolicComputation::ReplaceAllSymbols);
  replaces_to(KFun<"f">(KAdd("x"_e, "y"_e)), KNotDefined, symbolStore,
              Poincare::SymbolicComputation::ReplaceAllSymbolsWithUndefined);

  replaces_to(KIntegral("t"_e, "y"_e, "x"_e, KFun<"f">("t"_e)),
              KIntegral("t"_e, "y"_e, "x"_e, KFun<"f">("t"_e)), symbolStore,
              Poincare::SymbolicComputation::KeepAllSymbols);
  replaces_to(KIntegral("t"_e, "y"_e, "x"_e, KFun<"f">("t"_e)),
              KIntegral("t"_e, "y"_e, "x"_e, KPow("t"_e, 2_e)), symbolStore,
              Poincare::SymbolicComputation::ReplaceDefinedFunctions);
  replaces_to(KIntegral("t"_e, "y"_e, "x"_e, KFun<"f">("t"_e)),
              KIntegral("t"_e, "y"_e, 3_e, KPow("t"_e, 2_e)), symbolStore,
              Poincare::SymbolicComputation::ReplaceDefinedSymbols);
  /* ReplaceAllSymbols and ReplaceAllSymbolsWithUndefined make more sense on
   * SystemExpressions, where user symbols representing local parametric
   * variables have been replaced with Var0, Var1 etc. */
  replaces_to(KIntegral("t"_e, "y"_e, "x"_e, KFun<"f">(KVarX)),
              KIntegral("t"_e, KNotDefined, 3_e, KPow(KVarX, 2_e)), symbolStore,
              Poincare::SymbolicComputation::ReplaceAllSymbols);
  replaces_to(KIntegral("t"_e, "y"_e, "x"_e, KFun<"f">(KVarX)),
              KIntegral("t"_e, KNotDefined, KNotDefined, KNotDefined),
              symbolStore,
              Poincare::SymbolicComputation::ReplaceAllSymbolsWithUndefined);
}
