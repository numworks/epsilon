#ifndef CALCULATION_ADDITIONAL_OUTPUTS_UNIT_COMPARISON_HELPER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_UNIT_COMPARISON_HELPER_H

#include <poincare/unit.h>
#include <apps/i18n.h>

namespace Calculation {

namespace UnitComparison {

using namespace Poincare;

constexpr static int k_sizeOfUnitComparisonBuffer = 5;

typedef struct {
  I18n::Message title1;
  I18n::Message title2;
  I18n::Message subtitle;
  double value;
  int8_t exponantOfTen;
} ReferenceValue;

int SetUpperAndLowerReferenceValues(Poincare::Expression e, const ReferenceValue ** referenceValues, Expression * expressions, char * textBuffer);

bool AreSameRootSymbol(const char * s1, const char * s2);
double EvaluateReferenceValueAtIndex(const ReferenceValue * const* table, int index);
void FillRatioBuffer(char * textBuffer, double ratio);


}

}

#endif
