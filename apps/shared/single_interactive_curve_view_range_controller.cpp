#include "single_interactive_curve_view_range_controller.h"

#include "shared/poincare_helpers.h"
#include "shared/single_range_controller.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

// SingleRangeController

SingleInteractiveCurveViewRangeController::
    SingleInteractiveCurveViewRangeController(
        Responder* parentResponder, InteractiveCurveViewRange* interactiveRange,
        Shared::MessagePopUpController* confirmPopUpController)
    : SingleRangeControllerExactExpressions(
          parentResponder, confirmPopUpController, &m_bottomMessageView),
      m_range(interactiveRange),
      m_gridUnitCell(&this->m_selectableListView, this),
      m_stepParameter(NAN) {
  m_gridUnitCell.label()->setMessage(I18n::Message::Step);
}

void SingleInteractiveCurveViewRangeController::setAxis(OMG::Axis axis) {
  m_axis = axis;
  extractParameters();
}

bool SingleInteractiveCurveViewRangeController::boundsParametersAreDifferent() {
  /* m_secondaryRangeParam is ignored here because it is only relevant when main
   * parameters (xAuto) are different. */
  float min =
      m_axis == OMG::Axis::Horizontal ? m_range->xMin() : m_range->yMin();
  float max =
      m_axis == OMG::Axis::Horizontal ? m_range->xMax() : m_range->yMax();

  return m_autoParam != m_range->zoomAuto(m_axis) ||
         m_rangeParam.min() != min || m_rangeParam.max() != max;
}

bool SingleInteractiveCurveViewRangeController::parametersAreDifferent() {
  FloatType newStepValue = PoincareHelpers::ToFloat(m_stepParameter);
  FloatType currentStepValue =
      PoincareHelpers::ToFloat(m_range->userStep(m_axis));
  bool stepParameterIsDifferent =
      std::isnan(newStepValue) != std::isnan(currentStepValue) ||
      (!std::isnan(newStepValue) && !std::isnan(currentStepValue) &&
       newStepValue != currentStepValue);
  return stepParameterIsDifferent || boundsParametersAreDifferent();
}

void SingleInteractiveCurveViewRangeController::extractParameters() {
  m_autoParam = m_range->zoomAuto(m_axis);
  m_stepParameter = m_range->userStep(m_axis);
  if (m_axis == OMG::Axis::Horizontal) {
    m_rangeParam =
        Range1D<float>::ValidRangeBetween(m_range->xMin(), m_range->xMax());
  } else {
    assert(m_axis == OMG::Axis::Vertical);
    m_rangeParam =
        Range1D<float>::ValidRangeBetween(m_range->yMin(), m_range->yMax());
  }
  // Reset m_secondaryRangeParam
  m_secondaryRangeParam = Range1D<float>();
}

void SingleInteractiveCurveViewRangeController::setAutoRange() {
  assert(m_autoParam);
  if (m_range->zoomAuto(m_axis)) {
    // Parameters are already computed in m_range
    extractParameters();
  } else {
    /* Create and update a temporary InteractiveCurveViewRange to recompute
     * parameters. */
    Shared::InteractiveCurveViewRange tempRange(*m_range);
    tempRange.setZoomAuto(m_axis, m_autoParam);
    tempRange.computeRanges();

    float min =
        m_axis == OMG::Axis::Horizontal ? tempRange.xMin() : tempRange.yMin();
    float max =
        m_axis == OMG::Axis::Horizontal ? tempRange.xMax() : tempRange.yMax();
    m_rangeParam = Range1D<float>::ValidRangeBetween(min, max);
    if (m_axis == OMG::Axis::Horizontal) {
      /* The y range has been updated too and must be stored for
       * confirmParameters. */
      m_secondaryRangeParam =
          Range1D<float>::ValidRangeBetween(tempRange.yMin(), tempRange.yMax());
    }
    // Reset the grid unit to "auto"
    m_stepParameter = ExpressionOrFloat{};
  }
}

void SingleInteractiveCurveViewRangeController::confirmParameters() {
  m_range->setUserStep(m_axis, m_stepParameter);
  if (boundsParametersAreDifferent()) {
    // Deactivate auto status before updating values.
    m_range->setZoomAuto(m_axis, false);
    if (m_axis == OMG::Axis::Horizontal) {
      m_range->setXRange(m_rangeParam.min(), m_rangeParam.max());
      m_range->setZoomAuto(m_axis, m_autoParam);
      if (m_autoParam && m_range->zoomAuto(OMG::Axis::Vertical)) {
        /* yMin and yMax must also be updated. We could avoid having to store
         * these values if we called m_range->computeRanges() instead, but it
         * would cost a significant computation time. */
        assert(!std::isnan(m_secondaryRangeParam.min()) &&
               !std::isnan(m_secondaryRangeParam.max()));

        m_range->setZoomAuto(OMG::Axis::Vertical, false);
        m_range->setYRange(m_secondaryRangeParam.min(),
                           m_secondaryRangeParam.max());
        m_range->setZoomAuto(OMG::Axis::Vertical, true);
      }
    } else {
      assert(m_axis == OMG::Axis::Vertical);
      m_range->setYRange(m_rangeParam.min(), m_rangeParam.max());
      m_range->setZoomAuto(m_axis, m_autoParam);
    }
    assert(!boundsParametersAreDifferent());
  }
  assert(!parametersAreDifferent());
}

bool SingleInteractiveCurveViewRangeController::setParameterAtIndex(
    int parameterIndex, ParameterType value) {
  if (typeAtRow(parameterIndex) == k_stepCellType) {
    if (!IsValidStepValue(value)) {
      App::app()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }
    m_stepParameter = value;
    return true;
  }
  return SingleRangeControllerExactExpressions::setParameterAtIndex(
      parameterIndex, value);
}

int SingleInteractiveCurveViewRangeController::typeAtRow(int row) const {
  return row == 3 ? k_stepCellType
                  : SingleRangeControllerExactExpressions::typeAtRow(row);
}

KDCoordinate SingleInteractiveCurveViewRangeController::nonMemoizedRowHeight(
    int row) {
  return typeAtRow(row) == k_stepCellType
             ? m_gridUnitCell.minimalSizeForOptimalDisplay().height()
             : SingleRangeControllerExactExpressions::nonMemoizedRowHeight(row);
}

KDCoordinate SingleInteractiveCurveViewRangeController::separatorBeforeRow(
    int row) const {
  return typeAtRow(row) == k_stepCellType
             ? k_defaultRowSeparator
             : SingleRangeControllerExactExpressions::separatorBeforeRow(row);
}

int SingleInteractiveCurveViewRangeController::reusableParameterCellCount(
    int type) const {
  return type == k_stepCellType ? 1
                                : SingleRangeControllerExactExpressions::
                                      reusableParameterCellCount(type);
}

HighlightCell* SingleInteractiveCurveViewRangeController::reusableParameterCell(
    int index, int type) {
  return type == k_stepCellType
             ? &m_gridUnitCell
             : SingleRangeControllerExactExpressions::reusableParameterCell(
                   index, type);
}

TextField* SingleInteractiveCurveViewRangeController::textFieldOfCellAtIndex(
    HighlightCell* cell, int index) {
  if (typeAtRow(index) == k_stepCellType) {
    assert(cell == &m_gridUnitCell);
    return m_gridUnitCell.textField();
  }
  return SingleRangeControllerExactExpressions::textFieldOfCellAtIndex(cell,
                                                                       index);
}

SingleInteractiveCurveViewRangeController::ParameterType
SingleInteractiveCurveViewRangeController::parameterAtIndex(int index) {
  return typeAtRow(index) == k_stepCellType
             ? m_stepParameter
             : SingleRangeControllerExactExpressions::parameterAtIndex(index);
}

bool SingleInteractiveCurveViewRangeController::hasUndefinedValue(
    const char* text, ParameterType value, int row) const {
  if (text[0] == 0 && typeAtRow(row) == k_stepCellType) {
    // Accept empty inputs for grid unit cell
    return false;
  }
  return SingleRangeControllerExactExpressions::hasUndefinedValue(text, value,
                                                                  row);
}

void SingleInteractiveCurveViewRangeController::fillCellForRow(
    HighlightCell* cell, int row) {
  if (typeAtRow(row) == k_stepCellType && stepIsAuto()) {
    textFieldOfCellAtIndex(cell, row)->setText(
        I18n::translate(I18n::Message::DefaultSetting));
    return;
  }
  SingleRangeControllerExactExpressions::fillCellForRow(cell, row);
}

bool SingleInteractiveCurveViewRangeController::textFieldDidReceiveEvent(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) &&
      !textField->isEditing() && typeAtRow(selectedRow()) == k_stepCellType &&
      stepIsAuto()) {
    // Remove "Auto" text to start edition
    textField->setText("");
  }
  return SingleRangeControllerExactExpressions::textFieldDidReceiveEvent(
      textField, event);
}

void SingleInteractiveCurveViewRangeController::textFieldDidAbortEditing(
    Escher::AbstractTextField* textField) {
  // Reload selected cell to display "Auto" when text is empty
  m_selectableListView.reloadSelectedCell();
  SingleRangeControllerExactExpressions::textFieldDidAbortEditing(textField);
}

}  // namespace Shared
