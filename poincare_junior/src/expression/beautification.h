#ifndef POINCARE_EXPRESSION_BEAUTIFICATION_H
#define POINCARE_EXPRESSION_BEAUTIFICATION_H

#include <poincare_junior/src/memory/edition_reference.h>

#include "context.h"
#include "projection.h"

namespace PoincareJ {

class Beautification {
 public:
  static float DegreeForSortingAddition(const Tree* expr, bool symbolsOnly);
  static void SplitMultiplication(const Tree* expr, EditionReference& numerator,
                                  EditionReference& denominator);
  static bool BeautifyIntoDivision(Tree* expr);
  static bool AddUnits(Tree* expr, ProjectionContext projectionContext);

  static bool ShallowBeautify(Tree* node, void* context = nullptr);
  EDITION_REF_WRAP_1D(ShallowBeautify, void*, nullptr);
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
  EDITION_REF_WRAP(ShallowBeautifyPercent);
};

}  // namespace PoincareJ

#endif
