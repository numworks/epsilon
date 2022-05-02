#include <poincare/inv_student.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <poincare/student_distribution.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper InvStudent::s_functionHelper;

int InvStudentNode::numberOfChildren() const { return InvStudent::s_functionHelper.numberOfChildren(); }

Layout InvStudentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(InvStudent(this), floatDisplayMode, numberOfSignificantDigits, InvStudent::s_functionHelper.name());
}

int InvStudentNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, InvStudent::s_functionHelper.name());
}

Expression InvStudentNode::shallowReduce(ReductionContext reductionContext) {
  return InvStudent(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> InvStudentNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> aEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> kEvaluation = childAtIndex(1)->approximate(T(), approximationContext);

  T a = aEvaluation.toScalar();
  T k = kEvaluation.toScalar();

  // CumulativeDistributiveInverseForProbability handles bad mu and var values
  return Complex<T>::Builder(StudentDistribution::CumulativeDistributiveInverseForProbability<T>(a, k));
}

Expression InvStudent::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    bool stopReduction = false;
    Expression e = StudentDistributionFunction::shallowReduce(reductionContext.context(), &stopReduction);
    if (stopReduction) {
      return e;
    }
  }
  Expression a = childAtIndex(0);
  Expression k = childAtIndex(1);
  Context * context = reductionContext.context();

  // Check a
  if (a.deepIsMatrix(context)) {
    return replaceWithUndefinedInPlace();
  }
  if (a.type() != ExpressionNode::Type::Rational) {
    return *this;
  }

  // Special values

  // Undef if a < 0 or a > 1
  Rational rationalA = static_cast<Rational &>(a);
  if (rationalA.isNegative()) {
    return replaceWithUndefinedInPlace();
  }
  Integer num = rationalA.unsignedIntegerNumerator();
  Integer den = rationalA.integerDenominator();
  if (den.isLowerThan(num)) {
    return replaceWithUndefinedInPlace();
  }

  // -INF if a == 0 and +INF if a == 1
  bool is0 = rationalA.isZero();
  bool is1 = !is0 && rationalA.isOne();
  if (is0 || is1) {
    Expression result = Infinity::Builder(is0);
    replaceWithInPlace(result);
    return result;
  }

  // 0 if a == 0.5
  if (rationalA.isHalf()) {
    Expression zero = Rational::Builder(0);
    replaceWithInPlace(zero);
    return zero;
  }

  return *this;
}

}
