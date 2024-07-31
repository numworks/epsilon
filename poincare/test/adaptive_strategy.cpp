#include <apps/shared/global_context.h>
#include <poincare/old/junior_expression.h>

#include <algorithm>
#include <cmath>
#include <string_view>

#include "helper.h"

void project_and_reduce(const char* input, const char* output,
                        ProjectionContext projectionContext = {
                            .m_complexFormat = ComplexFormat::Real}) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        Simplification::ProjectAndReduce(tree, &projectionContext, true);
      },
      projectionContext);
}

void simplify(const char* input, const char* output,
              ProjectionContext projectionContext = {
                  .m_complexFormat = ComplexFormat::Real}) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        Simplification::SimplifyWithAdaptiveStrategy(tree, &projectionContext);
      },
      projectionContext);
}

QUIZ_CASE(pcj_adaptive_strategy) {
  simplify("2^2^2", "16");
  simplify("2^2^2^2^2^2^2", "∞");
  project_and_reduce("2^2^2", "16");
  project_and_reduce("2^2^2^2^2^2^2", "inf");
}
