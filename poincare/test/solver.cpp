#include <poincare/src/expression/list.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/solver.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/storage_context.h>
#include <poincare/src/memory/tree_stack.h>

#include "helper.h"
using namespace Poincare::Internal;

bool check_solutions(std::initializer_list<const char*> inputs,
                     std::initializer_list<const char*> outputs,
                     Solver::Error expectedError = Solver::Error::NoError) {
  Tree* equationSet = Poincare::Internal::List::PushEmpty();
  for (const char* equation : inputs) {
    NAry::AddChild(equationSet, TextToTree(equation));
  }
  Solver::Context context = Solver::Context();
  Solver::Error error = Solver::Error::NoError;
  Tree* solutions = Solver::ExactSolve(equationSet, &context, &error);
  quiz_assert(error == expectedError);
  if (solutions) {
    quiz_assert(solutions->numberOfChildren() == outputs.size());
    const Tree* solution = solutions->nextNode();
    for (const char* output : outputs) {
      Tree* expectedSolution = TextToTree(output);
      ProjectionContext projectionContext;
      Simplification::Simplify(expectedSolution, &projectionContext);
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

#if 0

QUIZ__CASE(pcj_solver) {
  // User variables
  StorageContext::SetTreeForIdentifier(2_e, "a");
  check_solutions({"a*x-2"}, {"x-1"});
  check_solutions({"a+x-2", "x"}, {"x"});
  check_solutions({"a+x-3", "x"}, {"a-3", "x"});
  StorageContext::DeleteTreeForIdentifier("a");
  // Errors
  check_solutions({"x+y+z", "x-y"}, {}, Solver::Error::TooManyVariables);
  check_solutions({"x^2", "y"}, {}, Solver::Error::NonLinearSystem);
  check_solutions({"y*(1+x)", "y-1"}, {}, Solver::Error::NonLinearSystem);
  check_solutions({"x*y+y", "y-1"}, {}, Solver::Error::NonLinearSystem);
  check_solutions({"identity(3)"}, {}, Solver::Error::EquationUndefined);
  // check_solutions({"x^2+1"}, {}, Solver::Error::EquationNonreal);
  // check_solutions({"sin(x)"}, {}, Solver::Error::RequireApproximateSolution);
}

#endif
