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
#include <shared/poincare_helpers.h>
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
                                           Statistic * statistic) :
    SelectableListViewPage(parent),
    m_inputController(inputController),
    m_operatorDataSource(statistic->hypothesisParams()),
    m_h0(&m_selectableTableView, handler, this),
    m_ha(&m_selectableTableView, &m_operatorDataSource, this),
    m_next(&m_selectableTableView, I18n::Message::Ok, buttonActionInvocation()),
    m_statistic(statistic) {
  m_h0.setMessage(I18n::Message::H0);
  m_h0.setSubLabelMessage(I18n::Message::H0Sub);
  m_ha.setMessage(I18n::Message::Ha);
  m_ha.setSubLabelMessage(I18n::Message::HaSub);
}

const char * Probability::HypothesisController::title() {
  const char * testType = testTypeToText(App::app()->testType());
  const char * test = testToText(App::app()->test());
  snprintf(m_titleBuffer, sizeof(m_titleBuffer), "%s-test on %s", testType, test);
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
  float h0 = Shared::PoincareHelpers::ApproximateToScalar<float>(
      text,
      AppsContainer::sharedAppsContainer()->globalContext());
  // Check
  if (std::isnan(h0) || !m_statistic->isValidH0(h0)) {
    App::app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }

  m_statistic->hypothesisParams()->setFirstParam(h0);
  loadHypothesisParam();
  return true;
}

void Probability::HypothesisController::onDropdownSelected(int selectedRow) {
  HypothesisParams::ComparisonOperator op;
  switch (selectedRow) {
    case 0:
      op = HypothesisParams::ComparisonOperator::Lower;
      break;
    case 1:
      op = HypothesisParams::ComparisonOperator::Different;
      break;
    case 2:
      op = HypothesisParams::ComparisonOperator::Higher;
      break;
  }
  m_statistic->hypothesisParams()->setOp(op);
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
  m_ha.dropdown()->init();
  loadHypothesisParam();
  resetMemoization();
  m_selectableTableView.reloadData(true);
}

void HypothesisController::buttonAction() {
  openPage(m_inputController);
}

void HypothesisController::loadHypothesisParam() {
  constexpr int bufferSize = 20;
  char buffer[bufferSize];
  float p = m_statistic->hypothesisParams()->firstParam();
  defaultParseFloat(p, buffer, bufferSize);
  m_h0.setAccessoryText(buffer);
  m_ha.reload();
  resetMemoization();  // TODO only when m_ha changes size ?
  m_selectableTableView.reloadData();
}
