#include <poincare/determinant.h>
#include <poincare/addition.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <poincare/layout_helper.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/subtraction.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

int DeterminantNode::numberOfChildren() const { return Determinant::s_functionHelper.numberOfChildren(); }

Layout DeterminantNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(Determinant(this), floatDisplayMode, numberOfSignificantDigits, Determinant::s_functionHelper.aliasesList().mainAlias(), context);
}

int DeterminantNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Determinant::s_functionHelper.aliasesList().mainAlias());
}

template<typename T>
Evaluation<T> DeterminantNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), approximationContext);
  if (input.type() != EvaluationNode<T>::Type::MatrixComplex) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(static_cast<MatrixComplex<T>&>(input).determinant());
}

Expression DeterminantNode::shallowReduce(const ReductionContext& reductionContext) {
  return Determinant(this).shallowReduce(reductionContext);
}

Expression Determinant::shallowReduce(ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression c0 = childAtIndex(0);
  // det(A) = undef if A is not a matrix
  if (!c0.deepIsMatrix(reductionContext.context(), reductionContext.shouldCheckMatrices())) {
    return replaceWithUndefinedInPlace();
  }
  if (c0.type() == ExpressionNode::Type::Matrix) {
    Matrix m0 = static_cast<Matrix &>(c0);
    bool couldComputeDeterminant = true;
    Expression result = m0.determinant(reductionContext, &couldComputeDeterminant, true);
    if (couldComputeDeterminant) {
      assert(!result.isUninitialized());
      replaceWithInPlace(result);
      return result.shallowReduce(reductionContext);
    }
  }
  return *this;
}

}
