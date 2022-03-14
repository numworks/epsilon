#include "unit_comparison_helper.h"
#include "../app.h"
#include <apps/i18n.h>
#include <cmath>
#include <poincare/multiplication.h>
#include <poincare/print_float.h>
#include <poincare/float.h>
#include <poincare/unit.h>

using namespace Poincare;

namespace Calculation {

namespace UnitComparison {

constexpr static ReferenceValue k_empty_value = {
  (I18n::Message)0,
  (I18n::Message)0,
  (I18n::Message)0,
  0,
  0
};

constexpr static ReferenceValue k_computer_weight = {
  I18n::Message::ComputerWeightTitle1,
  I18n::Message::ComputerWeightTitle2,
  I18n::Message::ComputerWeightSubtitle,
  3.0,
  0
};

constexpr static ReferenceValue k_human_weight = {
  I18n::Message::HumanWeightTitle1,
  I18n::Message::HumanWeightTitle2,
  I18n::Message::HumanWeightSubtitle,
  6.5,
  1
};

constexpr static const char * k_mass_rootSymbol = "g";
constexpr static const ReferenceValue * k_mass_references[] = {&k_computer_weight, &k_human_weight, &k_empty_value};

int SetUpperAndLowerReferenceValues(Poincare::Expression e, const ReferenceValue ** referenceValues, Expression * expressions, char * textBuffer) {
  assert(e.type() == ExpressionNode::Type::Multiplication);

  // 1. Store value of input
  Expression firstChild = e.childAtIndex(0);
  double inputValue = static_cast<Float<double> &>(firstChild).value();

  // 2. Find table of corresponding unit.
  const ReferenceValue * const* valuesOfSameUnit = nullptr;
  Expression unitExpression;
  if (e.numberOfChildren() == 2) {
    unitExpression = e.childAtIndex(1);
  } else {
    Expression unitExpression = e.clone();
    // TODO
    // unitExpression.removeChildAtIndexInPlace(0);
  }
  if (unitExpression.type() == ExpressionNode::Type::Unit) {
    const char * rootSymbol = static_cast<Unit &>(unitExpression).representative()->rootSymbol();
    if (AreSameRootSymbol(rootSymbol, k_mass_rootSymbol)) {
      valuesOfSameUnit = k_mass_references;
    }
   }
  if (valuesOfSameUnit == nullptr) {
    return 0;
  }

  // 3. Iterate through table to find upper and lower values
  int index = 0;
  int upperIndex = -1;
  int lowerIndex = -1;
  while(valuesOfSameUnit[index]->value != 0) {
    if (EvaluateReferenceValueAtIndex(valuesOfSameUnit, index) >= inputValue) {
      upperIndex = index;
      lowerIndex = index - 1;
      break;
    }
    index ++;
  }
  if (upperIndex < 0) {
    lowerIndex = index - 1;
  }

  // 4. Find ratio for upper and lower values, then build results
  int nReturn = 0;
  if (lowerIndex != -1) {
    double lowerRatio = inputValue/EvaluateReferenceValueAtIndex(valuesOfSameUnit, lowerIndex);
    if (lowerRatio < 100) {
       // Reference value
      *(referenceValues + nReturn) = valuesOfSameUnit[lowerIndex];

      // Expression to print when user inputs EXE
      *(expressions + nReturn) = Multiplication::Builder(Float<double>::Builder(lowerRatio), Float<double>::Builder(EvaluateReferenceValueAtIndex(valuesOfSameUnit, lowerIndex)), unitExpression.clone());
      // TODO : Remove this
      // unitExpression.shallowReduce(ReductionContext(App::app()->localContext(), Poincare::Preferences::sharedPreferences()->complexFormat(), Poincare::Preferences::sharedPreferences()->angleUnit(),  Poincare::Preferences::sharedPreferences()->unitFormat(), Poincare::ExpressionNode::ReductionTarget::SystemForApproximation));

      // Text to print in menu
      FillRatioBuffer(&textBuffer[nReturn * k_sizeOfUnitComparisonBuffer], lowerRatio);

      nReturn++;
    }
  }
  if (upperIndex != -1) {
    double upperRatio = inputValue/EvaluateReferenceValueAtIndex(valuesOfSameUnit, upperIndex);
    if (upperRatio >= 0.01) {
       // Reference value
      *(referenceValues + nReturn)= valuesOfSameUnit[upperIndex];

      // Expression to print when user inputs EXE
      *(expressions + nReturn) = Multiplication::Builder(Float<double>::Builder(upperRatio), Float<double>::Builder(EvaluateReferenceValueAtIndex(valuesOfSameUnit, upperIndex)), unitExpression.clone());
      //unitExpression.shallowReduce(ReductionContext(App::app()->localContext(), Poincare::Preferences::sharedPreferences()->complexFormat(), Poincare::Preferences::sharedPreferences()->angleUnit(),  Poincare::Preferences::sharedPreferences()->unitFormat(), Poincare::ExpressionNode::ReductionTarget::SystemForApproximation));

      // Text to print in menu
      FillRatioBuffer(&textBuffer[nReturn * k_sizeOfUnitComparisonBuffer], upperRatio);

      nReturn++;
    }
  }
  return nReturn;
}

bool AreSameRootSymbol(const char * s1, const char * s2) {
  int i = 0;
  while (s1[i] != 0 && s2[i] !=0) {
    if (s1[i] != s2[i]) {
      return false;
    }
    i++;
  }
  return s1[i] == 0 && s2[i] ==0;
}

double EvaluateReferenceValueAtIndex(const ReferenceValue * const* table, int index) {
  return table[index]->value * std::pow(10.0, table[index]->exponantOfTen);
}

void FillRatioBuffer(char * textBuffer, double ratio) {
  assert(ratio < 100 && ratio >= 0.01);
  int bufferIndex = 0;
  bool withPercentage = false;
  if (ratio <= 1) {
    ratio = 100.0*ratio;
    withPercentage = true;
  }
  if (ratio >= 99.5) {
    textBuffer[0] = '1';
    textBuffer[1] = '0';
    textBuffer[2] = '0';
    bufferIndex = 3;
  } else {
    Poincare::PrintFloat::TextLengths l = PrintFloat::ConvertFloatToText<double>(ratio, textBuffer, 4, 4, 2, Preferences::PrintFloatMode::Decimal);
    bufferIndex = l.CharLength;
  }
  if (withPercentage) {
    textBuffer[bufferIndex] = '%';
    bufferIndex++;
    textBuffer[bufferIndex] = 0;
  }
}


}

}
