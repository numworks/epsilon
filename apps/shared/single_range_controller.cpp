#include "single_range_controller.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

// SingleRangeController

SingleRangeController::SingleRangeController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * interactiveRange, Shared::MessagePopUpController * confirmPopUpController) :
  FloatParameterController<float>(parentResponder),
  m_autoCell(I18n::Message::DefaultSetting),
  m_range(interactiveRange),
  m_confirmPopUpController(confirmPopUpController)
{
  for (int i = 0; i < k_numberOfTextCells; i++) {
    m_boundsCells[i].setParentResponder(&m_selectableTableView);
    m_boundsCells[i].setDelegates(inputEventHandlerDelegate, this);
  }
}

void SingleRangeController::viewWillAppear() {
  extractParameters();
  FloatParameterController<float>::viewWillAppear();
}

HighlightCell * SingleRangeController::reusableCell(int index, int type) {
  if (type == k_autoCellType) {
    return &m_autoCell;
  }
  if (type == k_parameterCellType) {
    return m_boundsCells + index;
  }
  return FloatParameterController<float>::reusableCell(index, type);
}

KDCoordinate SingleRangeController::nonMemoizedRowHeight(int j) {
  int type = typeAtIndex(j);
  HighlightCell * cell = type == k_autoCellType ? static_cast<HighlightCell *>(&m_autoCell) : type == k_parameterCellType ? static_cast<HighlightCell *>(&m_boundsCells[j - 1]) : nullptr;
  return cell ? heightForCellAtIndex(cell, j) : FloatParameterController<float>::nonMemoizedRowHeight(j);
}

void SingleRangeController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  int type = typeAtIndex(index);
  if (type == k_autoCellType) {
    m_autoCell.setState(autoStatus());
    return;
  }
  if (type == k_parameterCellType) {
    Escher::MessageTableCellWithEditableText * castedCell = static_cast<Escher::MessageTableCellWithEditableText *>(cell);
    castedCell->setMessage(index == 1 ? I18n::Message::Minimum : I18n::Message::Maximum);
  }
  FloatParameterController<float>::willDisplayCellForIndex(cell, index);
}

bool SingleRangeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left || event == Ion::Events::Back) {
    if (parametersAreDifferent()) {
      m_confirmPopUpController->presentModally();
    } else {
      stackController()->pop();
    }
    return true;
  }
  if (selectedRow() == 0 && (event == Ion::Events::OK || event == Ion::Events::EXE)) {
    // Update auto status
    setAutoStatus(!m_autoParam);
    return true;
  }
  return FloatParameterController<float>::handleEvent(event);
}

void SingleRangeController::setAutoStatus(bool autoParam) {
  if (m_autoParam == autoParam) {
    return;
  }
  m_autoParam = autoParam;
  if (m_autoParam) {
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
      m_rangeParam.setMin(m_editXRange ? tempRange.xMin() : tempRange.yMin());
      m_rangeParam.setMax(m_editXRange ? tempRange.xMax() : tempRange.yMax());
      if (m_editXRange) {
        /* The y range has been updated too and must be stored for
         * confirmParameters. */
        m_secondaryRangeParam.setMin(tempRange.yMin());
        m_secondaryRangeParam.setMax(tempRange.yMax());
      }
    }
  }
  resetMemoization();
  m_selectableTableView.reloadData();
}

void SingleRangeController::setEditXRange(bool editXRange) {
  m_editXRange = editXRange;
  extractParameters();
}

float SingleRangeController::parameterAtIndex(int index) {
  assert(index >= 1 && index < k_numberOfTextCells + 1);
  return (index == 1 ? m_rangeParam.min() : m_rangeParam.max());
}

HighlightCell * SingleRangeController::reusableParameterCell(int index, int type) {
  assert(index >= 1 && index < k_numberOfTextCells + 1);
  return &m_boundsCells[index - 1];
}

bool SingleRangeController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex >= 1 && parameterIndex < k_numberOfTextCells + 1);
  // Apply InteractiveCurveViewRange float bounds
  if (parameterIndex == 1) {
    m_rangeParam.setMin(f, InteractiveCurveViewRange::k_lowerMaxFloat, InteractiveCurveViewRange::k_upperMaxFloat);
  } else {
    m_rangeParam.setMax(f, InteractiveCurveViewRange::k_lowerMaxFloat, InteractiveCurveViewRange::k_upperMaxFloat);
  }
  return true;
}

void SingleRangeController::extractParameters() {
  if (m_editXRange) {
    m_autoParam = m_range->xAuto();
    m_rangeParam.setMin(m_range->xMin());
    m_rangeParam.setMax(m_range->xMax());
  } else {
    m_autoParam = m_range->yAuto();
    m_rangeParam.setMin(m_range->yMin());
    m_rangeParam.setMax(m_range->yMax());
  }
  // Reset m_secondaryRangeParam
  m_secondaryRangeParam.setMin(NAN);
  m_secondaryRangeParam.setMax(NAN);
}

bool SingleRangeController::parametersAreDifferent() {
  /* m_secondaryRangeParam is ignored here because it is only relevant when main
   * parameters (xAuto) are different. */
  if (m_editXRange) {
    return m_autoParam != (m_range->xAuto())
           || m_rangeParam.min() != (m_range->xMin())
           || m_rangeParam.max() != (m_range->xMax());
  }
  return m_autoParam != (m_range->yAuto())
         || m_rangeParam.min() != (m_range->yMin())
         || m_rangeParam.max() != (m_range->yMax());
}

void SingleRangeController::confirmParameters() {
  if (parametersAreDifferent()) {
    // Deactivate auto status before updating values.
    if (m_editXRange) {
      m_range->setXAuto(false);
      m_range->setXMin(m_rangeParam.min());
      m_range->setXMax(m_rangeParam.max());
      m_range->setXAuto(m_autoParam);
      if (m_autoParam && m_range->yAuto()) {
        /* yMin and yMax must also be updated. We could avoid having to store
         * these values if we called m_range->computeRanges() instead, but it
         * would cost a significant computation time. */
        assert(!std::isnan(m_secondaryRangeParam.min()) && !std::isnan(m_secondaryRangeParam.max()));
        m_range->setYAuto(false);
        m_range->setYMin(m_secondaryRangeParam.min());
        m_range->setYMax(m_secondaryRangeParam.max());
        m_range->setYAuto(true);
      }
    } else {
      m_range->setYAuto(false);
      m_range->setYMin(m_rangeParam.min());
      m_range->setYMax(m_rangeParam.max());
      m_range->setYAuto(m_autoParam);
    }
    assert(!parametersAreDifferent());
  }
}

void SingleRangeController::buttonAction() {
  confirmParameters();
  FloatParameterController<float>::buttonAction();
}

bool SingleRangeController::textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  setAutoStatus(false);
  return FloatParameterController<float>::textFieldDidFinishEditing(textField, text, event);
}

}
