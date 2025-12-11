#include "helper.h"

#include <poincare/src/expression/advanced_reduction.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/systematic_reduction.h>

using namespace Poincare;
using namespace Poincare::Internal;

void deepSystematicReduce_and_operation_to(const Tree* input,
                                           Tree::Operation operation,
                                           const Tree* output) {
  Tree* tree = input->cloneTree();
  // Expand / contract expects a deep systematic reduced tree
  SystematicReduction::DeepReduce(tree);
  quiz_assert(operation(tree));
  assert_trees_are_equal(tree, output);
  tree->removeTree();
}

void expand_to(const Tree* input, const Tree* output) {
  deepSystematicReduce_and_operation_to(input, AdvancedReduction::DeepExpand,
                                        output);
}

void algebraic_expand_to(const Tree* input, const Tree* output) {
  deepSystematicReduce_and_operation_to(
      input, AdvancedReduction::DeepExpandAlgebraic, output);
}

void contract_to(const Tree* input, const Tree* output) {
  deepSystematicReduce_and_operation_to(input, AdvancedReduction::DeepContract,
                                        output);
}

void reduces_to_tree(const Tree* input, const Tree* output) {
  Tree* reduced = input->cloneTree();
  simplify(reduced, {}, false);
  assert_trees_are_equal(reduced, output);
  reduced->removeTree();
}

void simplifies_to(const char* input, const char* output,
                   ProjectionContext projectionContext, bool preserveInput) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        simplify(tree, projectionContext, true);
      },
      projectionContext,
      Poincare::PrintFloat::k_undefinedNumberOfSignificantDigits,
      preserveInput);
}

void simplifies_to(const Tree* input, const Tree* output,
                   ProjectionContext projectionContext) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        simplify(tree, projectionContext, true);
      },
      projectionContext,
      Poincare::PrintFloat::k_undefinedNumberOfSignificantDigits);
}

void projects_and_reduces_to(const char* input, const char* output,
                             ProjectionContext projectionContext) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        simplify(tree, projectionContext, false);
        // Beautify anyway to compare input and outputs
        ReductionTarget previousReductionTarget =
            projectionContext.m_reductionTarget;
        projectionContext.m_reductionTarget = ReductionTarget::User;
        Beautification::DeepBeautify(tree, projectionContext);
        projectionContext.m_reductionTarget = previousReductionTarget;
      },
      projectionContext);
}

void simplifies_to_no_beautif(const char* input, const char* output,
                              ProjectionContext projectionContext) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        simplify(tree, projectionContext, false);
      },
      projectionContext);
}
