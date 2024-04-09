#include <poincare/layout.h>
#include <poincare/old/matrix.h>
#include <poincare/old/matrix_complex.h>
#include <poincare/old/matrix_echelon_form.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>

namespace Poincare {

int MatrixEchelonFormNode::numberOfChildren() const {
  return sNumberOfChildren;
}

OExpression MatrixEchelonFormNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return MatrixEchelonForm(this).shallowReduce(reductionContext);
}

size_t MatrixEchelonFormNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode,
                                     numberOfSignificantDigits,
                                     functionHelperName());
}

template <typename T>
Evaluation<T> MatrixEchelonFormNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> ref;
  if (input.otype() == EvaluationNode<T>::Type::MatrixComplex) {
    ref = static_cast<MatrixComplex<T>&>(input).ref(isFormReduced());
  } else {
    ref = Complex<T>::Undefined();
  }
  assert(!ref.isUninitialized());
  return ref;
}

OExpression MatrixEchelonForm::shallowReduce(
    ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  OExpression c = childAtIndex(0);
  if (c.otype() == ExpressionNode::Type::OMatrix) {
    bool couldComputeRef = false;
    OExpression result = static_cast<OMatrix&>(c).createRef(
        reductionContext, &couldComputeRef, isFormReduced());
    if (couldComputeRef) {
      replaceWithInPlace(result);
      return result;
    }
    // The matrix could not be transformed properly
    return *this;
  }
  return replaceWithUndefinedInPlace();
}

}  // namespace Poincare
