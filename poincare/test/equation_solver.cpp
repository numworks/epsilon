#include <apps/shared/global_context.h>
#include <poincare/src/expression/equation_solver.h>
#include <poincare/src/expression/list.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_stack.h>

#include "helper.h"
using namespace Poincare::Internal;

bool check_solutions(
    std::initializer_list<const char*> inputs,
    std::initializer_list<const char*> outputs,
    ProjectionContext projectionContext,
    EquationSolver::Error expectedError = EquationSolver::Error::NoError) {
  Tree* equationSet = Poincare::Internal::List::PushEmpty();
  for (const char* equation : inputs) {
    NAry::AddChild(equationSet, parse(equation));
  }
  EquationSolver::Context context = EquationSolver::Context();
  EquationSolver::Error error = EquationSolver::Error::NoError;
  Tree* solutions = EquationSolver::ExactSolve(equationSet, &context,
                                               projectionContext, &error);
  quiz_assert(error == expectedError);
  if (solutions) {
    quiz_assert(solutions->numberOfChildren() ==
                static_cast<int>(outputs.size()));
    projectionContext.m_symbolic =
        context.overrideUserVariables
            ? SymbolicComputation::DoNotReplaceAnySymbol
            : SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
    const Tree* solution = solutions->nextNode();
    for (const char* output : outputs) {
      Tree* expectedSolution = parse(output);
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

QUIZ_CASE(pcj_equation_solver) {
  Shared::GlobalContext globalContext;
  assert(
      Ion::Storage::FileSystem::sharedFileSystem->numberOfRecords() ==
      Ion::Storage::FileSystem::sharedFileSystem->numberOfRecordsWithExtension(
          "sys"));
  ProjectionContext projCtx = {.m_context = &globalContext};

  check_solutions({"x-3+y", "y-x+1"}, {"2", "1"}, projCtx);
  check_solutions({"x+x"}, {"0"}, projCtx);
  check_solutions({"x-π"}, {"π"}, projCtx);
  check_solutions({"x+x+1"}, {"-1/2"}, projCtx);
  check_solutions({"x+y", "y+x", "y-x+2"}, {"1", "-1"}, projCtx);

  check_solutions({"x+y"}, {"-t", "t"}, projCtx);
  check_solutions({"x+y-2", "y+2z+3"}, {"2*t+5", "-2*t-3", "t"}, projCtx);

  check_solutions({"t+u"}, {"-t1", "t1"}, projCtx);

  check_solutions({"a+b", "c+d"}, {"-t2", "t2", "-t1", "t1"}, projCtx);
  check_solutions({"x+y", "2x+2y"}, {"-t", "t"}, projCtx);

  check_solutions({"1"}, {}, projCtx);
  check_solutions({"a-b", "b-c", "c-d", "d-f", "f-g", "g-a", "a+b+c+d+f+g+1"},
                  {"-1/6", "-1/6", "-1/6", "-1/6", "-1/6", "-1/6"}, projCtx);
  // User variables
  store("2→a", &globalContext);
  check_solutions({"a*x-2"}, {"1"}, projCtx);
  check_solutions({"a+x-2", "x"}, {"0"}, projCtx);
  check_solutions({"a+x-3", "x"}, {"3", "0"}, projCtx);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
  // Errors
  check_solutions({"x^2", "y"}, {}, projCtx,
                  EquationSolver::Error::NonLinearSystem);
  check_solutions({"y*(1+x)", "y-1"}, {}, projCtx,
                  EquationSolver::Error::NonLinearSystem);
  check_solutions({"x*y+y", "y-1"}, {}, projCtx,
                  EquationSolver::Error::NonLinearSystem);
  check_solutions({"identity(3)"}, {}, projCtx,
                  EquationSolver::Error::EquationUndefined);

#if 0
  check_solutions({"x^2+1"}, {}, projCtx, EquationSolver::Error::EquationNonreal);
  check_solutions({"sin(x)"}, {}, projCtx,
                  EquationSolver::Error::RequireApproximateSolution);
#endif

  // Complex format detection
  projCtx.m_complexFormat = ComplexFormat::Real;
  store("i→a", &globalContext);
  check_solutions({"x-a", "x-a+y-root(-1,3)"}, {"i", "e^(π/3×i)"}, projCtx);
  check_solutions({"x-a", "x-a+y-root(-1,3)", "a-1"}, {"1", "1", "-1"},
                  projCtx);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();

  store("x+1→f(x)", &globalContext);
  check_solutions({"f(a)+x", "f(x)"}, {"0", "-1"}, projCtx);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
}

void check_range(const char* input, double min, double max) {
  Tree* equation = parse(input);
  Approximation::PrepareFunctionForApproximation(equation, "x",
                                                 ComplexFormat::Real);
  EquationSolver::Context context = EquationSolver::Context();
  Poincare::Range1D<double> range =
      EquationSolver::AutomaticInterval(equation, &context);
  quiz_assert(range.min() == min);
  quiz_assert(range.max() == max);
}

QUIZ_CASE(pcj_equation_solver_auto_range) {
  // TODO: import all tests from solver app
  check_range("cos(x)-0", -15.5654296875, 15.5654296875);
}
