#include "single_interactive_curve_view_range_controller.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

// SingleRangeController

SingleInteractiveCurveViewRangeController::
    SingleInteractiveCurveViewRangeController(
        Responder* parentResponder, InteractiveCurveViewRange* interactiveRange,
        Shared::MessagePopUpController* confirmPopUpController)
    : SingleRangeController(parentResponder, confirmPopUpController),
      m_range(interactiveRange) {}

void SingleInteractiveCurveViewRangeController::setAxis(Axis axis) {
  m_axis = axis;
  extractParameters();
}

bool SingleInteractiveCurveViewRangeController::parametersAreDifferent() {
  /* m_secondaryRangeParam is ignored here because it is only relevant when main
   * parameters (xAuto) are different. */
  float min = m_axis == Axis::X ? m_range->xMin() : m_range->yMin();
  float max = m_axis == Axis::X ? m_range->xMax() : m_range->yMax();

  assert(m_axis == Axis::X || m_axis == Axis::Y);

  return m_autoParam != (m_range->isAuto(m_axis)) ||
         m_rangeParam.min() != min || m_rangeParam.max() != max;
}

void SingleInteractiveCurveViewRangeController::extractParameters() {
  m_autoParam = m_range->isAuto(m_axis);

  if (m_axis == Axis::X) {
    m_rangeParam = Range1D::ValidRangeBetween(m_range->xMin(), m_range->xMax());
  } else {
    assert(m_axis == Axis::Y);
    m_rangeParam = Range1D::ValidRangeBetween(m_range->yMin(), m_range->yMax());
  }
  // Reset m_secondaryRangeParam
  m_secondaryRangeParam = Range1D();
}

void SingleInteractiveCurveViewRangeController::setAutoRange() {
  assert(m_autoParam);
  if (m_range->isAuto(m_axis)) {
    // Parameters are already computed in m_range
    extractParameters();
  } else {
    /* Create and update a temporary InteractiveCurveViewRange to recompute
     * parameters. */
    Shared::InteractiveCurveViewRange tempRange(*m_range);
    tempRange.setAuto(m_axis, m_autoParam);
    tempRange.computeRanges();

    float min = m_axis == Axis::X ? tempRange.xMin() : tempRange.yMin();
    float max = m_axis == Axis::X ? tempRange.xMax() : tempRange.yMax();
    m_rangeParam = Range1D::ValidRangeBetween(min, max);
    if (m_axis == Axis::X) {
      /* The y range has been updated too and must be stored for
       * confirmParameters. */
      m_secondaryRangeParam =
          Range1D::ValidRangeBetween(tempRange.yMin(), tempRange.yMax());
    } else {
      assert(m_axis == Axis::Y);
    }
  }
}

void SingleInteractiveCurveViewRangeController::confirmParameters() {
  if (!parametersAreDifferent()) {
    return;
  }
  // Deactivate auto status before updating values.
  m_range->setAuto(m_axis, false);

  if (m_axis == Axis::X) {
    m_range->setXRange(m_rangeParam.min(), m_rangeParam.max());
    m_range->setAuto(Axis::X, m_autoParam);
    if (m_autoParam && m_range->isAuto(Axis::Y)) {
      /* yMin and yMax must also be updated. We could avoid having to store
       * these values if we called m_range->computeRanges() instead, but it
       * would cost a significant computation time. */
      assert(!std::isnan(m_secondaryRangeParam.min()) &&
             !std::isnan(m_secondaryRangeParam.max()));

      m_range->setAuto(Axis::Y, false);
      m_range->setYRange(m_secondaryRangeParam.min(),
                         m_secondaryRangeParam.max());
      m_range->setAuto(Axis::Y, true);
    }
  } else {
    assert(m_axis == Axis::Y);
    m_range->setYRange(m_rangeParam.min(), m_rangeParam.max());
    m_range->setAuto(Axis::Y, m_autoParam);
  }
  assert(!parametersAreDifferent());
}

}  // namespace Shared
