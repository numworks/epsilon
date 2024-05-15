#ifndef CALCULATION_UNIT_COMPARISON_HELPER_H
#define CALCULATION_UNIT_COMPARISON_HELPER_H

#include <apps/i18n.h>
#include <poincare/expression.h>
#include <poincare/old/unit.h>

namespace Calculation {

namespace UnitComparison {

/* If you add new reference values (in .cpp), they always need to be
 * sorted in increasing order.
 */

constexpr static int k_sizeOfUnitComparisonBuffer = 5;
constexpr static int k_sizeOfUnitBuffer = 30;
constexpr static int k_numberOfSignicativeDigits = 2;
constexpr static float k_maxPercentageRatioDisplay = 1.05;

struct ReferenceUnit {
  const char* SIUnit;
  const char* displayedUnit;
};

struct ReferenceValue {
  I18n::Message title1;
  I18n::Message title2;
  I18n::Message subtitle;
  float value;
};

int FindUpperAndLowerReferenceValues(
    double inputValue, Poincare::UserExpression orderedSIUnit,
    const ReferenceValue** returnReferenceValues,
    int* returnReferenceTableIndex);
bool ShouldDisplayUnitComparison(double inputValue,
                                 Poincare::UserExpression unit);
void FillRatioBuffer(double ratio, char* textBuffer, int bufferSize);
Poincare::UserExpression BuildComparisonExpression(
    double value, const ReferenceValue* referenceValue, int tableIndex);
}  // namespace UnitComparison

}  // namespace Calculation

#endif
