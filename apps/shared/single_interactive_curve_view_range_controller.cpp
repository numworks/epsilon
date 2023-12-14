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

void SingleInteractiveCurveViewRangeController::setEditXRange(bool editXRange) {
  m_editXRange = editXRange;
  extractParameters();
}

bool SingleInteractiveCurveViewRangeController::parametersAreDifferent() {
  /* m_secondaryRangeParam is ignored here because it is only relevant when main
   * parameters (xAuto) are different. */
  if (m_editXRange) {
    return m_autoParam != (m_range->xAuto()) ||
           m_rangeParam.min() != (m_range->xMin()) ||
           m_rangeParam.max() != (m_range->xMax());
  }
  return m_autoParam != (m_range->yAuto()) ||
         m_rangeParam.min() != (m_range->yMin()) ||
         m_rangeParam.max() != (m_range->yMax());
}

void SingleInteractiveCurveViewRangeController::extractParameters() {
  if (m_editXRange) {
    m_autoParam = m_range->xAuto();
    m_rangeParam = Range1D::ValidRangeBetween(m_range->xMin(), m_range->xMax());
  } else {
    m_autoParam = m_range->yAuto();
    m_rangeParam = Range1D::ValidRangeBetween(m_range->yMin(), m_range->yMax());
  }
  // Reset m_secondaryRangeParam
  m_secondaryRangeParam = Range1D();
}

void SingleInteractiveCurveViewRangeController::setAutoRange() {
  assert(m_autoParam);
  if (m_editXRange ? m_range->xAuto() : m_range->yAuto()) {
    // Parameters are already computed in m_range
    extractParameters();
  } else {
    /* Create and update a temporary InteractiveCurveViewRange to recompute
     * parameters. */
    Shared::InteractiveCurveViewRange tempRange(*m_range);
    if (m_editXRange) {
      tempRange.setXAuto(m_autoParam);
    } else {
      tempRange.setYAuto(m_autoParam);
    }
    tempRange.computeRanges();
    float min = m_editXRange ? tempRange.xMin() : tempRange.yMin();
    float max = m_editXRange ? tempRange.xMax() : tempRange.yMax();
    m_rangeParam = Range1D::ValidRangeBetween(min, max);
    if (m_editXRange) {
      /* The y range has been updated too and must be stored for
       * confirmParameters. */
      m_secondaryRangeParam =
          Range1D::ValidRangeBetween(tempRange.yMin(), tempRange.yMax());
    }
  }
}

void SingleInteractiveCurveViewRangeController::confirmParameters() {
  if (!parametersAreDifferent()) {
    return;
  }
  // Deactivate auto status before updating values.
  if (m_editXRange) {
    m_range->setXAuto(false);
    m_range->setXRange(m_rangeParam.min(), m_rangeParam.max());
    m_range->setXAuto(m_autoParam);
    if (m_autoParam && m_range->yAuto()) {
      /* yMin and yMax must also be updated. We could avoid having to store
       * these values if we called m_range->computeRanges() instead, but it
       * would cost a significant computation time. */
      assert(!std::isnan(m_secondaryRangeParam.min()) &&
             !std::isnan(m_secondaryRangeParam.max()));
      m_range->setYAuto(false);
      m_range->setYRange(m_secondaryRangeParam.min(),
                         m_secondaryRangeParam.max());
      m_range->setYAuto(true);
    }
  } else {
    m_range->setYAuto(false);
    m_range->setYRange(m_rangeParam.min(), m_rangeParam.max());
    m_range->setYAuto(m_autoParam);
  }
  assert(!parametersAreDifferent());
}

}  // namespace Shared
