#include "unit_list_controller.h"
#include "unit_comparison_helper.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/unit_convert.h>

using namespace Poincare;
using namespace Escher;
using namespace Shared;

namespace Calculation {

UnitListController::UnitListController(EditExpressionController * editExpressionController) :
  ExpressionsListController(editExpressionController, true),
  m_numberOfExpressionCells(0),
  m_numberOfBufferCells(0),
  m_referenceValues{nullptr, nullptr},
  m_SIValue(0.0)
{
}

int UnitListController::reusableCellCount(int type) {
  if (type == k_expressionCellType) {
    return m_numberOfExpressionCells;
  }
  assert(type == k_bufferCellType);
  return m_numberOfBufferCells;
}

void UnitListController::viewDidDisappear() {
  ExpressionsListController::viewDidDisappear();
  for (int i = 0; i < k_maxNumberOfBufferCells; i++) {
    m_referenceValues[i] = nullptr;
  }
  m_numberOfExpressionCells = 0;
  m_numberOfBufferCells = 0;
}

HighlightCell * UnitListController::reusableCell(int index, int type) {
  if (type == k_expressionCellType) {
    return ExpressionsListController::reusableCell(index, type);
  }
  assert(type == k_bufferCellType);
  return &m_bufferCells[index];
}

KDCoordinate UnitListController::nonMemoizedRowHeight(int index) {
  if (typeAtIndex(index) == k_expressionCellType) {
    return ExpressionsListController::nonMemoizedRowHeight(index);
  }
  BufferTableCellWithMessage tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, index);
}

void UnitListController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  cell->setHighlighted(false);
  if (typeAtIndex(index) == k_expressionCellType) {
    return ExpressionsListController::willDisplayCellForIndex(cell, index);
  }
  BufferTableCellWithMessage * myCell = static_cast<BufferTableCellWithMessage *>(cell);
  fillBufferCellAtIndex(myCell, index - m_numberOfExpressionCells);
  myCell->setSubLabelMessage(messageAtIndex(index));
}

int UnitListController::numberOfRows() const {
  return m_numberOfExpressionCells + m_numberOfBufferCells;
}

void UnitListController::setExpression(Poincare::Expression e) {
  resetMemoization();
  m_expression = e;

  // I. Handle expression cells
  // 0. Initialize expressions and layouts
  Poincare::Expression expressions[k_maxNumberOfExpressionCells];
  for (size_t i = 0; i < k_maxNumberOfExpressionCells; i++) {
    m_layouts[i] = Layout();
    expressions[i] = Expression();
  }

  /* 1. First rows: miscellaneous classic units for some dimensions, in both
   * metric and imperial units. */
  Expression copy = m_expression.clone();
  Expression units;
  // Reduce to be able to recognize units
  PoincareHelpers::ReduceAndRemoveUnit(&copy, App::app()->localContext(), ExpressionNode::ReductionTarget::User, &units);
  double value = Shared::PoincareHelpers::ApproximateToScalar<double>(copy, App::app()->localContext());
  ExpressionNode::ReductionContext reductionContext(
      App::app()->localContext(),
      Preferences::sharedPreferences()->complexFormat(),
      Preferences::sharedPreferences()->angleUnit(),
      GlobalPreferences::sharedGlobalPreferences()->unitFormat(),
      ExpressionNode::ReductionTarget::User,
      ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
  int numberOfExpressions = Unit::SetAdditionalExpressions(units, value, expressions, k_maxNumberOfExpressionCells, reductionContext, m_expression);

  Expression siExpression;
  const Unit::Representative * representative = units.type() == ExpressionNode::Type::Unit ? static_cast<Unit &>(units).representative() : UnitNode::Representative::RepresentativeForDimension(UnitNode::Vector<int>::FromBaseUnits(units));
  if (representative && representative->dimensionVector() == Unit::AngleRepresentative::Default().dimensionVector()) {
    // Needs to be defined for the unit comparison but is not used with angles
    siExpression = m_expression;
  } else {
    // 2. SI units only
    assert(numberOfExpressions < k_maxNumberOfExpressionCells - 1);
    expressions[numberOfExpressions] = m_expression;
    Shared::PoincareHelpers::CloneAndSimplify(&expressions[numberOfExpressions], App::app()->localContext(), ExpressionNode::ReductionTarget::User, Poincare::ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, Poincare::ExpressionNode::UnitConversion::InternationalSystem);
    siExpression = expressions[numberOfExpressions]; // Remember for later (part II)
    numberOfExpressions++;
  }

  /* 3. Get rid of duplicates
   * We find duplicates by comparing the serializations, to eliminate
   * expressions that only differ by the types of their number nodes. */
  Expression reduceExpression = m_expression;
  // Make m_expression comparable to expressions (turn BasedInteger into Rational for instance)
  Shared::PoincareHelpers::CloneAndSimplify(&reduceExpression, App::app()->localContext(), ExpressionNode::ReductionTarget::User, Poincare::ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, Poincare::ExpressionNode::UnitConversion::None);
  int currentExpressionIndex = 0;
  while (currentExpressionIndex < numberOfExpressions) {
    bool duplicateFound = false;
    constexpr int buffersSize = Constant::MaxSerializedExpressionSize;
    char buffer1[buffersSize];
    int size1 = PoincareHelpers::Serialize(expressions[currentExpressionIndex], buffer1, buffersSize);
    for (int i = 0; i < currentExpressionIndex + 1; i++) {
      // Compare the currentExpression to all previous expressions and to m_expression
      Expression comparedExpression = i == currentExpressionIndex ? reduceExpression : expressions[i];
      assert(!comparedExpression.isUninitialized());
      char buffer2[buffersSize];
      int size2 = PoincareHelpers::Serialize(comparedExpression, buffer2, buffersSize);
      if (size1 == size2 && strcmp(buffer1, buffer2) == 0) {
        numberOfExpressions--;
        // Shift next expressions
        for (int j = currentExpressionIndex; j < numberOfExpressions; j++) {
          expressions[j] = expressions[j+1];
        }
        // Remove last expression
        expressions[numberOfExpressions] = Expression();
        // The current expression has been discarded, no need to increment the current index
        duplicateFound = true;
        break;
      }
    }
    if (!duplicateFound) {
      // The current expression is not a duplicate, check next expression
      currentExpressionIndex++;
    }
  }

  // Memoize number of expression cells
  m_numberOfExpressionCells = numberOfExpressions;

  // Memoize layouts
  for (size_t i = 0; i < k_maxNumberOfExpressionCells; i++) {
    if (!expressions[i].isUninitialized()) {
      m_layouts[i] = Shared::PoincareHelpers::CreateLayout(expressions[i], App::app()->localContext());
      // Radians may have two layouts to display
      if (expressions[i].isInRadians(App::app()->localContext())) {
        Layout approximated = Shared::PoincareHelpers::CreateLayout(expressions[i].approximateKeepingUnits<double>(reductionContext), App::app()->localContext());
        if (!approximated.isIdenticalTo(m_layouts[i], true)) {
          m_exactLayouts[i] = m_layouts[i];
          m_layouts[i] = Layout();
          m_approximatedLayouts[i] = approximated;
        }
      }
    }
  }

  // II. Handle buffer cells
  // 0. Initialize reference values
  for (size_t i = 0; i < k_maxNumberOfBufferCells; i++) {
    m_referenceValues[i] = nullptr;
  }

  // 1. Extract value and unit of SI expression
  assert(siExpression.hasUnit());
  Expression clone = siExpression.clone();
  Expression unit;
  PoincareHelpers::ReduceAndRemoveUnit(&clone, App::app()->localContext(), ExpressionNode::ReductionTarget::User, &unit, ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined, ExpressionNode::UnitConversion::None);
  m_SIValue = PoincareHelpers::ApproximateToScalar<double>(clone, App::app()->localContext());
  // 2. Set upper and lower reference values
  m_numberOfBufferCells = UnitComparison::FindUpperAndLowerReferenceValues(m_SIValue, unit, m_referenceValues, &m_tableIndexForComparison);

  }

I18n::Message UnitListController::messageAtIndex(int index) {
  if (typeAtIndex(index) == k_bufferCellType) {
    assert(index - m_numberOfExpressionCells < m_numberOfBufferCells);
    return m_referenceValues[index - m_numberOfExpressionCells]->subtitle;
  }
  assert(index < m_numberOfExpressionCells);
  return (I18n::Message)0;
}

void UnitListController::fillBufferCellAtIndex(Escher::BufferTableCellWithMessage * bufferCell, int index) {
  assert(index < m_numberOfBufferCells);
  const UnitComparison::ReferenceValue * referenceValue = m_referenceValues[index];
  assert(referenceValue != nullptr);
  I18n::Message messageInCell;
  char floatToTextBuffer[UnitComparison::k_sizeOfUnitComparisonBuffer];
  double ratio = m_SIValue / static_cast<double>(referenceValue->value);
  UnitComparison::FillRatioBuffer(ratio, floatToTextBuffer, UnitComparison::k_sizeOfUnitComparisonBuffer);
  if (ratio < UnitComparison::k_maxPercentageRatioDisplay) {
    messageInCell = referenceValue->title1;
  } else {
    messageInCell = referenceValue->title2;
  }
  bufferCell->setMessageWithPlaceholders(messageInCell, floatToTextBuffer);
}

int UnitListController::textAtIndex(char * buffer, size_t bufferSize, int index) {
  assert(index >= 0);
  if (index < m_numberOfExpressionCells) {
    return ExpressionsListController::textAtIndex(buffer, bufferSize, index);
  }
  index = index - m_numberOfExpressionCells;
  assert(index < m_numberOfBufferCells);
  return UnitComparison::BuildComparisonExpression(m_SIValue, m_referenceValues[index], m_tableIndexForComparison).serialize(buffer, bufferSize, Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::LargeNumberOfSignificantDigits);
}

}
