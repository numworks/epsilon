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
  simplify(reduced, {.complexFormat = ComplexFormat::Real}, false);
  assert_trees_are_equal(reduced, output);
  reduced->removeTree();
}

void simplifies_to(const char* input, const char* output,
                   const SimplificationParameters& simplificationParameters,
                   bool preserveInput) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, const SimplificationParameters& simplificationParameters) {
        simplify(tree, simplificationParameters, true);
      },
      simplificationParameters,
      Poincare::PrintFloat::k_undefinedNumberOfSignificantDigits,
      preserveInput);
}

void simplifies_to(const Tree* input, const Tree* output,
                   const SimplificationParameters& simplificationParameters) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, const SimplificationParameters& simplificationParameters) {
        simplify(tree, simplificationParameters, true);
      },
      simplificationParameters,
      Poincare::PrintFloat::k_undefinedNumberOfSignificantDigits);
}

void projects_and_reduces_to(
    const char* input, const char* output,
    const SimplificationParameters& simplificationParameters) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, const SimplificationParameters& simplificationParameters) {
        simplify(tree, simplificationParameters, false);
        if (simplificationParameters.reductionTarget != ReductionTarget::User) {
          // Beautify anyway to compare input and outputs
          SimplificationParameters parametersForBeautification =
              simplificationParameters;
          parametersForBeautification.reductionTarget = ReductionTarget::User;
          Beautification::DeepBeautify(
              tree, ToBeautificationParameters(parametersForBeautification));
        } else {
          Beautification::DeepBeautify(
              tree, ToBeautificationParameters(simplificationParameters));
        }
      },
      simplificationParameters);
}

void simplifies_to_no_beautif(
    const char* input, const char* output,
    const SimplificationParameters& simplificationParameters) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, const SimplificationParameters& simplificationParameters) {
        simplify(tree, simplificationParameters, false);
      },
      simplificationParameters);
}
