#include <assert.h>
#include <omg/float.h>
#include <poincare/layout.h>
#include <poincare/old/boolean.h>
#include <poincare/old/comparison.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/subtraction.h>
#include <poincare/old/undefined.h>
#include <poincare/src/memory/tree.h>

namespace Poincare {

const char* ComparisonNode::ComparisonOperatorString(OperatorType type) {
  OperatorString result = k_operatorStrings[static_cast<int>(type)];
  assert(result.type == type);
  return result.mainString;
}

Layout ComparisonNode::ComparisonOperatorLayout(OperatorType type) {
  Layout result;
  assert(false);
#if 0
  const char* operatorString = ComparisonOperatorString(type);
  assert(result.otype() == LayoutNode::Type::CodePointLayout ||
         result.otype() == LayoutNode::Type::CombinedCodePointsLayout);
#endif
  return result;
}

bool ComparisonNode::IsComparisonOperatorString(const char* s,
                                                const char* stringEnd,
                                                OperatorType* returnType,
                                                size_t* returnLength) {
  int maxOperatorLength = stringEnd ? stringEnd - s : INT_MAX;
  int lengthOfFoundOperator = 0;
  OperatorType typeOfFoundOperator;
  for (int i = 0; i < static_cast<int>(OperatorType::NumberOfTypes); i++) {
    const char* currentOperatorString = k_operatorStrings[i].mainString;
    // Loop twice, once on the main string, the other on the alternative string
    for (int k = 0; k < 2; k++) {
      int operatorLength = strlen(currentOperatorString);
      if (operatorLength <= maxOperatorLength &&
          operatorLength > lengthOfFoundOperator &&
          strncmp(s, currentOperatorString, operatorLength) == 0) {
        lengthOfFoundOperator = operatorLength;
        typeOfFoundOperator = k_operatorStrings[i].type;
      }
      currentOperatorString = k_operatorStrings[i].alternativeString;
      if (!currentOperatorString) {
        break;
      }
    }
  }
  if (lengthOfFoundOperator == 0) {
    return false;
  }
  *returnLength = lengthOfFoundOperator;
  *returnType = typeOfFoundOperator;
  return true;
}

ComparisonNode::OperatorType ComparisonNode::SwitchInferiorSuperior(
    OperatorType type) {
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

bool ComparisonNode::IsBinaryComparison(Expression e,
                                        OperatorType* operatorType) {
  assert(!e.isUninitialized());
  if (!e.isComparison()) {
    return false;
  }
  if (operatorType) {
    assert(e.otype() == ExpressionNode::Type::Expression &&
           e.tree()->numberOfChildren() == 2);
    switch (e.tree()->type()) {
      case Internal::Type::Equal:
        *operatorType = OperatorType::Equal;
        break;
      case Internal::Type::NotEqual:
        *operatorType = OperatorType::NotEqual;
        break;
      case Internal::Type::Superior:
        *operatorType = OperatorType::Superior;
        break;
      case Internal::Type::SuperiorEqual:
        *operatorType = OperatorType::SuperiorEqual;
        break;
      case Internal::Type::Inferior:
        *operatorType = OperatorType::Inferior;
        break;
      case Internal::Type::InferiorEqual:
        *operatorType = OperatorType::InferiorEqual;
        break;
      default:
        assert(false);
    }
  }
  return true;
}

bool ComparisonNode::IsComparisonWithoutNotEqualOperator(OExpression e) {
  assert(!e.isUninitialized());
  if (e.otype() != Type::Comparison) {
    return false;
  }
  Comparison c = static_cast<Comparison&>(e);
  const int operatorCount = c.numberOfOperators();
  for (int i = 0; i < operatorCount; i++) {
    if (c.operatorAtIndex(i) == OperatorType::NotEqual) {
      return false;
    }
  }
  return true;
}

bool ComparisonNode::IsBinaryComparisonWithOperator(Expression e,
                                                    OperatorType operatorType) {
  OperatorType operatorTypeOfE;
  return IsBinaryComparison(e, &operatorTypeOfE) &&
         operatorTypeOfE == operatorType;
}

OMG::Troolean ComparisonNode::TruthValueOfOperator(
    OperatorType type, OMG::Troolean chidlrenAreEqual,
    OMG::Troolean leftChildIsGreater) {
  if (type == OperatorType::Equal || type == OperatorType::NotEqual) {
    return type == OperatorType::Equal ? chidlrenAreEqual
                                       : TrooleanNot(chidlrenAreEqual);
  }
  if (type == OperatorType::Inferior || type == OperatorType::InferiorEqual) {
    // Revert the symbol to handle only the Superior and SuperiorOrEqual cases
    type = SwitchInferiorSuperior(type);
    leftChildIsGreater = TrooleanNot(leftChildIsGreater);
  }
  if (type == OperatorType::Superior) {
    return TrooleanAnd(leftChildIsGreater, TrooleanNot(chidlrenAreEqual));
  }
  assert(type == OperatorType::SuperiorEqual);
  return TrooleanOr(leftChildIsGreater, chidlrenAreEqual);
}

ComparisonNode::ComparisonNode(int numberOfOperands,
                               OperatorType* operatorsListButTheLast,
                               OperatorType lastOperatorOfList)
    : ExpressionNode(), m_numberOfOperands(numberOfOperands) {
  assert(m_numberOfOperands >= 2);
  if (operatorsListButTheLast) {
    memcpy(m_operatorsList, operatorsListButTheLast,
           sizeof(OperatorType) * (numberOfOperators() - 1));
  }
  m_operatorsList[numberOfOperators() - 1] = lastOperatorOfList;
}

static size_t SizeOfComparisonNodeWithOperators(int numberOfOperators) {
  assert(numberOfOperators > 0);
  return sizeof(ComparisonNode) +
         sizeof(ComparisonNode::OperatorType) * numberOfOperators;
}

size_t ComparisonNode::size() const {
  return SizeOfComparisonNodeWithOperators(numberOfOperators());
}

#if POINCARE_TREE_LOG
void ComparisonNode::logAttributes(std::ostream& stream) const {
  stream << " operators=\"";
  int nOperators = numberOfOperators();
  for (int i = 0; i < nOperators; i++) {
    stream << " " << ComparisonOperatorString(m_operatorsList[i]);
  }
  stream << " \"";
}
#endif

size_t ComparisonNode::serialize(char* buffer, size_t bufferSize,
                                 Preferences::PrintFloatMode floatDisplayMode,
                                 int numberOfSignificantDigits) const {
  size_t numberOfChar = 0;
  for (int i = 0; i < m_numberOfOperands; i++) {
    if (i > 0) {
      // Write the operator
      numberOfChar += strlcpy(buffer + numberOfChar,
                              ComparisonOperatorString(m_operatorsList[i - 1]),
                              bufferSize - numberOfChar);
      if (numberOfChar >= bufferSize - 1) {
        assert(buffer[bufferSize - 1] == 0);
        return bufferSize - 1;
      }
    }
    numberOfChar += SerializationHelper::SerializeChild(
        childAtIndex(i), this, buffer + numberOfChar, bufferSize - numberOfChar,
        floatDisplayMode, numberOfSignificantDigits);
    if (numberOfChar >= bufferSize - 1) {
      assert(buffer[bufferSize - 1] == 0);
      return bufferSize - 1;
    }
  }
  return numberOfChar;
}

bool ComparisonNode::childNeedsSystemParenthesesAtSerialization(
    const PoolObject* child) const {
  int i = indexOfChild(child);
  OExpression e = Comparison(this).childAtIndex(i);
  // Factorials can mess up with equal signs
  return e.recursivelyMatches(OExpression::IsFactorial, nullptr);
}

Evaluation<float> ComparisonNode::approximate(
    SinglePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<float>(approximationContext);
}

Evaluation<double> ComparisonNode::approximate(
    DoublePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<double>(approximationContext);
}

template <typename T>
Evaluation<T> ComparisonNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  Evaluation<T> firstChildApprox;
  Evaluation<T> secondChildApprox =
      childAtIndex(0)->approximate(T(), approximationContext);
  for (int i = 1; i < m_numberOfOperands; i++) {
    firstChildApprox = secondChildApprox;
    secondChildApprox = childAtIndex(i)->approximate(T(), approximationContext);
    if (firstChildApprox.otype() != EvaluationNode<T>::Type::Complex ||
        secondChildApprox.otype() != EvaluationNode<T>::Type::Complex) {
      return Complex<T>::Undefined();
    }
    std::complex<T> difference = firstChildApprox.complexAtIndex(0) -
                                 secondChildApprox.complexAtIndex(0);
    T scalarDifference = ComplexNode<T>::ToRealScalar(difference);
    T epsilon = std::max(std::fabs(firstChildApprox.toRealScalar()),
                         std::fabs(secondChildApprox.toRealScalar())) *
                OMG::Float::Epsilon<T>();
    OMG::Troolean chidlrenAreEqual;
    OMG::Troolean leftChildIsGreater;
    if (std::isnan(scalarDifference)) {
      leftChildIsGreater = OMG::Troolean::Unknown;
      chidlrenAreEqual =
          (firstChildApprox.isUndefined() || secondChildApprox.isUndefined())
              ? OMG::Troolean::Unknown
              : OMG::Troolean::False;
    } else {
      /* leftChildIsGreater is always used in combination with childrenAreEqual
       * so the fact that it's strictly greater or not is not important. */
      leftChildIsGreater = OMG::BoolToTroolean(scalarDifference > 0.0);
      chidlrenAreEqual =
          OMG::BoolToTroolean(std::isfinite(scalarDifference) &&
                              std::fabs(scalarDifference) <= epsilon);
    }
    OMG::Troolean truthValue = TruthValueOfOperator(
        m_operatorsList[i - 1], chidlrenAreEqual, leftChildIsGreater);
    switch (truthValue) {
      case OMG::Troolean::False:
        return BooleanEvaluation<T>::Builder(false);
      case OMG::Troolean::Unknown:
        return Complex<T>::Undefined();
      default:
        assert(truthValue == OMG::Troolean::True);
    }
  }
  return BooleanEvaluation<T>::Builder(true);
}

OExpression ComparisonNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Comparison(this).shallowReduce(reductionContext);
}

Comparison Comparison::Builder(OExpression child0,
                               ComparisonNode::OperatorType operatorType,
                               OExpression child1) {
  void* bufferNode =
      Pool::sharedPool->alloc(SizeOfComparisonNodeWithOperators(1));
  ComparisonNode* node = new (bufferNode) ComparisonNode(operatorType);
  PoolHandle h = PoolHandle::BuildWithGhostChildren(node);
  h.replaceChildAtIndexInPlace(0, child0);
  h.replaceChildAtIndexInPlace(1, child1);
  return static_cast<Comparison&>(h);
}

Comparison Comparison::addComparison(ComparisonNode::OperatorType operatorType,
                                     OExpression child) {
  int numberOfOperands = numberOfChildren() + 1;
  void* bufferNode = Pool::sharedPool->alloc(
      SizeOfComparisonNodeWithOperators(numberOfOperands - 1));
  ComparisonNode::OperatorType* listOfOperators = node()->listOfOperators();
  ComparisonNode* node = new (bufferNode)
      ComparisonNode(numberOfOperands, listOfOperators, operatorType);
  PoolHandle h = PoolHandle::BuildWithGhostChildren(node);
  for (int i = 0; i < numberOfOperands - 1; i++) {
    h.replaceChildAtIndexInPlace(i, childAtIndex(i));
  }
  h.replaceChildAtIndexInPlace(numberOfOperands - 1, child);
  replaceWithInPlace(h);
  return static_cast<Comparison&>(h);
}

OExpression Comparison::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::KeepUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  OExpression firstChild;
  OExpression secondChild = childAtIndex(0);
  const int numberOfOperands = numberOfChildren();
  for (int i = 1; i < numberOfOperands; i++) {
    firstChild = secondChild;
    secondChild = childAtIndex(i);
    OExpression difference =
        Subtraction::Builder(firstChild.clone(), secondChild.clone());
    difference = difference.shallowReduce(reductionContext);
    OMG::Troolean childrenAreEqual =
        difference.isNull(reductionContext.context());
    OMG::Troolean leftIsGreater =
        difference.isPositive(reductionContext.context());
    OMG::Troolean comparison = ComparisonNode::TruthValueOfOperator(
        node()->operatorAtIndex(i - 1), childrenAreEqual, leftIsGreater);
    if (comparison == OMG::Troolean::Unknown) {
      return *this;  // Let approximation decide
    }
    if (comparison == OMG::Troolean::False) {
      OExpression result = OBoolean::Builder(false);
      replaceWithInPlace(result);
      return result;
    }
  }
  OExpression result = OBoolean::Builder(true);
  replaceWithInPlace(result);
  return result;
}

OExpression Comparison::cloneWithStrictOrLenientOperators(bool strict) const {
  // operators[strict] is strict if strict == 1 and lenient if strict == 0
  constexpr ComparisonNode::OperatorType inferiorOperators[] = {
      ComparisonNode::OperatorType::InferiorEqual,
      ComparisonNode::OperatorType::Inferior};
  constexpr ComparisonNode::OperatorType superiorOperators[] = {
      ComparisonNode::OperatorType::SuperiorEqual,
      ComparisonNode::OperatorType::Superior};
  int n = numberOfOperators();
  OExpression result = clone();
  ComparisonNode::OperatorType* operatorsList =
      static_cast<Comparison&>(result).node()->listOfOperators();
  for (int i = 0; i < n; i++) {
    if (strict && operatorsList[i] == ComparisonNode::OperatorType::Equal) {
      return Undefined::Builder();
    } else if (!strict &&
               operatorsList[i] == ComparisonNode::OperatorType::NotEqual) {
      operatorsList[i] = ComparisonNode::OperatorType::Equal;
    } else if (operatorsList[i] ==
               inferiorOperators[static_cast<int>(!strict)]) {
      operatorsList[i] = inferiorOperators[static_cast<int>(strict)];
    } else if (operatorsList[i] ==
               superiorOperators[static_cast<int>(!strict)]) {
      operatorsList[i] = superiorOperators[static_cast<int>(strict)];
    }
  }
  return result;
}

}  // namespace Poincare
