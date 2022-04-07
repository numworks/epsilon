#include <poincare/list_median.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/list.h>
#include <poincare/list_sort.h>
#include <poincare/serialization_helper.h>
#include <float.h>

namespace Poincare {

const Expression::FunctionHelper ListMedian::s_functionHelper;

int ListMedianNode::numberOfChildren() const {
  return ListMedian::s_functionHelper.numberOfChildren();
}

int ListMedianNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListMedian::s_functionHelper.name());
}

Layout ListMedianNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListMedian(this), floatDisplayMode, numberOfSignificantDigits, ListMedian::s_functionHelper.name());
}

Expression ListMedianNode::shallowReduce(ReductionContext reductionContext) {
  return ListMedian(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListMedianNode::templatedApproximate(ApproximationContext approximationContext) const {
  ExpressionNode * child = childAtIndex(0);
  int n = child->numberOfChildren();
  if (child->type() != ExpressionNode::Type::List || n == 0) {
    return Complex<T>::Undefined();
  }

  int index1, index2;
  Preferences::ComplexFormat complexFormat = approximationContext.complexFormat();
  ListMedian(this).approximationHelper(&index1, &index2, approximationContext.context(), complexFormat, approximationContext.angleUnit());
  if (index1 < 0 || index2 < 0) {
    return Complex<T>::Undefined();
  }

  if (index1 == index2) {
    return child->childAtIndex(index1)->approximate(static_cast<T>(0.), approximationContext);
  }
  return Evaluation<T>::Product(
      Evaluation<T>::Sum(
        child->childAtIndex(index1)->approximate(static_cast<T>(0.), approximationContext),
        child->childAtIndex(index2)->approximate(static_cast<T>(0.), approximationContext),
        complexFormat),
      Complex<T>::Builder(0.5),
      complexFormat);
}

static void numberOfElementsLesserAndGreater(float x, const List list, int * lesser, int * greater, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  *lesser = *greater = 0;
  int n = list.numberOfChildren();
  for (int i = 0; i < n; i++) {
    float y = list.childAtIndex(i).approximateToScalar<float>(context, complexFormat, angleUnit);
    if (x < y) {
      *greater += 1;
    } else if (y < x) {
      *lesser += 1;
    }
  }
  assert(*lesser + *greater < n);
}

void ListMedian::approximationHelper(int * index1, int * index2, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  *index1 = *index2 = -1;
  assert(childAtIndex(0).type() == ExpressionNode::Type::List);
  Expression child = childAtIndex(0);
  List listChild = static_cast<List &>(child);
  int n = listChild.numberOfChildren();
  int k = n / 2;

  float upperBound = FLT_MAX;
  float lowerBound = -FLT_MAX;
  for (int i = 0; i < n; i++) {
    float x = listChild.childAtIndex(i).approximateToScalar<float>(context, complexFormat, angleUnit);
    if (x < lowerBound || upperBound < x) {
      continue;
    }
    int lesser, greater;
    numberOfElementsLesserAndGreater(x, listChild, &lesser, &greater, context, complexFormat, angleUnit);

    if (greater > k) {
      /* More than half the values are greater than x: the median is greater
       * than x. */
      if (x > lowerBound) {
        lowerBound = x;
      }
    } else if (lesser > k) {
      /* More than half the values are lesser than x: the median is lesser
       * than x. */
      if (x < upperBound) {
        upperBound = x;
      }
    } else if (lesser < k && greater < k) {
      /* There is a group of elements of equal values in the middle of the
       * series: their value is the median. */
      *index1 = *index2 = i;
    } else {
      if (greater == k) {
        /* Half the values are greater than x: x is the lower bound of the
         * median */
        *index1 = i;
        lowerBound = x;
      }
      if (lesser == k) {
        /* Half the values are lesser than x: x is the upper bound of the
         * median */
        *index2 = i;
        upperBound = x;
      }
    }

    if (*index1 >= 0 && *index2 >= 0) {
      return;
    }
  }
}

Expression ListMedian::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression sorted = ListSort::Builder(childAtIndex(0)).shallowReduce(reductionContext);
  int n = sorted.numberOfChildren();
  if (sorted.type() != ExpressionNode::Type::List || n == 0) {
    return replaceWithUndefinedInPlace();
  }

  Expression e;
  if (n % 2 == 1) {
    Expression e = sorted.childAtIndex(n / 2);
    replaceWithInPlace(e);
    return e;
  } else {
    Expression a = sorted.childAtIndex(n / 2 - 1), b = sorted.childAtIndex(n / 2);
    Addition sum = Addition::Builder(a, b);
    Division div = Division::Builder(sum, Rational::Builder(2));
    sum.shallowReduce(reductionContext);
    replaceWithInPlace(div);
    return div.shallowReduce(reductionContext);
  }
}

}
