#include <poincare/list_sort.h>
#include <poincare/layout_helper.h>
#include <poincare/helpers.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

const Expression::FunctionHelper ListSort::s_functionHelper;

int ListSortNode::numberOfChildren() const {
  return ListSort::s_functionHelper.numberOfChildren();
}

int ListSortNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListSort::s_functionHelper.name());
}

Layout ListSortNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListSort(this), floatDisplayMode, numberOfSignificantDigits, ListSort::s_functionHelper.name());
}

Expression ListSortNode::shallowReduce(ReductionContext reductionContext) {
  return ListSort(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListSortNode::templatedApproximate(ApproximationContext approximationContext) const {
  return Complex<T>::Undefined();
}

Expression ListSort::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression child = childAtIndex(0);
  if (child.type() != ExpressionNode::Type::List) {
    return replaceWithUndefinedInPlace();
  }

  void * pack[] = { &child, &reductionContext };
  Helpers::Sort(
      // Swap
      [](int i, int j, void * ctx, int n) {
        void ** p = reinterpret_cast<void **>(ctx);
        Expression * e = reinterpret_cast<Expression *>(p[0]);
        assert(e->numberOfChildren() == n && 0 <= i && 0 <= j && i < n && j < n);
        e->swapChildrenInPlace(i, j);
      },
      // Compare
      [](int i, int j, void * ctx, int n) {
        void ** p = reinterpret_cast<void **>(ctx);
        Expression * e = reinterpret_cast<Expression *>(p[0]);
        ExpressionNode::ReductionContext * r = reinterpret_cast<ExpressionNode::ReductionContext *>(p[1]);
        assert(e->numberOfChildren() == n && 0 <= i && 0 <= j && i < n && j < n);
        Context * context = r->context();
        Preferences::ComplexFormat complexFormat = r->complexFormat();
        Preferences::AngleUnit angleUnit = r->angleUnit();
        Expression cI = e->childAtIndex(i), cJ = e->childAtIndex(j);
        float xI = cI.approximateToScalar<float>(context, complexFormat, angleUnit, true);
        float xJ = cJ.approximateToScalar<float>(context, complexFormat, angleUnit, true);
        return std::isnan(xI) || xI > xJ;
      },
      pack,
      child.numberOfChildren());
  replaceWithInPlace(child);
  return child;
}

template Evaluation<float> ListSortNode::templatedApproximate<float>(ApproximationContext approximationContext) const;
template Evaluation<double> ListSortNode::templatedApproximate<double>(ApproximationContext approximationContext) const;

}
