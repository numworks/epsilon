#include "unit_list_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/unit_convert.h>

#include "../app.h"
#include "unit_comparison_helper.h"

using namespace Poincare;
using namespace Escher;
using namespace Shared;

namespace Calculation {

UnitListController::UnitListController(
    EditExpressionController *editExpressionController)
    : ExpressionsListController(editExpressionController, true),
      m_numberOfExpressionCells(0),
      m_numberOfBufferCells(0),
      m_referenceValues{nullptr, nullptr},
      m_SIValue(0.0) {}

int UnitListController::reusableCellCount(int type) const {
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

HighlightCell *UnitListController::reusableCell(int index, int type) {
  if (type == k_expressionCellType) {
    return ExpressionsListController::reusableCell(index, type);
  }
  assert(type == k_bufferCellType);
  return &m_bufferCells[index];
}

KDCoordinate UnitListController::nonMemoizedRowHeight(int row) {
  if (typeAtRow(row) == k_expressionCellType) {
    return ExpressionsListController::nonMemoizedRowHeight(row);
  }
  assert(typeAtRow(row) == k_bufferCellType);
  BufferCell tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, row);
}

void UnitListController::fillCellForRow(HighlightCell *cell, int row) {
  if (typeAtRow(row) == k_expressionCellType) {
    return ExpressionsListController::fillCellForRow(cell, row);
  }
  assert(typeAtRow(row) == k_bufferCellType);
  BufferCell *myCell = static_cast<BufferCell *>(cell);
  fillBufferCellAtIndex(myCell, row - m_numberOfExpressionCells);
  myCell->subLabel()->setMessage(messageAtIndex(row));
}

int UnitListController::numberOfRows() const {
  return m_numberOfExpressionCells + m_numberOfBufferCells;
}

void UnitListController::computeAdditionalResults(
    const Expression input, const Expression exactOutput,
    const Expression approximateOutput) {
  assert(AdditionalResultsType::HasUnit(exactOutput, m_calculationPreferences));
  Expression exactClone = exactOutput.clone();

  /* I. Handle expression cells
   *   0. Initialize expressions and layouts */
  Poincare::Expression expressions[k_maxNumberOfExpressionCells];
  for (size_t i = 0; i < k_maxNumberOfExpressionCells; i++) {
    m_layouts[i] = Layout();
    expressions[i] = Expression();
  }

  /*   1. First rows: miscellaneous classic units for some dimensions, in both
   *      metric and imperial units. */
  Expression copy = exactClone;
  Expression units;
  // Reduce to be able to recognize units
  Context *context = App::app()->localContext();
  PoincareHelpers::CloneAndReduceAndRemoveUnit(
      &copy, &units, context,
      {.complexFormat = complexFormat(), .angleUnit = angleUnit()});
  assert(!units.isUninitialized());
  double value = Shared::PoincareHelpers::ApproximateToScalar<double>(
      copy, context,
      {.complexFormat = complexFormat(), .angleUnit = angleUnit()});
  ReductionContext reductionContext =
      Shared::PoincareHelpers::ReductionContextForParameters(
          exactClone, context,
          {.complexFormat = complexFormat(),
           .angleUnit = angleUnit(),
           .symbolicComputation = SymbolicComputation::
               ReplaceAllSymbolsWithDefinitionsOrUndefined});
  int numberOfExpressions = Unit::SetAdditionalExpressions(
      units, value, expressions, k_maxNumberOfExpressionCells, reductionContext,
      exactClone);

  Expression siExpression;
  if (Unit::HasAngleDimension(units)) {
    // Needs to be defined for the unit comparison but is not used with angles
    siExpression = exactClone;
  } else {
    //  2. SI units only
    assert(numberOfExpressions < k_maxNumberOfExpressionCells - 1);
    expressions[numberOfExpressions] = exactClone;
    Shared::PoincareHelpers::CloneAndSimplify(
        &expressions[numberOfExpressions], context,
        {.complexFormat = complexFormat(),
         .angleUnit = angleUnit(),
         .unitConversion = Poincare::UnitConversion::InternationalSystem});
    siExpression =
        expressions[numberOfExpressions];  // Remember for later (part II)
    numberOfExpressions++;
  }

  /*  3. Get rid of duplicates
   * We find duplicates by comparing the serializations, to eliminate
   * expressions that only differ by the types of their number nodes. */
  Expression reduceExpression = exactClone;
  /* Make exactClone comparable to expressions (turn BasedInteger into
   * Rational for instance) */
  Shared::PoincareHelpers::CloneAndSimplify(
      &reduceExpression, context,
      {.complexFormat = complexFormat(),
       .angleUnit = angleUnit(),
       .unitConversion = Poincare::UnitConversion::None});
  int currentExpressionIndex = 0;
  while (currentExpressionIndex < numberOfExpressions) {
    bool duplicateFound = false;
    constexpr size_t buffersSize = Constant::MaxSerializedExpressionSize;
    char buffer1[buffersSize];
    size_t size1 = PoincareHelpers::Serialize(
        expressions[currentExpressionIndex], buffer1, buffersSize);
    for (int i = 0; i < currentExpressionIndex + 1; i++) {
      /* Compare the currentExpression to all previous expressions and to
       * exactClone */
      Expression comparedExpression =
          i == currentExpressionIndex ? reduceExpression : expressions[i];
      assert(!comparedExpression.isUninitialized());
      char buffer2[buffersSize];
      size_t size2 =
          PoincareHelpers::Serialize(comparedExpression, buffer2, buffersSize);
      if (size1 == size2 && strcmp(buffer1, buffer2) == 0) {
        numberOfExpressions--;
        // Shift next expressions
        for (int j = currentExpressionIndex; j < numberOfExpressions; j++) {
          expressions[j] = expressions[j + 1];
        }
        // Remove last expression
        expressions[numberOfExpressions] = Expression();
        /* The current expression has been discarded, no need to increment the
         * current index. */
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
      m_layouts[i] =
          Shared::PoincareHelpers::CreateLayout(expressions[i], context);
      // Radians may have two layouts to display
      if (expressions[i].isInRadians(context)) {
        Layout approximated = Shared::PoincareHelpers::CreateLayout(
            expressions[i].approximateKeepingUnits<double>(reductionContext),
            context);
        if (!approximated.isIdenticalTo(m_layouts[i], true)) {
          m_exactLayouts[i] = m_layouts[i];
          m_layouts[i] = Layout();
          m_approximatedLayouts[i] = approximated;
        }
      }
    }
  }

  /* II. Handle buffer cells
   *   0. Initialize reference values */
  for (size_t i = 0; i < k_maxNumberOfBufferCells; i++) {
    m_referenceValues[i] = nullptr;
  }

  //   1. Extract value and unit of SI expression
  assert(siExpression.hasUnit());
  Expression unit;
  PoincareHelpers::CloneAndReduceAndRemoveUnit(
      &siExpression, &unit, context,
      {.complexFormat = complexFormat(),
       .angleUnit = angleUnit(),
       .symbolicComputation =
           SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined,
       .unitConversion = UnitConversion::None});
  m_SIValue = PoincareHelpers::ApproximateToScalar<double>(
      siExpression, context,
      {.complexFormat = complexFormat(), .angleUnit = angleUnit()});
  //   2. Set upper and lower reference values
  m_numberOfBufferCells = UnitComparison::FindUpperAndLowerReferenceValues(
      m_SIValue, unit, m_referenceValues, &m_tableIndexForComparison);

  /* TODO:
   * If numberOfExpressions == 0, it might be because the SI reduction failed.
   * Maybe a generic solution could be found, like for example just diplaying
   * "undef" or "error" in the first row to indicate to the user that the
   * calculation failed. */
  assert(m_numberOfExpressionCells + m_numberOfBufferCells > 0);
}

I18n::Message UnitListController::messageAtIndex(int index) {
  if (typeAtRow(index) == k_bufferCellType) {
    assert(index - m_numberOfExpressionCells < m_numberOfBufferCells);
    return m_referenceValues[index - m_numberOfExpressionCells]->subtitle;
  }
  assert(index < m_numberOfExpressionCells);
  return (I18n::Message)0;
}

void UnitListController::fillBufferCellAtIndex(BufferCell *bufferCell,
                                               int index) {
  assert(index < m_numberOfBufferCells);
  const UnitComparison::ReferenceValue *referenceValue =
      m_referenceValues[index];
  assert(referenceValue != nullptr);
  I18n::Message messageInCell;
  char floatToTextBuffer[UnitComparison::k_sizeOfUnitComparisonBuffer];
  double ratio = m_SIValue / static_cast<double>(referenceValue->value);
  UnitComparison::FillRatioBuffer(ratio, floatToTextBuffer,
                                  UnitComparison::k_sizeOfUnitComparisonBuffer);
  if (ratio < UnitComparison::k_maxPercentageRatioDisplay) {
    messageInCell = referenceValue->title1;
  } else {
    messageInCell = referenceValue->title2;
  }
  bufferCell->label()->setMessageWithPlaceholders(messageInCell,
                                                  floatToTextBuffer);
}

size_t UnitListController::textAtIndex(char *buffer, size_t bufferSize,
                                       HighlightCell *cell, int index) {
  assert(index >= 0);
  if (index < m_numberOfExpressionCells) {
    return ExpressionsListController::textAtIndex(buffer, bufferSize, cell,
                                                  index);
  }
  index = index - m_numberOfExpressionCells;
  assert(index < m_numberOfBufferCells);
  return UnitComparison::BuildComparisonExpression(
             m_SIValue, m_referenceValues[index], m_tableIndexForComparison)
      .serialize(buffer, bufferSize,
                 Poincare::Preferences::PrintFloatMode::Decimal,
                 Poincare::Preferences::LargeNumberOfSignificantDigits);
}

}  // namespace Calculation
