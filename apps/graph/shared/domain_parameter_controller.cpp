#include "domain_parameter_controller.h"
#include "../app.h"
#include <poincare/infinity.h>
#include <assert.h>

using namespace Shared;
using namespace Escher;
using namespace Poincare;

namespace Graph {

DomainParameterController::DomainParameterController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate) :
  Shared::SingleRangeController(parentResponder, inputEventHandlerDelegate, &m_confirmPopUpController),
  m_confirmPopUpController(Invocation::Builder<DomainParameterController>([](DomainParameterController * controller, void * sender) {
    Container::activeApp()->modalViewController()->dismissModalViewController();
    controller->pop(false);
    return true;
  }, this))
{}

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
  return !(function()->properties().isCartesian() && textFieldShouldFinishEditing(textField, event) && textField->text()[0] == '\0') && FloatParameterController<float>::textFieldDidReceiveEvent(textField, event);
}

bool DomainParameterController::textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  switchToolboxContent(textField, false);
  if (text[0] == '\0') {
    if (textField == m_boundsCells[0].textField()) {
      text = Infinity::Name(true);
    } else {
      assert(textField == m_boundsCells[1].textField());
      text = Infinity::Name(false);
    }
  }
  return FloatParameterController<float>::textFieldDidFinishEditing(textField, text, event);
}

bool DomainParameterController::textFieldDidAbortEditing(AbstractTextField * textField) {
  switchToolboxContent(textField, false);
  return false;
}

I18n::Message DomainParameterController::parameterMessage(int index) const {
  ContinuousFunctionProperties plotProperties = function()->properties();
  if (plotProperties.isParametric()) {
    return index == 0 ? I18n::Message::TMin : I18n::Message::TMax;
  } else if (plotProperties.isPolar()) {
    return index == 0 ? I18n::Message::ThetaMin : I18n::Message::ThetaMax;
  } else {
    return index == 0 ? I18n::Message::XMin : I18n::Message::XMax;
  }
}

bool DomainParameterController::parametersAreDifferent() {
  return function()->tMin() != m_rangeParam.min()
      || function()->tMax() != m_rangeParam.max()
      || function()->tAuto() != m_autoParam;
}

void DomainParameterController::extractParameters() {
  setParameterAtIndex(1, function()->tMin());
  setParameterAtIndex(2, function()->tMax());
  m_autoParam = function()->tAuto();
  /* Setting m_rangeParam tMin might affect m_rangeParam.max(), but setting tMax
   * right after will not affect m_rangeParam.min() because Function's Range1D
   * parameters are valid (tMax>tMin), and final tMin value is already set.
   * Same happens in confirmParameters when setting function's parameters from
   * valid m_rangeParam parameters. */
  assert(!parametersAreDifferent());
}

void DomainParameterController::setAutoStatus(bool autoParam) {
  if (m_autoParam == autoParam) {
    return;
  }
  if (autoParam) {
    setParameterAtIndex(1, function()->autoTMin());
    setParameterAtIndex(2, function()->autoTMax());
  }
  m_autoParam = autoParam;
  resetMemoization();
  m_selectableTableView.reloadData();
}

bool DomainParameterController::setParameterAtIndex(int parameterIndex, float f) {
  /* Setting Min (or Max) parameter can alter the previously set Max
   * (or Min) parameter if Max <= Min. It also disable the auto domain. */
  m_autoParam = false;
  parameterIndex == 1 ? m_rangeParam.setMin(f, INFINITY) : m_rangeParam.setMax(f, INFINITY);
  return true;
}

void DomainParameterController::confirmParameters() {
  function()->setTAuto(m_autoParam);
  if (!m_autoParam) {
    function()->setTMin(m_rangeParam.min());
    function()->setTMax(m_rangeParam.max());
  }
  // See comment on Range1D initialization in extractParameters
  assert(!parametersAreDifferent());
}

FloatParameterController<float>::InfinityTolerance DomainParameterController::infinityAllowanceForRow(int row) const {
  if (function()->properties().isCartesian()) {
    return row == 1 ? FloatParameterController<float>::InfinityTolerance::MinusInfinity : FloatParameterController<float>::InfinityTolerance::PlusInfinity;
  }
  return FloatParameterController<float>::InfinityTolerance::None;
}

Shared::ExpiringPointer<Shared::ContinuousFunction> DomainParameterController::function() const {
  assert(!m_record.isNull());
  return App::app()->functionStore()->modelForRecord(m_record);
}

void DomainParameterController::switchToolboxContent(Escher::AbstractTextField * textField, bool setSpecificContent) {
  assert(textField == m_boundsCells[0].textField() || textField == m_boundsCells[1].textField());
  FunctionToolbox::AddedCellsContent content;
  if (setSpecificContent) {
    content = !function()->properties().isCartesian() ? FunctionToolbox::AddedCellsContent::None
            : textField == m_boundsCells[0].textField() ? FunctionToolbox::AddedCellsContent::NegativeInfinity
            : FunctionToolbox::AddedCellsContent::PositiveInfinity;
  } else {
    content = FunctionToolbox::AddedCellsContent::ComparisonOperators;
  }
  FunctionToolbox * toolbox = App::app()->listController()->toolbox();
  toolbox->setAddedCellsContent(content);
}

}
