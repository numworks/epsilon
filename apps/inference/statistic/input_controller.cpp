#include "input_controller.h"

#include <apps/shared/float_parameter_controller.h>
#include <escher/stack_view_controller.h>
#include <poincare/print.h>

#include "inference/app.h"
#include "inference/constants.h"
#include "inference/text_helpers.h"
#include "results_controller.h"

using namespace Escher;
using namespace Inference;

InputController::InputController(Escher::StackViewController * parent,
                                 ResultsController * resultsController,
                                 Statistic * statistic,
                                 Escher::InputEventHandlerDelegate * handler) :
      FloatParameterController<double>(parent),
      DynamicCellsDataSource<ExpressionCellWithEditableTextWithMessage, k_maxNumberOfExpressionCellsWithEditableTextWithMessage>(this),
      m_statistic(statistic),
      m_resultsController(resultsController)
{
  m_okButton.setMessage(I18n::Message::Next);
  // Initialize cells
  m_significanceCell.setParentResponder(&m_selectableTableView);
  m_significanceCell.innerCell()->setDelegates(handler, this);
  m_significanceCell.innerCell()->setMessage(I18n::Message::Alpha);
  m_significanceCell.innerCell()->setSubLabelMessage(I18n::Message::SignificanceLevel);
}

void InputController::initCell(ExpressionCellWithEditableTextWithMessage, void * cell, int index) {
  ExpressionCellWithEditableTextWithMessage * c = static_cast<ExpressionCellWithEditableTextWithMessage *>(cell);
  c->setParentResponder(&m_selectableTableView);
  c->setDelegates(App::app(), this);
}

void InputController::InputTitle(Escher::ViewController * vc, Statistic * statistic, char * titleBuffer, size_t titleBufferSize) {
  if (statistic->hasHypothesisParameters()) {
    // H0:<first symbol>=<firstParam> Ha:<first symbol><operator symbol><firstParams> α=<threshold>
    const char * symbol = statistic->hypothesisSymbol();
    const char * op = HypothesisParams::strForComparisonOp(statistic->hypothesisParams()->comparisonOperator());
    StackViewController * stackViewControllerResponder = static_cast<StackViewController *>(vc->parentResponder());
    if (stackViewControllerResponder->topViewController() != vc) {
      Poincare::Print::CustomPrintf(titleBuffer, titleBufferSize, "H0:%s=%*.*ed Ha:%s%s%*.*ed α=%*.*ed",
          symbol,
          statistic->hypothesisParams()->firstParam(), Poincare::Preferences::PrintFloatMode::Decimal, k_numberOfTitleSignificantDigits,
          symbol,
          op,
          statistic->hypothesisParams()->firstParam(), Poincare::Preferences::PrintFloatMode::Decimal, k_numberOfTitleSignificantDigits,
          statistic->threshold(), Poincare::Preferences::PrintFloatMode::Decimal, k_numberOfTitleSignificantDigits);
    } else {
      Poincare::Print::CustomPrintf(titleBuffer, titleBufferSize, "H0:%s=%*.*ed Ha:%s%s%*.*ed",
          symbol,
          statistic->hypothesisParams()->firstParam(), Poincare::Preferences::PrintFloatMode::Decimal, k_numberOfTitleSignificantDigits,
          symbol,
          op,
          statistic->hypothesisParams()->firstParam(), Poincare::Preferences::PrintFloatMode::Decimal, k_numberOfTitleSignificantDigits);
    }
  } else {
    Poincare::Print::CustomPrintf(titleBuffer, titleBufferSize, I18n::translate(statistic->title()),
        I18n::translate(I18n::Message::Interval));
  }
}

ViewController::TitlesDisplay InputController::titlesDisplay() {
  if (m_statistic->hasHypothesisParameters()) {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  return ViewController::TitlesDisplay::DisplayLastTitle;
}

int InputController::typeAtIndex(int i) const {
  if (i == m_statistic->indexOfThreshold()) {
    return k_significanceCellType;
  }
  return FloatParameterController<double>::typeAtIndex(i);
}

void InputController::didBecomeFirstResponder() {
  selectCellAtLocation(0, 0);
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void InputController::buttonAction() {
  if (!m_statistic->validateInputs()) {
    App::app()->displayWarning(I18n::Message::InvalidInputs);
    return;
  }
  m_statistic->compute();
  stackOpenPage(m_resultsController);
}

void InputController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  if (index < m_statistic->indexOfThreshold()) {
    ExpressionCellWithEditableTextWithMessage * mCell =
        static_cast<ExpressionCellWithEditableTextWithMessage *>(cell);
    mCell->setLayout(m_statistic->parameterSymbolAtIndex(index));
    mCell->setSubLabelMessage(m_statistic->parameterDefinitionAtIndex(index));
  } else if (index == m_statistic->indexOfThreshold()) {
    MessageTableCellWithSeparator * thresholdCell = static_cast<MessageTableCellWithSeparator *>(cell);
    I18n::Message name, description;
    name = m_statistic->thresholdName();
    description = m_statistic->thresholdDescription();
    thresholdCell->innerCell()->setMessage(name);
    thresholdCell->innerCell()->setSubLabelMessage(description);
  }
  FloatParameterController<double>::willDisplayCellForIndex(cell, index);
}

int InputController::reusableParameterCellCount(int type) {
  if (type == k_parameterCellType) {
    return k_numberOfReusableCells;
  }
  assert(type == k_significanceCellType);
  return 1;
}

Escher::HighlightCell * InputController::reusableParameterCell(int index, int type) {
  if (type == k_parameterCellType) {
    assert(index >= 0 && index < k_numberOfReusableCells);
    return cell(index);
  }
  assert(type == k_significanceCellType);
  return &m_significanceCell;
}

bool InputController::handleEvent(Ion::Events::Event event) {
  // If the previous controller was the hypothesis controller, the pop on Left event is unable.
  return !m_statistic->hasHypothesisParameters() && popFromStackViewControllerOnLeftEvent(event);
}

bool InputController::isCellEditing(Escher::HighlightCell * cell, int index) {
  if (index == m_statistic->indexOfThreshold()) {
    return static_cast<MessageTableCellWithSeparator *>(cell)
        ->innerCell()
        ->textField()
        ->isEditing();
  }
  return static_cast<ExpressionCellWithEditableTextWithMessage *>(cell)->textField()->isEditing();
}

void InputController::setTextInCell(Escher::HighlightCell * cell,
                                                 const char * text,
                                                 int index) {
  if (index == m_statistic->indexOfThreshold()) {
    static_cast<MessageTableCellWithSeparator *>(cell)->innerCell()->textField()->setText(text);
  } else {
    static_cast<ExpressionCellWithEditableTextWithMessage *>(cell)->textField()->setText(text);
  }
}

bool InputController::setParameterAtIndex(int parameterIndex, double f) {
  if (!m_statistic->authorizedParameterAtIndex(f, parameterIndex)) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  m_statistic->setParameterAtIndex(f, parameterIndex);
  return true;
}
