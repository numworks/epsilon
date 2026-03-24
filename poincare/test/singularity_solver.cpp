#include <poincare/properties.h>
#include <poincare/src/equation_solver/equation_solver.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/variables.h>
#include <quiz.h>

#include <initializer_list>

#include "helper.h"

using namespace Poincare::Internal;

bool check_singularities(
    const char* input,
    std::initializer_list<const Tree*> expectedSingularities) {
  Tree* e =
      parse(input, Poincare::EmptySymbolContext{}, {.preserveInput = true});
  /* Replace "x" by the projected Var0 variable, as this is a requirement of
   * ExactSolveFindSingularities */
  Variables::ReplaceUserVariable(e, "x", 0,
                                 Poincare::ComplexProperties::Real());
  EquationSolver::SingularitySolverResult result =
      EquationSolver::ExactSolveFindSingularities(e,
                                                  Poincare::UserParameters{});
  if (expectedSingularities.size() == 0) {
    quiz_assert(result.exactSolutionList->isUndefined());
  } else {
    quiz_assert(result.error == Poincare::EquationSolver::Error::NoError);
    assert(result.exactSolutionList->isList());
    assert(result.exactSolutionList->numberOfChildren() ==
           static_cast<int>(expectedSingularities.size()));
    auto expectedIterator = expectedSingularities.begin();
    const Tree* computedTree = result.exactSolutionList->nextNode();
    while (expectedIterator < expectedSingularities.end()) {
      quiz_assert(computedTree->treeIsIdenticalTo(*expectedIterator));
      expectedIterator++;
      computedTree = computedTree->nextTree();
    }
  }
  result.exactSolutionList->removeTree();
  e->removeTree();
  return true;
}

QUIZ_CASE(pcj_singularity_solver) {
  check_singularities("1/(x^2+3x+2)", {-2_e, -1_e});
  check_singularities("1/(x^2+25)", {});
  check_singularities("ln(abs(x^2))", {0_e});
}
