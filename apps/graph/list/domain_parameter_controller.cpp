#include "domain_parameter_controller.h"
#include <apps/i18n.h>
#include "../app.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

DomainParameterController::DomainParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  FloatParameterController<float>(parentResponder),
  m_domainCells{},
  m_record(),
  m_tempDomain(),
  m_confirmPopUpController(Invocation([](void * context, void * sender) {
    Container::activeApp()->dismissModalViewController();
    ((DomainParameterController *)context)->stackController()->pop();
    return true;
  }, this))
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_domainCells[i].setParentResponder(&m_selectableTableView);
    m_domainCells[i].textField()->setDelegates(inputEventHandlerDelegate, this);
  }
}

void DomainParameterController::viewWillAppear() {
  // Initialize m_tempParameters to the extracted value.
  extractParameters();
  FloatParameterController::viewWillAppear();
}

void DomainParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }
  MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *)cell;
  Shared::ContinuousFunction::PlotType plotType = function()->plotType();
  switch (plotType) {
    case Shared::ContinuousFunction::PlotType::Cartesian:
    {
      I18n::Message labels[k_totalNumberOfCell] = {I18n::Message::XMin, I18n::Message::XMax};
      myCell->setMessage(labels[index]);
      break;
    }
    case Shared::ContinuousFunction::PlotType::Parametric:
    {
      I18n::Message labels[k_totalNumberOfCell] = {I18n::Message::TMin, I18n::Message::TMax};
      myCell->setMessage(labels[index]);
      break;
    }
    default:
    {
      assert(plotType == Shared::ContinuousFunction::PlotType::Polar);
      I18n::Message labels[k_totalNumberOfCell] = {I18n::Message::ThetaMin, I18n::Message::ThetaMax};
      myCell->setMessage(labels[index]);
      break;
    }
  }
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

HighlightCell * DomainParameterController::reusableParameterCell(int index, int type) {
  assert(index >= 0 && index < k_totalNumberOfCell);
  return &m_domainCells[index];
}

bool DomainParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left && stackController()->depth() > 2) {
    stackController()->pop();
    return true;
  }
  if (event == Ion::Events::Back && !equalTempParameters()) {
    // Open pop-up to confirm discarding values
    Container::activeApp()->displayModalViewController(&m_confirmPopUpController, 0.f, 0.f, Metric::ExamPopUpTopMargin, Metric::PopUpRightMargin, Metric::ExamPopUpBottomMargin, Metric::PopUpLeftMargin);
    return true;
  }
  return false;
}

float DomainParameterController::parameterAtIndex(int index) {
  return index == 0 ? m_tempDomain.min() : m_tempDomain.max();
}

void DomainParameterController::extractParameters() {
  setParameterAtIndex(0, function()->tMin());
  setParameterAtIndex(1, function()->tMax());
  /* Setting m_tempDomain tMin might affect m_tempDomain.max(), but setting tMax
   * right after will not affect m_tempDomain.min() because Function's Range1D
   * parameters are valid (tMax>tMin), and final tMin value is already set.
   * Same happens in confirmParameters when setting function's parameters from
   * valid m_tempDomain parameters. */
  assert(equalTempParameters());
}

bool DomainParameterController::setParameterAtIndex(int parameterIndex, float f) {
  /* Setting Min (or Max) parameter can alter the previously set Max
   * (or Min) parameter if Max <= Min. */
  parameterIndex == 0 ? m_tempDomain.setMin(f) : m_tempDomain.setMax(f);
  return true;
}

void DomainParameterController::confirmParameters() {
  function()->setTMin(parameterAtIndex(0));
  function()->setTMax(parameterAtIndex(1));
  // See comment on Range1D initialization in extractParameters
  assert(equalTempParameters());
}

bool DomainParameterController::equalTempParameters() {
  return function()->tMin() == m_tempDomain.min() && function()->tMax() == m_tempDomain.max();
}

void DomainParameterController::buttonAction() {
  confirmParameters();
  StackViewController * stack = stackController();
  stack->pop();
  stack->pop();
}

Shared::ExpiringPointer<Shared::ContinuousFunction> DomainParameterController::function() const {
  assert(!m_record.isNull());
  App * myApp = App::app();
  return myApp->functionStore()->modelForRecord(m_record);
}

FloatParameterController<float>::InfinityTolerance DomainParameterController::infinityAllowanceForRow(int row) const {
  Shared::ContinuousFunction::PlotType plotType = function()->plotType();
  if (plotType == Shared::ContinuousFunction::PlotType::Cartesian) {
    return row == 0 ? FloatParameterController<float>::InfinityTolerance::MinusInfinity : FloatParameterController<float>::InfinityTolerance::PlusInfinity;
  }
  return FloatParameterController<float>::InfinityTolerance::None;
}

}
