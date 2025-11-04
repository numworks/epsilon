#include "input_controller.h"

#include <apps/shared/float_parameter_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <poincare/print.h>
#include <poincare/statistics/inference.h>

#include "inference/app.h"
#include "inference/constants.h"
#include "inference/controllers/controller_container.h"
#include "inference/controllers/inference_controller.h"
#include "inference/text_helpers.h"
#include "results_controller.h"

using namespace Escher;
using namespace Inference;

InputController::InputController(Escher::StackViewController* parent,
                                 ControllerContainer* controllerContainer,
                                 InferenceModel* inference)
    : InferenceController(inference, controllerContainer),
      FloatParameterController<double>(parent, &m_messageView),
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

void InputController::initCell(ParameterCell* cell) {
  cell->setParentResponder(&m_selectableListView);
  cell->setDelegate(this);
}

static bool shouldDisplaySignificanceLevelForANOVA(
    const StackViewController* stackViewController) {
  /* This is a sort of hack knowing the order of pages for the ANOVA test. The
   * first controller which has the SameAsPreviousPage property is
   * InputANOVAController, it should not display the significance level. The
   * second controller with SameAsPreviousPage is ResultsANOVAController and it
   * should display the significance level (which was set in
   * InputANOVAController). */
  return (stackViewController->topViewController()->titlesDisplay() ==
          Escher::ViewController::TitlesDisplay::SameAsPreviousPage) &&
         !(stackViewController->secondTopViewController()->titlesDisplay() ==
           Escher::ViewController::TitlesDisplay::SameAsPreviousPage);
}

/* TODO: This method probably belongs somewhere else. It should be made easier
 * to know whether to display the significance level in the title, without using
 * complicated conditions as in shouldDisplaySignificanceLevelForANOVA. */
void InputController::InputTitle(const Escher::ViewController* vc,
                                 const InferenceModel* inference,
                                 char* titleBuffer, size_t titleBufferSize) {
  if (inference->hasHypothesisParameters()) {
    assert(inference->subApp() == SubApp::SignificanceTest);
    const SignificanceTest* signifTest =
        static_cast<const SignificanceTest*>(inference);
    constexpr int k_maxNumberOfGlyphs =
        Escher::Metric::MaxNumberOfSmallGlyphsInDisplayWidth;
    StackViewController* stackViewControllerResponder =
        static_cast<StackViewController*>(vc->parentResponder());
    bool shouldHideSignificanceLevel =
        (stackViewControllerResponder->topViewController() == vc) ||
        (signifTest->testType() == Poincare::Inference::TestType::ANOVA &&
         shouldDisplaySignificanceLevelForANOVA(stackViewControllerResponder));
    if (shouldHideSignificanceLevel) {
      Poincare::Print::CustomPrintfWithMaxNumberOfGlyphs(
          titleBuffer, titleBufferSize, k_numberOfTitleSignificantDigits,
          k_maxNumberOfGlyphs, "H0:%s Ha:%s",
          signifTest->h0Description().data(),
          signifTest->hADescription().data());
    } else {
      Poincare::Print::CustomPrintfWithMaxNumberOfGlyphs(
          titleBuffer, titleBufferSize, k_numberOfTitleSignificantDigits,
          k_maxNumberOfGlyphs, "H0:%s Ha:%s α=%s",
          signifTest->h0Description().data(),
          signifTest->hADescription().data(),
          signifTest->thresholdValueDescription().data());
    }
  } else {
    Poincare::Print::CustomPrintf(titleBuffer, titleBufferSize,
                                  I18n::translate(inference->title()),
                                  I18n::translate(I18n::Message::Interval));
  }
}

ViewController::TitlesDisplay InputController::titlesDisplay() const {
  return m_inferenceModel->hasHypothesisParameters()
             ? m_inferenceModel->canChooseDataset()
                   ? TitlesDisplay::DisplayLastAndThirdToLast
                   : TitlesDisplay::DisplayLastTwoTitles
             : TitlesDisplay::DisplayLastTitle;
}

void InputController::initView() {
  createCells();
  FloatParameterController::initView();
}

void InputController::viewWillAppear() {
  m_significanceCell.label()->setMessage(m_inferenceModel->thresholdName());
  m_significanceCell.subLabel()->setMessage(
      m_inferenceModel->thresholdDescription());
  FloatParameterController::viewWillAppear();
}

int InputController::typeAtRow(int row) const {
  if (row == m_inferenceModel->indexOfThreshold()) {
    return k_significanceCellType;
  }
  return FloatParameterController<double>::typeAtRow(row);
}

void InputController::buttonAction() {
  if (!m_inferenceModel->validateInputs()) {
    App::app()->displayWarning(I18n::Message::InvalidInputs);
    return;
  }
  m_inferenceModel->compute();
  stackOpenPage(&m_controllerContainer->m_resultsController);
}

void InputController::fillCellForRow(Escher::HighlightCell* cell, int row) {
  int type = typeAtRow(row);
  if (type == k_parameterCellType) {
    ParameterCell* mCell = static_cast<ParameterCell*>(cell);
    mCell->label()->setLayout(m_inferenceModel->parameterSymbolAtIndex(row));
    mCell->subLabel()->setMessage(
        m_inferenceModel->parameterDefinitionAtIndex(row));
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
  return !m_inferenceModel->hasHypothesisParameters() &&
         popFromStackViewControllerOnLeftEvent(event);
}

bool InputController::setParameterAtIndex(int parameterIndex, double f) {
  if (!m_inferenceModel->authorizedParameterAtIndex(f, parameterIndex)) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  m_inferenceModel->setParameterAtIndex(f, parameterIndex);
  return true;
}

void InputController::createCells() {
  if (DynamicCellsDataSource<ParameterCell>::m_cells == nullptr) {
    DynamicCellsDataSource<ParameterCell>::createCellsWithOffset(
        InputControllerDimensions::k_maxNumberOfParameterCell, 0);
  }
}
