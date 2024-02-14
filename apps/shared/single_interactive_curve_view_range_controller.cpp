#include "single_interactive_curve_view_range_controller.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

// SingleRangeController

SingleInteractiveCurveViewRangeController::
    SingleInteractiveCurveViewRangeController(
        Responder *parentResponder, InteractiveCurveViewRange *interactiveRange,
        Shared::MessagePopUpController *confirmPopUpController)
    : SingleRangeController<float>(parentResponder, confirmPopUpController),
      m_range(interactiveRange),
      m_gridUnitCell(&this->m_selectableListView, this),
      m_gridUnitParam(k_autoGridUnitValue) {
  m_gridUnitCell.label()->setMessage(I18n::Message::Step);
}

void SingleInteractiveCurveViewRangeController::setAxis(Axis axis) {
  m_axis = axis;
  extractParameters();
}

bool SingleInteractiveCurveViewRangeController::boundsParametersAreDifferent() {
  /* m_secondaryRangeParam is ignored here because it is only relevant when main
   * parameters (xAuto) are different. */
  float min = m_axis == Axis::X ? m_range->xMin() : m_range->yMin();
  float max = m_axis == Axis::X ? m_range->xMax() : m_range->yMax();

  return m_autoParam != m_range->zoomAuto(m_axis) ||
         m_rangeParam.min() != min || m_rangeParam.max() != max;
}

bool SingleInteractiveCurveViewRangeController::parametersAreDifferent() {
  return boundsParametersAreDifferent() ||
         m_gridUnitParam != m_range->userGridUnit(m_axis);
}

void SingleInteractiveCurveViewRangeController::extractParameters() {
  m_autoParam = m_range->zoomAuto(m_axis);
  m_gridUnitParam = m_range->userGridUnit(m_axis);
  if (m_axis == Axis::X) {
    m_rangeParam =
        Range1D<float>::ValidRangeBetween(m_range->xMin(), m_range->xMax());
  } else {
    assert(m_axis == Axis::Y);
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

    float min = m_axis == Axis::X ? tempRange.xMin() : tempRange.yMin();
    float max = m_axis == Axis::X ? tempRange.xMax() : tempRange.yMax();
    m_rangeParam = Range1D<float>::ValidRangeBetween(min, max);
    if (m_axis == Axis::X) {
      /* The y range has been updated too and must be stored for
       * confirmParameters. */
      m_secondaryRangeParam =
          Range1D<float>::ValidRangeBetween(tempRange.yMin(), tempRange.yMax());
    }
  }
}

void SingleInteractiveCurveViewRangeController::confirmParameters() {
  m_range->setUserGridUnit(m_axis, m_gridUnitParam);
  if (boundsParametersAreDifferent()) {
    // Deactivate auto status before updating values.
    m_range->setZoomAuto(m_axis, false);
    if (m_axis == Axis::X) {
      m_range->setXRange(m_rangeParam.min(), m_rangeParam.max());
      m_range->setZoomAuto(m_axis, m_autoParam);
      if (m_autoParam && m_range->zoomAuto(Axis::Y)) {
        /* yMin and yMax must also be updated. We could avoid having to store
         * these values if we called m_range->computeRanges() instead, but it
         * would cost a significant computation time. */
        assert(!std::isnan(m_secondaryRangeParam.min()) &&
               !std::isnan(m_secondaryRangeParam.max()));

        m_range->setZoomAuto(Axis::Y, false);
        m_range->setYRange(m_secondaryRangeParam.min(),
                           m_secondaryRangeParam.max());
        m_range->setZoomAuto(Axis::Y, true);
      }
    } else {
      assert(m_axis == Axis::Y);
      m_range->setYRange(m_rangeParam.min(), m_rangeParam.max());
      m_range->setZoomAuto(m_axis, m_autoParam);
    }
    assert(!boundsParametersAreDifferent());
  }
  assert(!parametersAreDifferent());
}

bool SingleInteractiveCurveViewRangeController::setParameterAtIndex(
    int parameterIndex, float f) {
  if (typeAtRow(parameterIndex) == k_gridUnitCellType) {
    if (f <= 0.0f) {
      App::app()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }
    m_gridUnitParam = std::isnan(f) ? k_autoGridUnitValue : f;
    return true;
  }
  return SingleRangeController<float>::setParameterAtIndex(parameterIndex, f);
}

int SingleInteractiveCurveViewRangeController::typeAtRow(int row) const {
  return row == 3 ? k_gridUnitCellType
                  : SingleRangeController<float>::typeAtRow(row);
}

KDCoordinate SingleInteractiveCurveViewRangeController::nonMemoizedRowHeight(
    int row) {
  return typeAtRow(row) == k_gridUnitCellType
             ? m_gridUnitCell.minimalSizeForOptimalDisplay().height()
             : SingleRangeController<float>::nonMemoizedRowHeight(row);
}

KDCoordinate SingleInteractiveCurveViewRangeController::separatorBeforeRow(
    int row) {
  return typeAtRow(row) == k_gridUnitCellType
             ? k_defaultRowSeparator
             : SingleRangeController<float>::separatorBeforeRow(row);
}

int SingleInteractiveCurveViewRangeController::reusableParameterCellCount(
    int type) const {
  return type == k_gridUnitCellType
             ? 1
             : SingleRangeController<float>::reusableParameterCellCount(type);
}

HighlightCell *SingleInteractiveCurveViewRangeController::reusableParameterCell(
    int index, int type) {
  return type == k_gridUnitCellType
             ? &m_gridUnitCell
             : SingleRangeController<float>::reusableParameterCell(index, type);
}

TextField *SingleInteractiveCurveViewRangeController::textFieldOfCellAtIndex(
    HighlightCell *cell, int index) {
  if (typeAtRow(index) == k_gridUnitCellType) {
    assert(cell == &m_gridUnitCell);
    return m_gridUnitCell.textField();
  }
  return SingleRangeController<float>::textFieldOfCellAtIndex(cell, index);
}

float SingleInteractiveCurveViewRangeController::parameterAtIndex(int index) {
  return typeAtRow(index) == k_gridUnitCellType
             ? m_gridUnitParam
             : SingleRangeController<float>::parameterAtIndex(index);
}

bool SingleInteractiveCurveViewRangeController::hasUndefinedValue(
    const char *text, float floatValue, int row) const {
  if (text[0] == 0 && typeAtRow(row) == k_gridUnitCellType) {
    // Accept empty inputs for grid unit cell
    return false;
  }
  return SingleRangeController<float>::hasUndefinedValue(text, floatValue, row);
}

void SingleInteractiveCurveViewRangeController::fillCellForRow(
    HighlightCell *cell, int row) {
  if (typeAtRow(row) == k_gridUnitCellType && gridUnitIsAuto()) {
    textFieldOfCellAtIndex(cell, row)->setText(
        I18n::translate(I18n::Message::DefaultSetting));
    return;
  }
  SingleRangeController<float>::fillCellForRow(cell, row);
}

bool SingleInteractiveCurveViewRangeController::textFieldDidReceiveEvent(
    Escher::AbstractTextField *textField, Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) &&
      !textField->isEditing() &&
      typeAtRow(selectedRow()) == k_gridUnitCellType && gridUnitIsAuto()) {
    // Remove "Auto" text to start edition
    textField->setText("");
  }
  return SingleRangeController<float>::textFieldDidReceiveEvent(textField,
                                                                event);
}

}  // namespace Shared
