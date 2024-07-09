#ifndef CALCULATION_ADDITIONAL_RESULTS_TYPE_CONTROLLER_H
#define CALCULATION_ADDITIONAL_RESULTS_TYPE_CONTROLLER_H

#include <poincare/expression.h>

namespace Poincare::Internal {

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
  bool empty : 1;

  bool isNotEmpty() const {
    assert(!empty == integer || rational || directTrigonometry ||
           inverseTrigonometry || unit || matrix || vector || complex ||
           function || scientificNotation);
    return !empty;
  }

  bool isUninitialized() const {
    return !integer && !rational && !directTrigonometry &&
           !inverseTrigonometry && !unit && !matrix && !vector && !complex &&
           !function && !scientificNotation && !empty;
  }

  static AdditionalResultsType AdditionalResultsForExpressions(
      const Poincare::UserExpression input,
      const Poincare::UserExpression exactOutput,
      const Poincare::UserExpression approximateOutput,
      const Poincare::Preferences::CalculationPreferences
          calculationPreferences);

  static bool ForbidAdditionalResults(
      const Poincare::UserExpression input,
      const Poincare::UserExpression exactOutput,
      const Poincare::UserExpression approximateOutput);

  static bool HasComplex(const Poincare::UserExpression approximateOutput,
                         const Poincare::Preferences::CalculationPreferences
                             calculationPreferences);
  static bool HasDirectTrigo(const Poincare::UserExpression input,
                             const Poincare::UserExpression exactOutput,
                             const Poincare::Preferences::CalculationPreferences
                                 calculationPreferences);
  static bool HasInverseTrigo(
      const Poincare::UserExpression input,
      const Poincare::UserExpression exactOutput,
      const Poincare::Preferences::CalculationPreferences
          calculationPreferences);
  static bool HasUnit(const Poincare::UserExpression exactOutput,
                      const Poincare::Preferences::CalculationPreferences
                          calculationPreferences);
  static bool HasVector(const Poincare::UserExpression exactOutput,
                        const Poincare::UserExpression approximateOutput,
                        const Poincare::Preferences::CalculationPreferences
                            calculationPreferences);
  static bool HasMatrix(const Poincare::UserExpression approximateOutput);
  static bool HasFunction(const Poincare::UserExpression input,
                          const Poincare::UserExpression approximateOutput);
  static bool HasScientificNotation(
      const Poincare::UserExpression approximateOutput,
      const Poincare::Preferences::CalculationPreferences
          calculationPreferences);
  static bool HasInteger(const Poincare::UserExpression exactOutput);
  static bool HasRational(const Poincare::UserExpression exactOutput);
};

}  // namespace Poincare::Internal

namespace Calculation {
using AdditionalResultsType = Poincare::Internal::AdditionalResultsType;
}

#endif
