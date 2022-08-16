#include "domain_parameter_controller.h"
#include "../app.h"
#include <apps/i18n.h>
#include <poincare/infinity.h>
#include <assert.h>

using namespace Shared;
using namespace Escher;
using namespace Poincare;

namespace Graph {

DomainParameterController::DomainParameterController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate) :
  FloatParameterController<float>(parentResponder),
  m_confirmPopUpController(Invocation([](void * context, void * sender) {
    Container::activeApp()->dismissModalViewController();
    ((DomainParameterController *)context)->stackController()->pop();
    return true;
  }, this))
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_domainCells[i].setParentResponder(&m_selectableTableView);
    m_domainCells[i].setDelegates(inputEventHandlerDelegate, this);
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
  MessageTableCellWithEditableText * myCell = static_cast<MessageTableCellWithEditableText *>(cell);
  ContinuousFunction::PlotType plotType = function()->plotType();
  switch (plotType) {
    case ContinuousFunction::PlotType::Parametric:
    {
      I18n::Message labels[k_totalNumberOfCell] = {I18n::Message::TMin, I18n::Message::TMax};
      myCell->setMessage(labels[index]);
      break;
    }
    case ContinuousFunction::PlotType::Polar:
    {
      I18n::Message labels[k_totalNumberOfCell] = {I18n::Message::ThetaMin, I18n::Message::ThetaMax};
      myCell->setMessage(labels[index]);
      break;
    }
    default:
    {
      I18n::Message labels[k_totalNumberOfCell] = {I18n::Message::XMin, I18n::Message::XMax};
      myCell->setMessage(labels[index]);
      break;
    }
  }
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

bool DomainParameterController::textFieldDidReceiveEvent(AbstractTextField * textField, Ion::Events::Event event) {
  /* Set the right additional cells in the toolbox. Ideally, we would like to
   * update the toolbox in textFieldDidStartEditing, but if the edition is
   * started by pressing the Toolbox key, textFieldDidStartEditing will be
   * called after displayModalViewController. */
  if (event == Ion::Events::Toolbox) {
    switchToolboxContent(textField, true);
  }
  /* Do not refuse empty text for functions of x, as that will later be
   * replaced by ±inf. */
  return !(function()->isAlongXorY() && textFieldShouldFinishEditing(textField, event) && textField->text()[0] == '\0') && FloatParameterController<float>::textFieldDidReceiveEvent(textField, event);
}

bool DomainParameterController::textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  switchToolboxContent(textField, false);
  if (text[0] == '\0') {
    if (textField == m_domainCells[0].textField()) {
      text = Infinity::Name(true);
    } else {
      assert(textField == m_domainCells[1].textField());
      text = Infinity::Name(false);
    }
  }
  return FloatParameterController<float>::textFieldDidFinishEditing(textField, text, event);
}

bool DomainParameterController::textFieldDidAbortEditing(AbstractTextField * textField) {
  switchToolboxContent(textField, false);
  return false;
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
    m_confirmPopUpController.presentModally();
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
  if (function()->isAlongXorY()) {
    return row == 0 ? FloatParameterController<float>::InfinityTolerance::MinusInfinity : FloatParameterController<float>::InfinityTolerance::PlusInfinity;
  }
  return FloatParameterController<float>::InfinityTolerance::None;
}

void DomainParameterController::switchToolboxContent(Escher::AbstractTextField * textField, bool setSpecificContent) {
  assert(textField == m_domainCells[0].textField() || textField == m_domainCells[1].textField());
  FunctionToolbox::AddedCellsContent content;
  if (setSpecificContent) {
    content = !function()->isAlongXorY() ? FunctionToolbox::AddedCellsContent::None
            : textField == m_domainCells[0].textField() ? FunctionToolbox::AddedCellsContent::NegativeInfinity
            : FunctionToolbox::AddedCellsContent::PositiveInfinity;
  } else {
    content = FunctionToolbox::AddedCellsContent::ComparisonOperators;
  }
  FunctionToolbox * toolbox = App::app()->listController()->toolboxForInputEventHandler(textField);
  toolbox->setAddedCellsContent(content);
}

}
