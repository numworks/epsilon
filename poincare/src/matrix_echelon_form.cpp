#include <poincare/matrix_echelon_form.h>
#include <poincare/matrix_complex.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

int MatrixEchelonFormNode::numberOfChildren() const { return sNumberOfChildren; }

Expression MatrixEchelonFormNode::shallowReduce(const ReductionContext& reductionContext) {
  return MatrixEchelonForm(this).shallowReduce(reductionContext);
}

Layout MatrixEchelonFormNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(MatrixEchelonForm(this), floatDisplayMode, numberOfSignificantDigits, functionHelperName(), context);
}

int MatrixEchelonFormNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, functionHelperName());
}

template<typename T>
Evaluation<T> MatrixEchelonFormNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> ref;
  if (input.type() == EvaluationNode<T>::Type::MatrixComplex) {
    ref = static_cast<MatrixComplex<T>&>(input).ref(isFormReduced());
  } else {
    ref = Complex<T>::Undefined();
  }
  assert(!ref.isUninitialized());
  return ref;
}


Expression MatrixEchelonForm::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
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
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    bool couldComputeRef = false;
    Expression result = static_cast<Matrix&>(c).createRef(reductionContext, &couldComputeRef, isFormReduced());
    if (couldComputeRef) {
      replaceWithInPlace(result);
      return result;
    }
    // The matrix could not be transformed properly
    return *this;
  }
  return replaceWithUndefinedInPlace();
}

}
