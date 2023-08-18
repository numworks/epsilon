#include "domain_parameter_controller.h"

#include <assert.h>
#include <poincare/infinity.h>

#include "../app.h"

using namespace Shared;
using namespace Escher;
using namespace Poincare;

namespace Graph {

DomainParameterController::DomainParameterController(Responder* parentResponder)
    : Shared::SingleRangeController(parentResponder, &m_confirmPopUpController),
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
  /* Do not refuse empty text for functions of x, as that will later be
   * replaced by ±inf. */
  return !(function()->properties().isCartesian() &&
           textFieldShouldFinishEditing(textField, event) &&
           textField->text()[0] == '\0') &&
         FloatParameterController<float>::textFieldDidReceiveEvent(textField,
                                                                   event);
}

bool DomainParameterController::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  switchToolboxContent(textField, false);
  assert(!textField->isEditing());
  textField->setEditing(true);  // To edit draft text buffer in setText
  if (textField->draftText()[0] == '\0') {
    if (textField == m_boundsCells[0].textField()) {
      textField->setText(Infinity::Name(true));
    } else {
      assert(textField == m_boundsCells[1].textField());
      textField->setText(Infinity::Name(false));
    }
  }
  textField->setEditing(false);  // set editing back to previous value
  return SingleRangeController::textFieldDidFinishEditing(textField, event);
}

bool DomainParameterController::textFieldDidAbortEditing(
    AbstractTextField* textField) {
  switchToolboxContent(textField, false);
  return false;
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

void DomainParameterController::setAutoRange() {
  assert(m_autoParam);
  setParameterAtIndex(1, function()->autoTMin());
  setParameterAtIndex(2, function()->autoTMax());
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
  Preferences* pref = Preferences::sharedPreferences;
  return Infinity::Builder(m_currentTextFieldIsMinField)
      .createLayout(pref->displayMode(), pref->numberOfSignificantDigits(),
                    App::app()->localContext());
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
