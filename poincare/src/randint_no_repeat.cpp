#include <poincare/randint_no_repeat.h>
#include <poincare/approximation_helper.h>
#include <poincare/layout_helper.h>
#include <poincare/list_complex.h>
#include <poincare/randint.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

int RandintNoRepeatNode::numberOfChildren() const {
  return RandintNoRepeat::s_functionHelper.numberOfChildren();
}

Layout RandintNoRepeatNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(RandintNoRepeat(this), floatDisplayMode, numberOfSignificantDigits, RandintNoRepeat::s_functionHelper.aliasesList().mainAlias(), context);
}

int RandintNoRepeatNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, RandintNoRepeat::s_functionHelper.aliasesList().mainAlias());
}

template<typename T>
Evaluation<T> RandintNoRepeatNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  T ta = childAtIndex(0)->approximate(T(), approximationContext).toScalar();
  T tb = childAtIndex(1)->approximate(T(), approximationContext).toScalar();
  T tn = childAtIndex(2)->approximate(T(), approximationContext).toScalar();
  int a = static_cast<int>(ta), b = static_cast<int>(tb), n = static_cast<int>(tn);
  if (ta != a || tb != b || tn != n || n < 0 || b - a + 1 < n) {
    return Complex<T>::Undefined();
  }

  /* One simple method to generate n unique numbers in [a,b] is to shuffle {a,a+1,...,b} and take the first n elements. However, we cannot afford to allocate space for b-a+1 elements.
   * We generate a sorted list of random numbers in [a,b], as it makes it simple to assert uniqueness.
   * e.g. For the fourth random numbers between 0 and 9:
   *    - - 2 3 - - 6 - - - <= These numbers have already been generated.
   *    We take r = randint(0, 9-3), then map [0,6] to {0,1,4,5,7,8,9}
   *    For instance, if r = 2, it maps to 4. */
  ListComplex<T> sorted = ListComplex<T>::Builder();
  for (int i = 0; i < n; i++) {
    int r = Randint::RandomInt(a, b - i);
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
    int r = Randint::RandomInt(a, b - i);
    Evaluation<T> e = sorted.childAtIndex(r - a);
    sorted.removeChildInPlace(e, 0);
    result.addChildAtIndexInPlace(e, i, i);
  }
  assert(result.numberOfChildren() == n);

  return std::move(result);
}

}
