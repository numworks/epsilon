#ifndef CALCULATION_ADDITIONAL_OUTPUTS_UNIT_COMPARISON_HELPER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_UNIT_COMPARISON_HELPER_H

#include <apps/i18n.h>
#include <poincare/unit.h>

namespace Calculation {

namespace UnitComparison {

/* If you add new reference values (in .cpp), they always need to be
 * sorted in increasing order.
 */

constexpr static int k_sizeOfUnitComparisonBuffer = 5;
constexpr static int k_sizeOfUnitBuffer = 30;
constexpr static int k_numberOfSignicativeDigits = 2;
constexpr static float k_maxPercentageRatioDisplay = 1.05;

typedef struct {
  const char* SIUnit;
  const char* displayedUnit;
} ReferenceUnit;

typedef struct {
  I18n::Message title1;
  I18n::Message title2;
  I18n::Message subtitle;
  float value;
} ReferenceValue;

int FindUpperAndLowerReferenceValues(
    double inputValue, Poincare::Expression orderedSIUnit,
    const ReferenceValue** returnReferenceValues,
    int* returnReferenceTableIndex);
bool ShouldDisplayUnitComparison(double inputValue, Poincare::Expression unit);
void FillRatioBuffer(double ratio, char* textBuffer, int bufferSize);
Poincare::Expression BuildComparisonExpression(
    double value, const ReferenceValue* referenceValue, int tableIndex);
}  // namespace UnitComparison

}  // namespace Calculation

#endif
