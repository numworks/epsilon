#include <poincare/unit_convert.h>
#include <poincare/code_point_layout.h>
#include <poincare/complex.h>
#include <poincare/context.h>
#include <poincare/division.h>
#include <poincare/float.h>
#include <poincare/horizontal_layout.h>
#include <poincare/infinity.h>
#include <poincare/multiplication.h>
#include <poincare/serialization_helper.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <ion.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <utility>

namespace Poincare {

Expression UnitConvertNode::shallowReduce(ReductionContext reductionContext) {
  return UnitConvert(this).shallowReduce(reductionContext);
}

int UnitConvertNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
   //TODO LEA factorize with Store
  constexpr int stringMaxSize = CodePoint::MaxCodePointCharLength + 1;
  char string[stringMaxSize];
  SerializationHelper::CodePoint(string, stringMaxSize, UCodePointRightwardsArrow);
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, string);
}

Layout UnitConvertNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  //TODO LEA factorize with Store
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  result.addChildAtIndex(CodePointLayout::Builder(UCodePointRightwardsArrow), result.numberOfChildren(), result.numberOfChildren(), nullptr);
  result.addOrMergeChildAtIndex(childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits), result.numberOfChildren(), false);
  return std::move(result);
}

template<typename T>
Evaluation<T> UnitConvertNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  /* If we are here, it means that the unit convert node was not shallowReduced.
   * Otherwise, it would have been replaced by the division of the value by the
   * unit. We thus return Undefined. */
  return Complex<T>::Undefined();
}

Expression UnitConvert::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  // UnitConvert the expression.
  Expression finalUnit = childAtIndex(1).clone();
  Expression division = Division::Builder(childAtIndex(0), childAtIndex(1));
  division = division.simplify(reductionContext);
  if (division.beautifiedExpressionHasUnits()) {
    return replaceWithUndefinedInPlace();
  }
  double floatValue = division.approximateToScalar<double>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
  if (std::isinf(floatValue)) {
    return Infinity::Builder(false); //FIXME sign?
  }
  if (std::isnan(floatValue)) {
    return Undefined::Builder();
  }
  division = Multiplication::Builder(Float<double>::Builder(floatValue), finalUnit);
  static_cast<Multiplication &>(division).mergeMultiplicationChildrenInPlace();
  replaceWithInPlace(division);
  return division;
}

template Evaluation<float> UnitConvertNode::templatedApproximate<float>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
template Evaluation<double> UnitConvertNode::templatedApproximate<double>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

}
