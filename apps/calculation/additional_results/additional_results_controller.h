#ifndef CALCULATION_ADDITIONAL_RESULTS_CONTROLLER_H
#define CALCULATION_ADDITIONAL_RESULTS_CONTROLLER_H

#include <poincare/expression.h>

namespace Calculation {

class Calculation;

class AdditionalResultsController {
 public:
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

  static AdditionalResultsType AdditionalResultsForCalculation(
      Calculation* calculation);

  static bool ForbidAdditionalResults(
      const Poincare::Expression input, const Poincare::Expression exactOutput,
      const Poincare::Expression approximateOutput);

  static bool HasComplexAdditionalResults(
      const Poincare::Expression approximateOutput);
  static bool HasDirectTrigoAdditionalResults(
      const Poincare::Expression input, const Poincare::Expression exactOutput);
  static bool HasInverseTrigoAdditionalResults(
      const Poincare::Expression input, const Poincare::Expression exactOutput);
  static bool HasUnitAdditionalResults(const Poincare::Expression exactOutput);
  static bool HasVectorAdditionalResults(
      const Poincare::Expression exactOutput);
  static bool HasMatrixAdditionalResults(
      const Poincare::Expression exactOutput);
  static bool HasFunctionAdditionalResults(
      const Poincare::Expression input,
      const Poincare::Expression approximateOutput);
  static bool HasScientificNotationAdditionalResults(
      const Poincare::Expression approximateOutput);
  static bool HasIntegerAdditionalResults(
      const Poincare::Expression exactOutput);
  static bool HasRationalAdditionalResults(
      const Poincare::Expression exactOutput);
};

}  // namespace Calculation

#endif
