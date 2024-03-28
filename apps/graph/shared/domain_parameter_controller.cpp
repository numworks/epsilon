#include "domain_parameter_controller.h"

#include <assert.h>
#include <poincare/infinity.h>
#include <poincare/k_tree.h>

#include "../app.h"

using namespace Shared;
using namespace Escher;

namespace Graph {

DomainParameterController::DomainParameterController(Responder* parentResponder)
    : Shared::SingleRangeController<float>(parentResponder,
                                           &m_confirmPopUpController),
      m_confirmPopUpController(Invocation::Builder<DomainParameterController>(
          [](DomainParameterController* controller, void* sender) {
            controller->pop(false);
            return true;
          },
          this)),
      m_currentTextFieldIsMinField(false) {}

bool DomainParameterController::textFieldDidReceiveEvent(
    AbstractTextField* textField, Ion::Events::Event event) {
  /* Set the right additional cells in the toolbox. Ideally, we would like to
   * update the toolbox in textFieldDidStartEditing, but if the edition is
   * started by pressing the Toolbox key, textFieldDidStartEditing will be
   * called after displayModalViewController. */
  if (event == Ion::Events::Toolbox) {
    switchToolboxContent(textField, true);
  }
  return FloatParameterController<float>::textFieldDidReceiveEvent(textField,
                                                                   event);
}

bool DomainParameterController::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  switchToolboxContent(textField, false);
  assert(!textField->isEditing());
  if (function()->properties().isCartesian() &&
      textField->draftText()[0] == '\0') {
    textField->setEditing(true);  // To edit draft text buffer in setText
    if (textField == m_boundsCells[0].textField()) {
      textField->setText(Poincare::Infinity::Name(true));
    } else {
      assert(textField == m_boundsCells[1].textField());
      textField->setText(Poincare::Infinity::Name(false));
    }
    textField->setEditing(false);  // set editing back to previous value
  }
  return SingleRangeController::textFieldDidFinishEditing(textField, event);
}

void DomainParameterController::textFieldDidAbortEditing(
    AbstractTextField* textField) {
  switchToolboxContent(textField, false);
}

I18n::Message DomainParameterController::parameterMessage(int index) const {
  ContinuousFunctionProperties plotProperties = function()->properties();
  if (plotProperties.isParametric()) {
    return index == 0 ? I18n::Message::TMin : I18n::Message::TMax;
  } else if (plotProperties.isPolar()) {
    return index == 0 ? I18n::Message::ThetaMin : I18n::Message::ThetaMax;
  } else if (plotProperties.isInversePolar()) {
    return index == 0 ? I18n::Message::RMin : I18n::Message::RMax;
  } else {
    return index == 0 ? I18n::Message::XMin : I18n::Message::XMax;
  }
}

bool DomainParameterController::parametersAreDifferent() {
  return function()->tMin() != m_rangeParam.min() ||
         function()->tMax() != m_rangeParam.max() ||
         function()->tAuto() != m_autoParam;
}

void DomainParameterController::extractParameters() {
  setRange(function()->tMin(), function()->tMax());
  m_autoParam = function()->tAuto();
  /* Setting m_rangeParam should affect tMin and tMax because Function's Range1D
   * parameters are valid (tMax>tMin).
   * Same happens in confirmParameters when setting function's parameters from
   * valid m_rangeParam parameters. */
  assert(!parametersAreDifferent());
}

void DomainParameterController::setAutoRange() {
  assert(m_autoParam);
  setRange(function()->autoTMin(), function()->autoTMax());
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

FloatParameterController<float>::InfinityTolerance
DomainParameterController::infinityAllowanceForRow(int row) const {
  if (function()->properties().isCartesian()) {
    return row == 1 ? FloatParameterController<
                          float>::InfinityTolerance::MinusInfinity
                    : FloatParameterController<
                          float>::InfinityTolerance::PlusInfinity;
  }
  return FloatParameterController<float>::InfinityTolerance::None;
}

Shared::ExpiringPointer<Shared::ContinuousFunction>
DomainParameterController::function() const {
  assert(!m_record.isNull());
  return App::app()->functionStore()->modelForRecord(m_record);
}

Poincare::Layout DomainParameterController::extraCellLayoutAtRow(int row) {
  assert(row == 0);
  return Poincare::Layout::Create(
      KA ^ "∞"_l, {.KA = (m_currentTextFieldIsMinField ? "-"_l : ""_l)});
}

void DomainParameterController::switchToolboxContent(
    Escher::AbstractTextField* textField, bool setSpecificContent) {
  assert(textField == m_boundsCells[0].textField() ||
         textField == m_boundsCells[1].textField());
  if (setSpecificContent && function()->properties().isCartesian()) {
    m_currentTextFieldIsMinField = (textField == m_boundsCells[0].textField());
    App::app()->defaultToolbox()->setExtraCellsDataSource(this);
  } else {
    App::app()->defaultToolbox()->setExtraCellsDataSource(nullptr);
  }
}

}  // namespace Graph
