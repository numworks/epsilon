#include <poincare/matrix_transpose.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <cmath>

namespace Poincare {

int MatrixTransposeNode::numberOfChildren() const { return MatrixTranspose::s_functionHelper.numberOfChildren(); }

Expression MatrixTransposeNode::shallowReduce(const ReductionContext& reductionContext) {
  return MatrixTranspose(this).shallowReduce(reductionContext);
}

Layout MatrixTransposeNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(MatrixTranspose(this), floatDisplayMode, numberOfSignificantDigits, MatrixTranspose::s_functionHelper.aliasesList().mainAlias(), context);
}

int MatrixTransposeNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, MatrixTranspose::s_functionHelper.aliasesList().mainAlias());
}

template<typename T>
Evaluation<T> MatrixTransposeNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> transpose;
  if (input.type() == EvaluationNode<T>::Type::MatrixComplex) {
    transpose = static_cast<MatrixComplex<T>&>(input).transpose();
  } else {
    transpose = input;
  }
  assert(!transpose.isUninitialized());
  return transpose;
}


Expression MatrixTranspose::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::UnitReduction::BanUnits
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    Expression result = static_cast<Matrix&>(c).createTranspose();
    replaceWithInPlace(result);
    return result;
  }
  if (c.deepIsMatrix(reductionContext.context())) {
    return *this;
  }
  replaceWithInPlace(c);
  return c;
}

}
