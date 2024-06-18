#include <apps/shared/global_context.h>
#include <poincare/src/expression/equation_solver.h>
#include <poincare/src/expression/list.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_stack.h>

#include "helper.h"
using namespace Poincare::Internal;

using Solver = EquationSolver;

bool check_solutions(std::initializer_list<const char*> inputs,
                     std::initializer_list<const char*> outputs,
                     ProjectionContext projectionContext,
                     Solver::Error expectedError = Solver::Error::NoError) {
  Tree* equationSet = Poincare::Internal::List::PushEmpty();
  for (const char* equation : inputs) {
    NAry::AddChild(equationSet, TextToTree(equation));
  }
  Solver::Context context = Solver::Context();
  Solver::Error error = Solver::Error::NoError;
  Tree* solutions =
      Solver::ExactSolve(equationSet, &context, projectionContext, &error);
  quiz_assert(error == expectedError);
  if (solutions) {
    quiz_assert(solutions->numberOfChildren() == outputs.size());
    projectionContext.m_symbolic =
        context.overrideUserVariables
            ? SymbolicComputation::DoNotReplaceAnySymbol
            : SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
    const Tree* solution = solutions->nextNode();
    for (const char* output : outputs) {
      Tree* expectedSolution = TextToTree(output);
      Simplification::SimplifyWithAdaptiveStrategy(expectedSolution,
                                                   &projectionContext);
      quiz_assert(solution->treeIsIdenticalTo(expectedSolution));
      solution = solution->nextTree();
      expectedSolution->removeTree();
    }
    solutions->removeTree();
  } else {
    quiz_assert(outputs.size() == 0);
  }
  equationSet->removeTree();
  return true;
}

QUIZ_CASE(pcj_solver) {
  Shared::GlobalContext globalContext;
  assert(
      Ion::Storage::FileSystem::sharedFileSystem->numberOfRecords() ==
      Ion::Storage::FileSystem::sharedFileSystem->numberOfRecordsWithExtension(
          "sys"));
  ProjectionContext projCtx = {.m_context = &globalContext};

  check_solutions({"x-3+y", "y-x+1"}, {"x-2", "y-1"}, projCtx);
  check_solutions({"x+x"}, {"x"}, projCtx);
  check_solutions({"x+x+1"}, {"x+1/2"}, projCtx);
  check_solutions({"x+y", "y+x", "y-x+2"}, {"x-1", "y+1"}, projCtx);
  check_solutions({"1"}, {}, projCtx);
  check_solutions({"a-b", "b-c", "c-d", "d-f", "f-g", "g-a", "a+b+c+d+f+g+1"},
                  {"a+1/6", "b+1/6", "c+1/6", "d+1/6", "f+1/6", "g+1/6"},
                  projCtx);
  // User variables
  store("2→a", &globalContext);
  check_solutions({"a*x-2"}, {"x-1"}, projCtx);
  check_solutions({"a+x-2", "x"}, {"x"}, projCtx);
  check_solutions({"a+x-3", "x"}, {"a-3", "x"}, projCtx);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
  // Errors
  check_solutions({"x+y+z", "x-y"}, {}, projCtx,
                  Solver::Error::TooManyVariables);
  check_solutions({"x^2", "y"}, {}, projCtx, Solver::Error::NonLinearSystem);
  check_solutions({"y*(1+x)", "y-1"}, {}, projCtx,
                  Solver::Error::NonLinearSystem);
  check_solutions({"x*y+y", "y-1"}, {}, projCtx,
                  Solver::Error::NonLinearSystem);
  check_solutions({"identity(3)"}, {}, projCtx,
                  Solver::Error::EquationUndefined);

#if 0
  check_solutions({"x^2+1"}, {}, projCtx, Solver::Error::EquationNonreal);
  check_solutions({"sin(x)"}, {}, projCtx,
                  Solver::Error::RequireApproximateSolution);
#endif
}

void check_range(std::initializer_list<const char*> inputs, double min,
                 double max) {
  Tree* equationSet = Poincare::Internal::List::PushEmpty();
  for (const char* equation : inputs) {
    NAry::AddChild(equationSet, TextToTree(equation));
  }
  Poincare::Range1D<double> range = Solver::AutomaticInterval(equationSet);
  quiz_assert(range.min() == min);
  quiz_assert(range.max() == max);
}

QUIZ_CASE(pcj_solver_auto_range) {
  // TODO: import all tests from solver app
  check_range({"cos(x)-0"}, -15.5654296875, 15.5654296875);
}
