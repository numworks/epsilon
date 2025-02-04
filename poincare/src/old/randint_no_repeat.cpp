#include <poincare/layout.h>
#include <poincare/old/approximation_helper.h>
#include <poincare/old/list_complex.h>
#include <poincare/old/randint.h>
#include <poincare/old/randint_no_repeat.h>
#include <poincare/old/serialization_helper.h>

namespace Poincare {

int RandintNoRepeatNode::numberOfChildren() const {
  return RandintNoRepeat::s_functionHelper.numberOfChildren();
}

size_t RandintNoRepeatNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      RandintNoRepeat::s_functionHelper.aliasesList().mainAlias());
}

OExpression RandintNoRepeatNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return RandintNoRepeat(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> RandintNoRepeatNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  if (approximationContext.withinReduce()) {
    // Return NAN to prevent the reduction from assuming anything at this point
    return Complex<T>::Undefined();
  }
  T ta = childAtIndex(0)->approximate(T(), approximationContext).toRealScalar();
  T tb = childAtIndex(1)->approximate(T(), approximationContext).toRealScalar();
  T tn = childAtIndex(2)->approximate(T(), approximationContext).toRealScalar();
  int a = static_cast<int>(ta), b = static_cast<int>(tb),
      n = static_cast<int>(tn);
  /* In all cases where b < a is true, b - a + 1 < n is also true unless we take
   * into account integer over(under)flow (and unless n = 0). */
  if (ta != a || tb != b || tn != n || n < 0 || b < a || b - a + 1 < n) {
    return Complex<T>::Undefined();
  }

  /* One simple method to generate n unique numbers in [a,b] is to shuffle
   * {a,a+1,...,b} and take the first n elements. However, we cannot afford to
   * allocate space for b-a+1 elements. We generate a sorted list of random
   * numbers in [a,b], as it makes it simple to assert uniqueness. e.g. For the
   * fourth random numbers between 0 and 9:
   *    - - 2 3 - - 6 - - - <= These numbers have already been generated.
   *    We take r = randint(0, 9-3), then map [0,6] to {0,1,4,5,7,8,9}
   *    For instance, if r = 2, it maps to 4. */
  ListComplex<T> sorted = ListComplex<T>::Builder();
  for (int i = 0; i < n; i++) {
    int r = Integer::RandomInt(static_cast<native_int_t>(a),
                               static_cast<native_int_t>(b - i))
                .extractedInt();
    assert(sorted.numberOfChildren() == i);
    int insertionIndex = i;
    for (int j = 0; j < i; j++) {
      int r2 = std::real(sorted.complexAtIndex(j));
      if (r >= r2) {
        r++;
      } else {
        insertionIndex = j;
        break;
      }
    }
    sorted.addChildAtIndexInPlace(Complex<T>::Builder(r), insertionIndex, i);
  }
  assert(sorted.numberOfChildren() == n);

  /* Then, shuffle the list. */
  ListComplex<T> result = ListComplex<T>::Builder();
  for (int i = 0; i < n; i++) {
    int r = Integer::RandomInt(0, static_cast<native_int_t>(n - 1 - i))
                .extractedInt();
    Evaluation<T> e = sorted.childAtIndex(r);
    sorted.removeChildInPlace(e, 0);
    result.addChildAtIndexInPlace(e, i, i);
  }
  assert(result.numberOfChildren() == n);

  return std::move(result);
}

OExpression RandintNoRepeat::shallowReduce(ReductionContext reductionContext) {
  SetReductionEncounteredUndistributedList(true);
  return *this;
}

}  // namespace Poincare
