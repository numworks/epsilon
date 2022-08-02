#include <poincare/logical_operator.h>
#include <poincare/float.h>
#include <poincare/layout_helper.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <cmath>

namespace Poincare {

template<typename T>
bool LogicalOperatorNode::IsApproximativelyNotZero(T x) {
  return x != 0.0 && std::fabs(x) >= Float<T>::EpsilonLax();
}

// Not Operator

Layout NotOperatorNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(NotOperator(this), floatDisplayMode, numberOfSignificantDigits, k_name, context);
}

int NotOperatorNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, k_name);
}

template<typename T>
Evaluation<T> NotOperatorNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  // TODO: Map over lists
  T childToScalar = childAtIndex(0)->approximate(T(), approximationContext).toScalar();
  if (std::isnan(childToScalar)) {
    return Complex<T>::Builder(NAN);
  }
  return IsApproximativelyNotZero(childToScalar) ? Complex<T>::Builder(0.0) : Complex<T>::Builder(1.0);
}

Expression NotOperatorNode::shallowReduce(const ReductionContext& reductionContext) {
  return NotOperator(this).shallowReduce(reductionContext);
}

Expression NotOperator::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        reductionContext,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  // TODO: This behaviour is inconsistent with approximation.
  // 3kg is true here and undef in approximation. Same for 1+i.
  ExpressionNode::NullStatus childNullStatus = childAtIndex(0).nullStatus(reductionContext.context());
  if (childNullStatus == ExpressionNode::NullStatus::Unknown) {
    return *this;
  }
  Expression result = childNullStatus == ExpressionNode::NullStatus::NonNull ? Rational::Builder(0) : Rational::Builder(1);
  replaceWithInPlace(result);
  return result;
}


// Binary Logical Operator

bool BinaryLogicalOperatorNode::IsBinaryLogicalOperator(const char * name, int nameLength, OperatorType * type) {
  for (int i = 0; i < k_numberOfOperators; i++) {
    if (strncmp(name, k_operatorNames[i].name, nameLength + 1) == 0) {
      if (type) {
        *type = k_operatorNames[i].type;
      }
      return true;
    }
  }
  return false;
}

const char * BinaryLogicalOperatorNode::operatorName() const {
  assert(m_typeOfOperator != OperatorType::LengthOfEnum);
  for (int i = 0; i < k_numberOfOperators; i++) {
    if (k_operatorNames[i].type == m_typeOfOperator) {
      return k_operatorNames[i].name;
    }
  }
  assert(false);
  return k_operatorNames[0].name; // silence compiler
}

bool BinaryLogicalOperatorNode::evaluate(bool a, bool b) const {
  switch (m_typeOfOperator) {
  case OperatorType::And:
    return a && b;
  case OperatorType::Or:
    return a || b;
  case OperatorType::Xor:
    return a != b;
  case OperatorType::Nand:
    return !(a && b);
  case OperatorType::Nor:
    return !(a || b);
  default:
    assert(false);
    return false;
  }
}

Layout BinaryLogicalOperatorNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Infix(BinaryLogicalOperator(this), floatDisplayMode, numberOfSignificantDigits, operatorName(), context);
}

int BinaryLogicalOperatorNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, operatorName());
}

template<typename T>
Evaluation<T> BinaryLogicalOperatorNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  // TODO: Map over lists
  T firstChild = childAtIndex(0)->approximate(T(), approximationContext).toScalar();
  T secondChild = childAtIndex(1)->approximate(T(), approximationContext).toScalar();
  if (std::isnan(firstChild) || std::isnan(secondChild)) {
    return Complex<T>::Builder(NAN);
  }
  return evaluate(IsApproximativelyNotZero(firstChild), IsApproximativelyNotZero(secondChild)) ? Complex<T>::Builder(1.0) : Complex<T>::Builder(0.0);
}

Expression BinaryLogicalOperatorNode::shallowReduce(const ReductionContext& reductionContext) {
  return BinaryLogicalOperator(this).shallowReduce(reductionContext);
}

Expression BinaryLogicalOperator::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        reductionContext,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  // TODO: This behaviour is inconsistent with approximation.
  // 3kg is true here and undef in approximation. Same for 1+i.
  ExpressionNode::NullStatus firstChildNullStatus = childAtIndex(0).nullStatus(reductionContext.context());
  ExpressionNode::NullStatus secondChildNullStatus = childAtIndex(1).nullStatus(reductionContext.context());
  if (firstChildNullStatus == ExpressionNode::NullStatus::Unknown || secondChildNullStatus == ExpressionNode::NullStatus::Unknown) {
    return *this;
  }
  bool booleanResult = static_cast<BinaryLogicalOperatorNode *>(node())->evaluate(firstChildNullStatus == ExpressionNode::NullStatus::NonNull, secondChildNullStatus == ExpressionNode::NullStatus::NonNull);
  Expression result = booleanResult ? Rational::Builder(1) : Rational::Builder(0);
  replaceWithInPlace(result);
  return result;
}

BinaryLogicalOperator BinaryLogicalOperator::Builder(Expression firstChild, Expression secondChild, BinaryLogicalOperatorNode::OperatorType type) {
  TreeHandle h = TreeHandle::BuilderWithChildren(Initializer<BinaryLogicalOperatorNode>, sizeof(BinaryLogicalOperatorNode), {firstChild, secondChild});
  BinaryLogicalOperator expression = static_cast<BinaryLogicalOperator&>(h);
  static_cast<BinaryLogicalOperatorNode *>(expression.node())->setOperatorType(type);
  return expression;
}

template Evaluation<float> BinaryLogicalOperatorNode::templatedApproximate<float>(const ApproximationContext&) const;
template Evaluation<double> BinaryLogicalOperatorNode::templatedApproximate<double>(const ApproximationContext&) const;
template Evaluation<float> NotOperatorNode::templatedApproximate<float>(const ApproximationContext&) const;
template Evaluation<double> NotOperatorNode::templatedApproximate<double>(const ApproximationContext&) const;

}