#ifndef CALCULATION_ADDITIONAL_OUTPUTS_UNIT_COMPARISON_HELPER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_UNIT_COMPARISON_HELPER_H

#include <poincare/unit.h>
#include <apps/i18n.h>

namespace Calculation {

namespace UnitComparison {

constexpr static int k_sizeOfUnitComparisonBuffer = 5;
constexpr static int k_sizeOfUnitBuffer = 30;
constexpr static int k_numberOfSignicativeNumbers = 2;

typedef struct {
  const char * SIUnit;
  const char * displayedUnit;
} ReferenceUnit;

typedef struct {
  I18n::Message title1;
  I18n::Message title2;
  I18n::Message subtitle;
  double value;
} ReferenceValue;

int FindUpperAndLowerReferenceValues(double inputValue, Poincare::Expression unit, const ReferenceValue ** referenceValues, int * returnUnitIndex);

void FillRatioBuffer(double ratio, char * textBuffer);

Poincare::Expression BuildComparisonExpression(double value, const ReferenceValue * referenceValue, int unitIndex);
}

}

#endif
