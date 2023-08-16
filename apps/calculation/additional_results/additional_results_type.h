#ifndef CALCULATION_ADDITIONAL_RESULTS_TYPE_CONTROLLER_H
#define CALCULATION_ADDITIONAL_RESULTS_TYPE_CONTROLLER_H

#include <poincare/expression.h>

namespace Calculation {

struct AdditionalResultsType {
  bool integer : 1;
  bool rational : 1;
  bool directTrigonometry : 1;
  bool inverseTrigonometry : 1;
  bool unit : 1;
  bool matrix : 1;
  bool vector : 1;
  bool complex : 1;
  bool function : 1;
  bool scientificNotation : 1;
  bool isNotEmpty() const {
    return integer || rational || directTrigonometry || inverseTrigonometry ||
           unit || matrix || vector || complex || function ||
           scientificNotation;
  }
};

AdditionalResultsType AdditionalResultsForExpressions(
    const Poincare::Expression input, const Poincare::Expression exactOutput,
    const Poincare::Expression approximateOutput);

bool ForbidAdditionalResults(const Poincare::Expression input,
                             const Poincare::Expression exactOutput,
                             const Poincare::Expression approximateOutput);

bool HasComplexAdditionalResults(const Poincare::Expression approximateOutput);
bool HasDirectTrigoAdditionalResults(const Poincare::Expression input,
                                     const Poincare::Expression exactOutput);
bool HasInverseTrigoAdditionalResults(const Poincare::Expression input,
                                      const Poincare::Expression exactOutput);
bool HasUnitAdditionalResults(const Poincare::Expression exactOutput);
bool HasVectorAdditionalResults(const Poincare::Expression exactOutput);
bool HasMatrixAdditionalResults(const Poincare::Expression exactOutput);
bool HasFunctionAdditionalResults(const Poincare::Expression input,
                                  const Poincare::Expression approximateOutput);
bool HasScientificNotationAdditionalResults(
    const Poincare::Expression approximateOutput);
bool HasIntegerAdditionalResults(const Poincare::Expression exactOutput);
bool HasRationalAdditionalResults(const Poincare::Expression exactOutput);

}  // namespace Calculation

#endif
