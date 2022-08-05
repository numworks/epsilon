#include <poincare/comparison.h>
#include <poincare/code_point_layout.h>
#include <poincare/float.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/subtraction.h>
#include <ion/unicode/utf8_decoder.h>
#include <assert.h>

namespace Poincare {

CodePoint ComparisonNode::ComparisonCodePoint(OperatorType type) {
  switch (type) {
  case OperatorType::Equal:
    return '=';
  case OperatorType::NotEqual:
    return UCodePointNotEqual;
  case OperatorType::Superior:
    return '>';
  case OperatorType::Inferior:
    return '<';
  case OperatorType::SuperiorEqual:
    return UCodePointSuperiorEqual;
  default:
    assert(type == OperatorType::InferiorEqual);
    return UCodePointInferiorEqual;
  }
}

bool ComparisonNode::IsComparisonOperatorCodePoint(CodePoint c, OperatorType * returnType) {
  for (int t = 0; t < static_cast<int>(OperatorType::NumberOfTypes); t++) {
    OperatorType currentType = static_cast<OperatorType>(t);
    if (c == ComparisonCodePoint(currentType)) {
      if (returnType) {
        *returnType = currentType;
      }
      return true;
    }
  }
  return false;
}

ComparisonNode::OperatorType ComparisonNode::Opposite(OperatorType type) {
  switch (type) {
  case OperatorType::Superior:
    return OperatorType::Inferior;
  case OperatorType::SuperiorEqual:
    return OperatorType::InferiorEqual;
  case OperatorType::Inferior:
    return OperatorType::Superior;
  default:
    assert(type == OperatorType::InferiorEqual);
    return OperatorType::SuperiorEqual;
  }
}

ComparisonNode::TrinaryBoolean ComparisonNode::TrinaryTruthValue(OperatorType type, TrinaryBoolean chidlrenAreEqual, TrinaryBoolean leftChildIsGreater) {
  if (chidlrenAreEqual == TrinaryBoolean::Unknown) {
    return TrinaryBoolean::Unknown;
  }
  if (chidlrenAreEqual == TrinaryBoolean::True) {
    bool result = (type == OperatorType::SuperiorEqual || type == OperatorType::InferiorEqual || type == OperatorType::Equal);
    return result ? TrinaryBoolean::True : TrinaryBoolean::False;
  }
  assert(chidlrenAreEqual == TrinaryBoolean::False);
  if (type == OperatorType::Equal) {
    return TrinaryBoolean::False;
  }
  if (type == OperatorType::NotEqual) {
    return TrinaryBoolean::True;
  }
  if (leftChildIsGreater == TrinaryBoolean::Unknown) {
    return TrinaryBoolean::Unknown;
  }
  bool leftIsGreater = leftChildIsGreater == TrinaryBoolean::True;
  bool operatorIsSuperior = (type == OperatorType::SuperiorEqual || type == OperatorType::Superior);
  return (operatorIsSuperior == leftIsGreater) ? TrinaryBoolean::True : TrinaryBoolean::False;
}

ComparisonNode::ComparisonNode(int numberOfOperands, OperatorType lastOperatorOfList, OperatorType * otherOperatorsList) :
  ExpressionNode(),
  m_numberOfOperands(numberOfOperands) {
  assert(m_numberOfOperands >= 2);
  if (otherOperatorsList) {
    memcpy(m_operatorsList, otherOperatorsList, sizeof(OperatorType) * (numberOfOperators() - 1));
  }
  m_operatorsList[numberOfOperators() - 1] = lastOperatorOfList;
}

static size_t SizeOfComparisonNode(int numberOfOperators) {
  assert(numberOfOperators > 0);
  return sizeof(ComparisonNode) + sizeof(ComparisonNode::OperatorType) * numberOfOperators;
}

size_t ComparisonNode::size() const {
  return SizeOfComparisonNode(numberOfOperators());
}

#if POINCARE_TREE_LOG
void ComparisonNode::logAttributes(std::ostream & stream) const {
  stream << " operators=\"";
  int nOperators = numberOfOperators();
  constexpr static size_t maxLengthOfOperator = UTF8Decoder::CharSizeOfCodePoint(UCodePointInferiorEqual);
  char buffer[maxLengthOfOperator + 1];
  for(int i = 0; i < nOperators; i++ ) {
    CodePoint c = ComparisonCodePoint(m_operatorsList[i]);
    UTF8Decoder::CodePointToChars(c, buffer, maxLengthOfOperator);
    buffer[UTF8Decoder::CharSizeOfCodePoint(c)] = 0;
    stream << " " << buffer;
  }
  stream << " \"";
}
#endif

Layout ComparisonNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  HorizontalLayout result = HorizontalLayout::Builder();
  for (int i = 0; i < m_numberOfOperands - 1; i++) {
    result.addOrMergeChildAtIndex(childAtIndex(i)->createLayout(floatDisplayMode, numberOfSignificantDigits, context), result.numberOfChildren(), false);
    Layout operatorLayout = CodePointLayout::Builder(ComparisonCodePoint(m_operatorsList[i]));
    operatorLayout.setMargin(true);
    result.addChildAtIndex(operatorLayout, result.numberOfChildren(), result.numberOfChildren(), nullptr);
  }
  Layout lastChildLayout = childAtIndex(m_numberOfOperands - 1)->createLayout(floatDisplayMode, numberOfSignificantDigits, context);
  lastChildLayout.setMargin(true);
  result.addOrMergeChildAtIndex(lastChildLayout, result.numberOfChildren(), false);
  return std::move(result);
}

int ComparisonNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  int numberOfChar = 0;
  for (int i = 0; i < m_numberOfOperands; i++) {
    if (i > 0) {
      // Write the operator
      numberOfChar += SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize - numberOfChar, ComparisonCodePoint(m_operatorsList[i - 1]));
      if (numberOfChar >= bufferSize-1) {
        assert(buffer[bufferSize - 1] == 0);
        return bufferSize - 1;
      }
    }
    numberOfChar += SerializationHelper::SerializeChild(childAtIndex(i), this, buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode, numberOfSignificantDigits);
    if (numberOfChar >= bufferSize-1) {
      assert(buffer[bufferSize - 1] == 0);
      return bufferSize - 1;
    }
  }
  return numberOfChar;
}

Evaluation<float> ComparisonNode::approximate(SinglePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<float>(approximationContext);
}

Evaluation<double> ComparisonNode::approximate(DoublePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<double>(approximationContext);
}

template<typename T>
Evaluation<T> ComparisonNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  Evaluation<T> firstChildApprox;
  Evaluation<T> secondChildApprox = childAtIndex(0)->approximate(T(), approximationContext);
  for (int i = 1; i < m_numberOfOperands; i++) {
    firstChildApprox = secondChildApprox;
    secondChildApprox = childAtIndex(i)->approximate(T(), approximationContext);
    if (firstChildApprox.type() != EvaluationNode<T>::Type::Complex || secondChildApprox.type() != EvaluationNode<T>::Type::Complex) {
      return Complex<T>::Undefined();
    }
    std::complex<T> difference = firstChildApprox.complexAtIndex(0) - secondChildApprox.complexAtIndex(0);
    T scalarDifference = ComplexNode<T>::ToScalar(difference);
    TrinaryBoolean chidlrenAreEqual;
    TrinaryBoolean leftChildIsGreater;
    if (std::isnan(scalarDifference)) {
      leftChildIsGreater = TrinaryBoolean::Unknown;
      chidlrenAreEqual = (firstChildApprox.isUndefined() || secondChildApprox.isUndefined()) ? TrinaryBoolean::Unknown : TrinaryBoolean::False;
    } else {
      leftChildIsGreater = scalarDifference >= 0.0 ? TrinaryBoolean::True : TrinaryBoolean::False;
      chidlrenAreEqual = (std::fabs(scalarDifference) < Float<T>::EpsilonLax()) ? TrinaryBoolean::True : TrinaryBoolean::False;
    }
    TrinaryBoolean truthValue = TrinaryTruthValue(m_operatorsList[i - 1], chidlrenAreEqual, leftChildIsGreater);
    switch (truthValue) {
    case TrinaryBoolean::False:
      return Complex<T>::Builder(0.0);
    case TrinaryBoolean::Unknown:
      return Complex<T>::Undefined();
    default:
      assert(truthValue == TrinaryBoolean::True);
    }
  }
  return Complex<T>::Builder(1.0);
}

Expression ComparisonNode::shallowReduce(const ReductionContext& reductionContext) {
  return Comparison(this).shallowReduce(reductionContext);
}

Comparison Comparison::Builder(Expression child0, ComparisonNode::OperatorType operatorType, Expression child1) {
  void * bufferNode = TreePool::sharedPool()->alloc(SizeOfComparisonNode(1));
  ComparisonNode * node = new (bufferNode) ComparisonNode(2, operatorType);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  h.replaceChildAtIndexInPlace(0, child0);
  h.replaceChildAtIndexInPlace(1, child1);
  return static_cast<Comparison &>(h);
}

Comparison Comparison::addComparison(ComparisonNode::OperatorType operatorType, Expression child) {
  int numberOfOperands = numberOfChildren() + 1;
  void * bufferNode = TreePool::sharedPool()->alloc(SizeOfComparisonNode(numberOfOperands - 1));
  ComparisonNode::OperatorType * listOfOperators = node()->listOfOperators();
  ComparisonNode * node = new (bufferNode) ComparisonNode(numberOfOperands, operatorType, listOfOperators);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  for (int i = 0; i < numberOfOperands - 1; i++) {
    h.replaceChildAtIndexInPlace(i, childAtIndex(i));
  }
  h.replaceChildAtIndexInPlace(numberOfOperands - 1, child);
  replaceWithInPlace(h);
  return static_cast<Comparison &>(h);
}

Expression Comparison::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        reductionContext,
        SimplificationHelper::UnitReduction::KeepUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression firstChild;
  Expression secondChild = childAtIndex(0);
  int numberOfOperands = numberOfChildren();
  for (int i = 1; i < numberOfOperands; i++) {
    firstChild = secondChild;
    secondChild = childAtIndex(i);
    Expression difference = Subtraction::Builder(firstChild.clone(), secondChild.clone());
    difference = difference.shallowReduce(reductionContext);
    ExpressionNode::NullStatus differenceNullStatus = difference.nullStatus(reductionContext.context());
    ExpressionNode::Sign differenceSign = difference.sign(reductionContext.context());

    ComparisonNode::TrinaryBoolean childrenAreEqual =
      differenceNullStatus == ExpressionNode::NullStatus::Unknown ?
        ComparisonNode::TrinaryBoolean::Unknown :
        (differenceNullStatus == ExpressionNode::NullStatus::Null ?
          ComparisonNode::TrinaryBoolean::True :
          ComparisonNode::TrinaryBoolean::False);
    ComparisonNode::TrinaryBoolean leftIsGreater =
      differenceSign == ExpressionNode::Sign::Unknown ?
        ComparisonNode::TrinaryBoolean::Unknown :
        (differenceSign == ExpressionNode::Sign::Positive ?
          ComparisonNode::TrinaryBoolean::True :
          ComparisonNode::TrinaryBoolean::False);
    ComparisonNode::TrinaryBoolean comparison = ComparisonNode::TrinaryTruthValue(node()->operatorAtIndex(i - 1), childrenAreEqual, leftIsGreater);
    if (comparison == ComparisonNode::TrinaryBoolean::Unknown) {
      return *this; // Let approximation decide
    }
    if (comparison == ComparisonNode::TrinaryBoolean::False) {
      Expression result = Rational::Builder(0);
      replaceWithInPlace(result);
      return result;
    }
  }
  Expression result = Rational::Builder(1);
  replaceWithInPlace(result);
  return result;
}

}