#include "hypothesis_controller.h"

#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <escher/container.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/invocation.h>
#include <escher/palette.h>
#include <escher/responder.h>
#include <escher/stack_view_controller.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>

#include "input_controller.h"
#include "probability/app.h"
#include "probability/data.h"
#include "probability/helpers.h"

using namespace Probability;

HypothesisController::HypothesisController(Escher::StackViewController * parent,
                                           NormalInputController * inputController,
                                           InputEventHandlerDelegate * handler,
                                           TextFieldDelegate * textFieldDelegate)
    : SelectableListViewPage(parent),
      m_inputController(inputController),
      m_h0(&m_selectableTableView, handler, textFieldDelegate),
      m_ha(&m_selectableTableView, handler, textFieldDelegate),
      m_next(&m_selectableTableView, I18n::Message::Ok, buttonActionInvocation()) {
  m_h0.setMessage(I18n::Message::H0);
  m_ha.setMessage(I18n::Message::Ha);
}

const char * Probability::HypothesisController::title() {
  int bufferSize = sizeof(m_titleBuffer);
  int written =
      testTypeToText(App::app()->snapshot()->data()->testType(), m_titleBuffer, bufferSize);
  const char on[] = " on ";
  memcpy(m_titleBuffer + written - 1, on, sizeof(on));
  written += sizeof(on) - 1;
  written += testToText(App::app()->snapshot()->data()->test(), m_titleBuffer + written - 1,
                        bufferSize - written - 1);
  assert(written < bufferSize);
  return m_titleBuffer;
}

HighlightCell * HypothesisController::reusableCell(int i, int type) {
  switch (i) {
    case k_indexOfH0:
      return &m_h0;
    case k_indexOfHa:
      return &m_ha;
    case k_indexOfNext:
      return &m_next;
    default:
      assert(false);
  }
}

void HypothesisController::didBecomeFirstResponder() {
  Probability::App::app()->snapshot()->navigation()->setPage(Data::Page::Hypothesis);
  // TODO factor out
  selectCellAtLocation(0, 0);
  loadHypothesisParam();
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void HypothesisController::buttonAction() {
  storeHypothesisParams();
  openPage(m_inputController, false, Escher::Palette::GrayDark, Escher::Palette::GrayDark);
}

void HypothesisController::loadHypothesisParam() {
  constexpr int bufferSize = 20;
  char buffer[bufferSize]{"<"};
  float p = App::app()->snapshot()->data()->hypothesisParams()->firstParam();
  Poincare::PrintFloat::ConvertFloatToText(p, buffer + 1, bufferSize, k_maxInputLength, 5,
                                           Poincare::Preferences::PrintFloatMode::Decimal);
  m_h0.setAccessoryText(buffer + 1);
  m_ha.setAccessoryText(buffer);
}

void HypothesisController::storeHypothesisParams() {
  // TODO maybe parse at the same time as when checking validity
  Data::HypothesisParams * params = App::app()->snapshot()->data()->hypothesisParams();
  constexpr int precision = Poincare::Preferences::MediumNumberOfSignificantDigits;
  constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(precision);
  char buffer[bufferSize];
  Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode(
      params->firstParam(), buffer, bufferSize, precision,
      Poincare::Preferences::PrintFloatMode::Decimal);

  params->setOp(Data::ComparisonOperator::Lower);
}
