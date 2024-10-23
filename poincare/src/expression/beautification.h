#ifndef POINCARE_EXPRESSION_BEAUTIFICATION_H
#define POINCARE_EXPRESSION_BEAUTIFICATION_H

#include <poincare/src/memory/tree_ref.h>

#include "context.h"
#include "projection.h"

namespace Poincare::Internal {

class Beautification {
  friend class Approximation;

 public:
  static float DegreeForSortingAddition(const Tree* e, bool symbolsOnly);
  static bool AddUnits(Tree* e, ProjectionContext projectionContext);
  static bool DeepBeautify(Tree* e, ProjectionContext projectionContext = {});
  TREE_REF_WRAP_1D(DeepBeautify, ProjectionContext, {});

  /* Create a Tree to represent a complex value according to the format, for
   * instance 0+1*i => <Constant i> in Cartesian mode. */
  template <typename T>
  static Tree* PushBeautifiedComplex(std::complex<T> value,
                                     ComplexFormat complexFormat);

 private:
  static bool TurnIntoPolarForm(Tree* e, Dimension dim,
                                const ProjectionContext& projectionContext);
  static bool DeepBeautifyAngleFunctions(Tree* e, AngleUnit angleUnit,
                                         bool* simplifyParent);
  static bool ShallowBeautifyAngleFunctions(Tree* e, AngleUnit angleUnit,
                                            bool* simplifyParent);
  static bool ShallowBeautifyPercent(Tree* e);
  static bool ShallowBeautifyOppositesDivisionsRoots(Tree* e, void* context);
  static bool ShallowBeautify(Tree* e, void* context);
  static bool ShallowBeautifySpecialDisplays(Tree* e, void* context);
};

}  // namespace Poincare::Internal

#endif
