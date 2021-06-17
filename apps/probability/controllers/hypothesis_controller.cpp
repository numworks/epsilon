#include "hypothesis_controller.h"

#include <apps/apps_container.h>
#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <escher/container.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/invocation.h>
#include <escher/responder.h>
#include <escher/stack_view_controller.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <string.h>

#include <new>

#include "input_controller.h"
#include "probability/app.h"
#include "probability/models/data.h"
#include "probability/text_helpers.h"

using namespace Probability;

HypothesisController::HypothesisController(Escher::StackViewController * parent,
                                           InputController * inputController,
                                           InputEventHandlerDelegate * handler,
                                           HypothesisParams * hypothesisParams) :
    SelectableListViewPage(parent),
    m_inputController(inputController),
    m_h0(&m_selectableTableView, handler, this),
    m_ha(&m_selectableTableView, handler, this),
    m_next(&m_selectableTableView, I18n::Message::Ok, buttonActionInvocation()),
    m_hypothesisParams(hypothesisParams) {
  m_h0.setMessage(I18n::Message::H0);
  m_ha.setMessage(I18n::Message::Ha);
}

const char * Probability::HypothesisController::title() {
  const char * testType = testTypeToText(App::app()->testType());
  const char * test = testToText(App::app()->test());
  sprintf(m_titleBuffer, "%s on %s", testType, test);
  return m_titleBuffer;
}

bool Probability::HypothesisController::textFieldShouldFinishEditing(Escher::TextField * textField,
                                                                     Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (textField == m_h0.textField()) {
      return true;
    }
    // Dummy parsing to check for operator in editable field
    const char * txt = textField->text();
    const char ops[3] = {'<', '=', '>'};
    for (int i = 0; i < 3; i++) {
      if (strchr(txt, ops[i]) != NULL) {
        return true;
      }
    }
    return false;
  }
  return false;
}

bool Probability::HypothesisController::textFieldDidFinishEditing(Escher::TextField * textField,
                                                                  const char * text,
                                                                  Ion::Events::Event event) {
  if (textField == m_h0.textField()) {
    float h0;
    if (textFieldDelegateApp()->hasUndefinedValue(text, h0, false, false)) {
      return false;
    }
    // TODO could check validity
    m_hypothesisParams->setFirstParam(h0);
    loadHypothesisParam(false);
    return true;
  }
  HypothesisParams::ComparisonOperator ops[3] = {HypothesisParams::ComparisonOperator::Lower,
                                                 HypothesisParams::ComparisonOperator::Different,
                                                 HypothesisParams::ComparisonOperator::Higher};
  for (int i = 0; i < 3; i++) {
    if (strchr(text, static_cast<char>(ops[i])) != NULL) {
      m_hypothesisParams->setOp(ops[i]);
      m_ha.setAccessoryText(text);
      return true;
    }
  }
  return false;
}

HighlightCell * HypothesisController::reusableCell(int i, int type) {
  switch (i) {
    case k_indexOfH0:
      return &m_h0;
    case k_indexOfHa:
      return &m_ha;
    default:
      assert(i == k_indexOfNext);
      return &m_next;
  }
}

void HypothesisController::didBecomeFirstResponder() {
  App::app()->setPage(Data::Page::Hypothesis);
  // TODO factor out
  selectCellAtLocation(0, 0);
  loadHypothesisParam();
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void HypothesisController::buttonAction() {
  storeHypothesisParams();
  openPage(m_inputController);
}

void HypothesisController::loadHypothesisParam(bool h0Only) {
  constexpr int bufferSize = 20;
  char buffer[bufferSize]{0};
  const char * symbol = testToTextSymbol(App::app()->test());
  const char op = static_cast<const char>(m_hypothesisParams->op());
  int written = sprintf(buffer, "%s%c", symbol, op);
  float p = m_hypothesisParams->firstParam();
  Poincare::PrintFloat::ConvertFloatToText(p, buffer + written, bufferSize, k_maxInputLength, 5,
                                           Poincare::Preferences::PrintFloatMode::Decimal);
  m_ha.setAccessoryText(buffer);
  if (!h0Only) {
    m_h0.setAccessoryText(buffer + written);
  }
}

void HypothesisController::storeHypothesisParams() {
  // TODO maybe parse at the same time as when checking validity
  // TODO use TextFieldDelegateApp
  m_hypothesisParams->setFirstParam(Shared::PoincareHelpers::ApproximateToScalar<float>(
      m_h0.textField()->text(), AppsContainer::sharedAppsContainer()->globalContext()));
  // params->setOp(HypothesisParams::ComparisonOperator::Higher);
}
