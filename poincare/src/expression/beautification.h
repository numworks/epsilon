#ifndef POINCARE_EXPRESSION_BEAUTIFICATION_H
#define POINCARE_EXPRESSION_BEAUTIFICATION_H

#include <poincare/src/memory/tree_ref.h>

#include "context.h"
#include "projection.h"

namespace Poincare::Internal {

class Beautification {
  friend class Approximation;

 public:
  static float DegreeForSortingAddition(const Tree* expr, bool symbolsOnly);
  static void SplitMultiplication(const Tree* expr, TreeRef& numerator,
                                  TreeRef& denominator, bool* needOpposite,
                                  bool* needI);
  static bool BeautifyIntoDivision(Tree* expr);
  static bool AddUnits(Tree* expr, ProjectionContext projectionContext);

  static bool SafeShallowBeautify(Tree* node, void* context = nullptr);
  EDITION_REF_WRAP_1D(SafeShallowBeautify, void*, nullptr);
  static bool DeepBeautify(Tree* node,
                           ProjectionContext projectionContext = {});
  EDITION_REF_WRAP_1D(DeepBeautify, ProjectionContext, {});

  /* Create a Tree to represent a complex value according to the format, for
   * instance 0+1*i => <Constant i> in Cartesian mode. */
  template <typename T>
  static Tree* PushBeautifiedComplex(std::complex<T> value,
                                     ComplexFormat complexFormat);

 private:
  static bool DeepBeautifyAngleFunctions(Tree* tree, AngleUnit angleUnit,
                                         bool* simplifyParent);
  static bool ShallowBeautifyAngleFunctions(Tree* tree, AngleUnit angleUnit);
  static bool ShallowBeautifyPercent(Tree* tree);
  static bool ShallowBeautify(Tree* node);
  static bool ShallowBubbleUpDivision(Tree* e);
  static bool DeepBubbleUpDivision(Tree* e);
  static void ReduceMultiplication(Tree* e);
};

}  // namespace Poincare::Internal

#endif
