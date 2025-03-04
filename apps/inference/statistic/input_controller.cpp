#include "input_controller.h"

#include <apps/shared/float_parameter_controller.h>
#include <escher/stack_view_controller.h>
#include <poincare/print.h>
#include <poincare/statistics/inference.h>

#include "inference/app.h"
#include "inference/constants.h"
#include "inference/text_helpers.h"
#include "results_controller.h"

using namespace Escher;
using namespace Inference;

InputController::InputController(Escher::StackViewController* parent,
                                 ResultsController* resultsController,
                                 Statistic* statistic)
    : FloatParameterController<double>(parent, &m_messageView),
      DynamicCellsDataSource<ParameterCell, k_maxNumberOfParameterCell>(this),
      m_statistic(statistic),
      m_resultsController(resultsController),
      m_significanceCell(&m_selectableListView, this),
      m_messageView(I18n::Message::InputStatistics, k_messageFormat) {
  m_okButton.setMessage(I18n::Message::Next);
}

KDCoordinate InputController::nonMemoizedRowHeight(int row) {
  int type = typeAtRow(row);
  if (type == k_parameterCellType) {
    ParameterCell tempCell;
    return protectedNonMemoizedRowHeight(&tempCell, row);
  }
  if (type == k_significanceCellType) {
    return m_significanceCell.minimalSizeForOptimalDisplay().height();
  }
  return Shared::FloatParameterController<double>::nonMemoizedRowHeight(row);
}

void InputController::initCell(ParameterCell, void* cell, int index) {
  ParameterCell* c = static_cast<ParameterCell*>(cell);
  c->setParentResponder(&m_selectableListView);
  c->setDelegate(this);
}

void InputController::InputTitle(const Escher::ViewController* vc,
                                 const Statistic* statistic, char* titleBuffer,
                                 size_t titleBufferSize) {
  if (statistic->hasHypothesisParameters()) {
    assert(statistic->subApp() == PcrInference::Method::SignificanceTest);
    const Test* signifTest = static_cast<const Test*>(statistic);
    /* H0:<first symbol>=<firstParam>
     * Ha:<first symbol><operator symbol><firstParams>
     * α=<threshold> */
    const char* symbol = signifTest->hypothesisSymbol();
    const char* op = Poincare::ComparisonJunior::OperatorString(
        signifTest->hypothesis()->m_alternative);
    StackViewController* stackViewControllerResponder =
        static_cast<StackViewController*>(vc->parentResponder());
    constexpr int k_maxNumberOfGlyphs =
        Escher::Metric::MaxNumberOfSmallGlyphsInDisplayWidth;
    if (stackViewControllerResponder->topViewController() != vc) {
      Poincare::Print::CustomPrintfWithMaxNumberOfGlyphs(
          titleBuffer, titleBufferSize, k_numberOfTitleSignificantDigits,
          k_maxNumberOfGlyphs, "H0:%s=%*.*ed Ha:%s%s%*.*ed α=%*.*ed", symbol,
          signifTest->hypothesis()->m_h0,
          Poincare::Preferences::PrintFloatMode::Decimal, symbol, op,
          signifTest->hypothesis()->m_h0,
          Poincare::Preferences::PrintFloatMode::Decimal,
          signifTest->threshold(),
          Poincare::Preferences::PrintFloatMode::Decimal);
    } else {
      Poincare::Print::CustomPrintfWithMaxNumberOfGlyphs(
          titleBuffer, titleBufferSize, k_numberOfTitleSignificantDigits,
          k_maxNumberOfGlyphs, "H0:%s=%*.*ed Ha:%s%s%*.*ed", symbol,
          signifTest->hypothesis()->m_h0,
          Poincare::Preferences::PrintFloatMode::Decimal, symbol, op,
          signifTest->hypothesis()->m_h0,
          Poincare::Preferences::PrintFloatMode::Decimal);
    }
  } else {
    Poincare::Print::CustomPrintf(titleBuffer, titleBufferSize,
                                  I18n::translate(statistic->title()),
                                  I18n::translate(I18n::Message::Interval));
  }
}

ViewController::TitlesDisplay InputController::titlesDisplay() const {
  return m_statistic->hasHypothesisParameters()
             ? m_statistic->canChooseDataset()
                   ? TitlesDisplay::DisplayLastAndThirdToLast
                   : TitlesDisplay::DisplayLastTwoTitles
             : TitlesDisplay::DisplayLastTitle;
}

void InputController::initView() {
  createCells();
  FloatParameterController::initView();
}

void InputController::viewWillAppear() {
  m_significanceCell.label()->setMessage(m_statistic->thresholdName());
  m_significanceCell.subLabel()->setMessage(
      m_statistic->thresholdDescription());
  FloatParameterController::viewWillAppear();
}

int InputController::typeAtRow(int row) const {
  if (row == m_statistic->indexOfThreshold()) {
    return k_significanceCellType;
  }
  return FloatParameterController<double>::typeAtRow(row);
}

void InputController::buttonAction() {
  if (!m_statistic->validateInputs()) {
    App::app()->displayWarning(I18n::Message::InvalidInputs);
    return;
  }
  m_statistic->compute();
  stackOpenPage(m_resultsController);
}

void InputController::fillCellForRow(Escher::HighlightCell* cell, int row) {
  int type = typeAtRow(row);
  if (type == k_parameterCellType) {
    ParameterCell* mCell = static_cast<ParameterCell*>(cell);
    mCell->label()->setLayout(m_statistic->parameterSymbolAtIndex(row));
    mCell->subLabel()->setMessage(m_statistic->parameterDefinitionAtIndex(row));
  }
  FloatParameterController<double>::fillCellForRow(cell, row);
}

KDCoordinate InputController::separatorBeforeRow(int row) const {
  return typeAtRow(row) == k_significanceCellType
             ? k_defaultRowSeparator
             : FloatParameterController<double>::separatorBeforeRow(row);
}

int InputController::reusableParameterCellCount(int type) const {
  if (type == k_parameterCellType) {
    return k_numberOfReusableCells;
  }
  assert(type == k_significanceCellType);
  return 1;
}

HighlightCell* InputController::reusableParameterCell(int index, int type) {
  if (type == k_parameterCellType) {
    assert(index >= 0 && index < k_numberOfReusableCells);
    return cell(index);
  }
  assert(type == k_significanceCellType);
  return &m_significanceCell;
}

TextField* InputController::textFieldOfCellAtIndex(HighlightCell* cell,
                                                   int index) {
  if (typeAtRow(index) == k_significanceCellType) {
    assert(cell == &m_significanceCell);
    return m_significanceCell.textField();
  }
  assert(typeAtRow(index) == k_parameterCellType);
  return static_cast<ParameterCell*>(cell)->textField();
}

bool InputController::handleEvent(Ion::Events::Event event) {
  /* If the previous controller was the hypothesis controller, the pop on Left
   * event is unable. */
  return !m_statistic->hasHypothesisParameters() &&
         popFromStackViewControllerOnLeftEvent(event);
}

bool InputController::setParameterAtIndex(int parameterIndex, double f) {
  if (!m_statistic->authorizedParameterAtIndex(f, parameterIndex)) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  m_statistic->setParameterAtIndex(f, parameterIndex);
  return true;
}
