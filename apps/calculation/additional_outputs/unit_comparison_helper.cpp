#include "unit_comparison_helper.h"
#include <apps/i18n.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/unit.h>

using namespace Poincare;

namespace Calculation {

namespace UnitComparison {

constexpr static Unit::Representative massRepresentative = Unit::MassRepresentative::Default();

constexpr static ReferenceValue k_computer_weight = {
  I18n::Message::ComputerWeightTitle1,
  I18n::Message::ComputerWeightTitle2,
  I18n::Message::ComputerWeightSubtitle,
  &massRepresentative,
  3.0,
  0
};

const ReferenceValue * UpperReferenceValue(Poincare::Expression e, Expression * comparisonExpression, char * comparisonTextBuffer) {
  *comparisonExpression = Multiplication::Builder(Rational::Builder(3), Rational::Builder(5));
  comparisonTextBuffer[0] = '2';
  comparisonTextBuffer[1] = '.';
  comparisonTextBuffer[2] = '4';
  comparisonTextBuffer[3] = 0;
  return &k_computer_weight;
}

const ReferenceValue * LowerReferenceValue(Poincare::Expression e, Expression * comparisonExpression, char * comparisonTextBuffer) {
  *comparisonExpression = Multiplication::Builder(Rational::Builder(3), Rational::Builder(5));
  comparisonTextBuffer[0] = '4';
  comparisonTextBuffer[1] = '.';
  comparisonTextBuffer[2] = '6';
  comparisonTextBuffer[3] = '%';
  comparisonTextBuffer[4] = 0;
  return &k_computer_weight;
}

}

}
