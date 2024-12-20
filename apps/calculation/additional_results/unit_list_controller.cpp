#include "unit_list_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/src/expression/projection.h>

#include "../app.h"
#include "unit_comparison_helper.h"

using namespace Poincare;
using namespace Escher;
using namespace Shared;

namespace Calculation {

UnitListController::UnitListController(
    EditExpressionController* editExpressionController)
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

HighlightCell* UnitListController::reusableCell(int index, int type) {
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

void UnitListController::fillCellForRow(HighlightCell* cell, int row) {
  if (typeAtRow(row) == k_expressionCellType) {
    return ExpressionsListController::fillCellForRow(cell, row);
  }
  assert(typeAtRow(row) == k_bufferCellType);
  BufferCell* myCell = static_cast<BufferCell*>(cell);
  fillBufferCellAtIndex(myCell, row - m_numberOfExpressionCells);
  myCell->subLabel()->setMessage(messageAtIndex(row));
}

int UnitListController::numberOfRows() const {
  return m_numberOfExpressionCells + m_numberOfBufferCells;
}

void UnitListController::computeAdditionalResults(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  assert(AdditionalResultsType::HasUnit(exactOutput, m_calculationPreferences));
  Preferences::UnitFormat unitFormat =
      GlobalPreferences::SharedGlobalPreferences()->unitFormat();
  Poincare::Context* context = App::app()->localContext();
  Internal::ProjectionContext ctx = {
      .m_complexFormat = complexFormat(),
      .m_angleUnit = angleUnit(),
      .m_unitFormat = unitFormat,
      .m_symbolic = SymbolicComputation::ReplaceAllSymbols,
      .m_context = context,
  };

  // Initialize expressions and layouts
  Poincare::UserExpression expressions[k_maxNumberOfExpressionCells];
  for (size_t i = 0; i < k_maxNumberOfExpressionCells; i++) {
    m_layouts[i] = Layout();
    expressions[i] = Expression();
  }

  // Build an expression for each relevant unit display mode
  int numberOfExpressions = 0;
  ctx.m_unitDisplay = (unitFormat == Preferences::UnitFormat::Metric)
                          ? Internal::UnitDisplay::AutomaticMetric
                          : Internal::UnitDisplay::AutomaticImperial;
  expressions[numberOfExpressions++] = input.cloneAndSimplify(&ctx);
  assert(!expressions[numberOfExpressions - 1].isUninitialized());
#if 0  // TODO: correctly implement AutomaticInput and reenable it
  ctx.m_unitDisplay = Internal::UnitDisplay::AutomaticInput;
  expressions[numberOfExpressions++] = input.cloneAndSimplify(&ctx);
  assert(!expressions[numberOfExpressions - 1].isUninitialized());
#endif
  ctx.m_unitDisplay = Internal::UnitDisplay::Decomposition;
  expressions[numberOfExpressions++] = input.cloneAndSimplify(&ctx);
  assert(!expressions[numberOfExpressions - 1].isUninitialized());
  ctx.m_unitDisplay = Internal::UnitDisplay::Equivalent;
  expressions[numberOfExpressions++] = input.cloneAndSimplify(&ctx);
  assert(!expressions[numberOfExpressions - 1].isUninitialized());

  if (unitFormat != Preferences::UnitFormat::Metric) {
    ctx.m_unitDisplay = Internal::UnitDisplay::AutomaticMetric;
    expressions[numberOfExpressions++] = input.cloneAndSimplify(&ctx);
    assert(!expressions[numberOfExpressions - 1].isUninitialized());
  }
  ctx.m_unitDisplay = Internal::UnitDisplay::BasicSI;
  UserExpression approximatedSIExpression;
  input.cloneAndSimplifyAndApproximate(expressions + numberOfExpressions++,
                                       &approximatedSIExpression, &ctx);
  assert(!expressions[numberOfExpressions - 1].isUninitialized());

  // Memoize distinct layouts
  Layout exactOutputLayout =
      Shared::PoincareHelpers::CreateLayout(exactOutput, context);
  for (size_t i = 0; i < k_maxNumberOfExpressionCells; i++) {
    if (!expressions[i].isUninitialized()) {
      Layout layout =
          Shared::PoincareHelpers::CreateLayout(expressions[i], context);
      // Skip layouts identical to exactOutput
      if (exactOutputLayout.isIdenticalTo(layout, true)) {
        continue;
      }
      // Skip duplicate layouts
      bool skipLayout = false;
      for (size_t j = 0; j < m_numberOfExpressionCells; j++) {
        Layout previousLayout =
            m_layouts[j].isUninitialized() ? m_exactLayouts[j] : m_layouts[j];
        if (previousLayout.isIdenticalTo(layout, true)) {
          skipLayout = true;
          break;
        }
      }
      if (skipLayout) {
        continue;
      }
      // Radians may have two layouts to display
      if (expressions[i].isInRadians(context)) {
        // Approximated radian expression has already been computed.
        Layout approximatedLayout = Shared::PoincareHelpers::CreateLayout(
            approximatedSIExpression, context);
        if (!approximatedLayout.isIdenticalTo(layout, true)) {
          m_exactLayouts[m_numberOfExpressionCells] = layout;
          m_approximatedLayouts[m_numberOfExpressionCells] = approximatedLayout;
        } else {
          m_layouts[m_numberOfExpressionCells] = layout;
        }
      } else {
        m_layouts[m_numberOfExpressionCells] = layout;
      }
      m_numberOfExpressionCells++;
    }
  }

  // Initialize buffer cells
  for (size_t i = 0; i < k_maxNumberOfBufferCells; i++) {
    m_referenceValues[i] = nullptr;
  }

  // Compute SI Value
  m_SIValue = approximatedSIExpression.approximateToScalar<double>(
      angleUnit(), complexFormat());
  // Set upper and lower reference values
  m_numberOfBufferCells = UnitComparison::FindUpperAndLowerReferenceValues(
      m_SIValue, approximatedSIExpression, context, m_referenceValues,
      &m_tableIndexForComparison);
  if (m_numberOfExpressionCells + m_numberOfBufferCells == 0) {
    /* No reference values nor relevant unit display modes have been found, fall
     * back to the exact output to have something to display.
     * TODO_PCJ: Detect these cases earlier and return false in
     * AdditionalResultsType::HasUnit */
    m_layouts[m_numberOfExpressionCells++] = exactOutputLayout;
  }
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

void UnitListController::fillBufferCellAtIndex(BufferCell* bufferCell,
                                               int index) {
  assert(index < m_numberOfBufferCells);
  const UnitComparison::ReferenceValue* referenceValue =
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

Layout UnitListController::layoutAtIndex(HighlightCell* cell, int index) {
  assert(index >= 0);
  if (index < m_numberOfExpressionCells) {
    return ExpressionsListController::layoutAtIndex(cell, index);
  }
  index = index - m_numberOfExpressionCells;
  assert(index < m_numberOfBufferCells);
  return UnitComparison::BuildComparisonExpression(
             m_SIValue, m_referenceValues[index], m_tableIndexForComparison)
      .createLayout(Preferences::PrintFloatMode::Decimal,
                    Poincare::Preferences::LargeNumberOfSignificantDigits,
                    nullptr);
}

}  // namespace Calculation
