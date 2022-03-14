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
  const Unit::Representative * unitRepresentative;
  float value;
  int8_t exponantOfTen;
} ReferenceValue;

const ReferenceValue * UpperReferenceValue(Poincare::Expression e, Expression * comparisonExpression, char * comparisonTextBuffer);
const ReferenceValue * LowerReferenceValue(Poincare::Expression e, Expression * comparisonExpression, char * comparisonTextBuffer);

}

}

#endif
