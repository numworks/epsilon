#include <poincare/expression.h>
#include <poincare/layout.h>
#include <poincare/old/addition.h>
#include <poincare/old/dependency.h>
#include <poincare/old/list.h>
#include <poincare/old/list_complex.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/symbol_abstract.h>
#include <poincare/old/undefined.h>

namespace Poincare {

// ListNode

size_t ListNode::serialize(char* buffer, size_t bufferSize,
                           Preferences::PrintFloatMode floatDisplayMode,
                           int numberOfSignificantDigits) const {
  size_t writtenChars = SerializationHelper::CodePoint(buffer, bufferSize, '{');
  if (writtenChars >= bufferSize - 1) {
    return writtenChars;
  }
  if (m_numberOfChildren > 0) {
    writtenChars += SerializationHelper::Infix(
        this, buffer + writtenChars, bufferSize - writtenChars,
        floatDisplayMode, numberOfSignificantDigits, ",");
  }
  if (writtenChars >= bufferSize - 1) {
    return writtenChars;
  }
  writtenChars += SerializationHelper::CodePoint(
      buffer + writtenChars, bufferSize - writtenChars, '}');
  return writtenChars;
}

// Helper functions
int ListNode::extremumIndex(const ApproximationContext& approximationContext,
                            bool minimum) {
  int numberOfElements = numberOfChildren();
  float currentExtremumValue = NAN;
  int returnIndex = -1;
  for (int i = 0; i < numberOfElements; i++) {
    ExpressionNode* child = childAtIndex(i);
    float newValue =
        child->approximate(static_cast<float>(0), approximationContext)
            .toRealScalar();
    if (std::isnan(newValue)) {
      return -1;
    }
    assert(!std::isnan(currentExtremumValue) || returnIndex < 0);
    if (returnIndex < 0 || (minimum && newValue < currentExtremumValue) ||
        (!minimum && newValue > currentExtremumValue)) {
      returnIndex = i;
      currentExtremumValue = newValue;
    }
  }
  return returnIndex;
}

OExpression ListNode::shallowReduce(const ReductionContext& reductionContext) {
  return OList(this).shallowReduce(reductionContext);
}

// OList

OExpression OList::Ones(int length) {
  OList result = OList::Builder();
  for (int i = 0; i < length; i++) {
    result.addChildAtIndexInPlace(Rational::Builder(1), i, i);
  }
  return std::move(result);
}

OExpression OList::shallowReduce(ReductionContext reductionContext) {
  OExpression myParent = parent();
  bool isDependenciesList =
      !myParent.isUninitialized() &&
      myParent.otype() == ExpressionNode::Type::Dependency &&
      myParent.indexOfChild(*this) == Dependency::k_indexOfDependenciesList;

  // A list can't contain a matrix or a list
  if (!isDependenciesList &&
      node()->hasMatrixOrListChild(reductionContext.context())) {
    return replaceWithUndefinedInPlace();
  }

  /* We do not reduce a list to undef when a child is undef because undef and
   * {undef} are different objects. */
  SimplificationHelper::UndefReduction undefReduction =
      isDependenciesList
          ? SimplificationHelper::UndefReduction::BubbleUpUndef
          : SimplificationHelper::UndefReduction::DoNotBubbleUpUndef;

  OExpression e = SimplificationHelper::defaultShallowReduce(
      *this, &reductionContext,
      SimplificationHelper::BooleanReduction::DefinedOnBooleans,
      SimplificationHelper::UnitReduction::KeepUnits,
      SimplificationHelper::MatrixReduction::DefinedOnMatrix,
      SimplificationHelper::ListReduction::DoNotDistributeOverLists,
      SimplificationHelper::PointReduction::DefinedOnPoint, undefReduction);
  if (!e.isUninitialized()) {
    return e;
  }
  return *this;
}

OExpression OList::extremum(const ReductionContext& reductionContext,
                            bool minimum) {
  const ApproximationContext approximationContext(reductionContext, true);
  int extremumIndex = node()->extremumIndex(approximationContext, minimum);
  if (extremumIndex < 0) {
    return Undefined::Builder();
  }
  return childAtIndex(extremumIndex);
}
}  // namespace Poincare
