#include "calculation_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <poincare/preferences.h>
#include <poincare/print.h>

#include <algorithm>
#include <cmath>

#include "../images/calculation1_icon.h"
#include "distributions/app.h"
#include "distributions/constants.h"

using namespace Shared;
using namespace Escher;

namespace Distributions {

constexpr int CalculationController::k_titleBufferSize;

CalculationController::ContentView::ContentView(
    SelectableTableView* selectableTableView, Distribution* distribution,
    Calculation* calculation)
    : m_selectableTableView(selectableTableView),
      m_distributionCurveView(distribution, calculation),
      m_unknownParameterBanner(
          {.style = {.backgroundColor = Palette::WallScreen},
           .horizontalAlignment = KDGlyph::k_alignCenter}) {}

View* CalculationController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  if (index == 0) {
    return m_selectableTableView;
  }
  if (index == 1) {
    return &m_distributionCurveView;
  }
  return &m_unknownParameterBanner;
}

void CalculationController::ContentView::layoutSubviews(bool force) {
  KDSize tableSize = m_selectableTableView->minimalSizeForOptimalDisplay();
  setChildFrame(m_selectableTableView,
                KDRect(0, 0, bounds().width(), tableSize.height()), force);
  KDCoordinate bannerHeight = 0;
  if (m_unknownParameterBanner.text()[0] == 0) {
    setChildFrame(&m_unknownParameterBanner, KDRect(0, 0, 0, 0), force);
  } else {
    bannerHeight = k_bannerHeight;
    setChildFrame(&m_unknownParameterBanner,
                  KDRect(0, bounds().height() - bannerHeight, bounds().width(),
                         bannerHeight),
                  force);
  }
  setChildFrame(&m_distributionCurveView,
                KDRect(0, tableSize.height(), bounds().width(),
                       bounds().height() - tableSize.height() - bannerHeight),
                force);
}

CalculationController::CalculationController(
    StackViewController* parentResponder, Distribution* distribution,
    Calculation* calculation)
    : ViewController(parentResponder),
      m_calculation(calculation),
      m_distribution(distribution),
      m_contentView(&m_selectableTableView, distribution, calculation),
      m_selectableTableView(this),
      m_imagesDataSource(distribution),
      m_dropdown(&m_selectableTableView, &m_imagesDataSource, this) {
  assert(distribution != nullptr);
  assert(calculation != nullptr);
  m_selectableTableView.setMargins(k_tableMargins);
  m_selectableTableView.setVerticalCellOverlap(0);
  m_selectableTableView.hideScrollBars();
  m_selectableTableView.setBackgroundColor(KDColorWhite);

  for (int i = 0; i < k_numberOfCalculationCells; i++) {
    m_calculationCells[i].editableTextCell()->setParentResponder(
        &m_selectableTableView);
    m_calculationCells[i].editableTextCell()->textField()->setDelegate(this);
  }
}

void CalculationController::reinitCalculation() {
  setCalculationAccordingToIndex(0, true);
  m_dropdown.selectRow(0);
}

void CalculationController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    updateTitle();
    m_dropdown.init();
    reload();
  } else {
    Escher::ViewController::handleResponderChainEvent(event);
  }
}

bool CalculationController::handleEvent(Ion::Events::Event event) {
  if (m_selectableTableView.selectedColumn() == 0 &&
      event == Ion::Events::Down) {
    m_dropdown.open();
    return true;
  }
  return false;
}

void CalculationController::viewWillAppear() {
  m_dropdown.selectRow(static_cast<int>(m_calculation->type()));
  updateCells();
  ViewController::viewWillAppear();
  selectCellAtLocation(1, 0);
}

void CalculationController::viewDidDisappear() {
  m_selectableTableView.deselectTable();
  ViewController::viewDidDisappear();
}

int CalculationController::numberOfColumns() const {
  return m_calculation->numberOfParameters() + 1;
}

/* WARNING: we set one type per cell to be able to deduce the column width from
 * the cell minimalSizeForOptimalDisplay. Otherwise, we can not know which cell
 * to interrogate to get the column width and we neither can call
 * tableView->cellAtLocation as this function depends on the
 * numberOfDisplaybleRows which depends on the column width! */

KDCoordinate CalculationController::nonMemoizedColumnWidth(int column) {
  if (column == 0) {
    return m_dropdown.minimalSizeForOptimalDisplay().width();
  }
  /* WARNING: that is possible only because we know which view cell corresponds
   * to which cell */
  return m_calculationCells[column - 1].minimalSizeForOptimalDisplay().width();
}

KDCoordinate CalculationController::defaultRowHeight() {
  return m_dropdown.minimalSizeForOptimalDisplay().height();
}

HighlightCell* CalculationController::reusableCell(int index, int type) {
  assert(index == 0);
  switch (type) {
    case 0:
      return &m_dropdown;
    default:
      return &m_calculationCells[type - 1];
  }
}

void CalculationController::textFieldDidHandleEvent(
    ::AbstractTextField* textField) {
  /* We do not reload the responder because it would setEditing(false)
   * the textField and the input would not be handled properly. */
  m_selectableTableView.reloadData(false);
  /* The textField frame might have increased which forces to reload the
   * textField scroll */
  textField->scrollToCursor();
}

bool CalculationController::textFieldShouldFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  return MathTextFieldDelegate::textFieldShouldFinishEditing(textField,
                                                             event) ||
         (event == Ion::Events::Right &&
          textField->cursorLocation() == textField->draftTextEnd() &&
          selectedColumn() < m_calculation->numberOfParameters()) ||
         (event == Ion::Events::Left &&
          textField->cursorLocation() == textField->text());
}

bool CalculationController::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  assert(selectedColumn() != 0);
  double floatBody = ParseInputFloatValue<double>(textField->draftText());
  if (HasUndefinedValue(floatBody)) {
    return false;
  }
  Calculation::Type calculationType = m_calculation->type();
  int resultColumn =
      calculationType == Calculation::Type::FiniteIntegral ? 3 : 2;
  if (selectedColumn() == resultColumn) {
    if (floatBody < 0.0) {
      floatBody = 0.0;
    }
    if (floatBody > 1.0) {
      floatBody = 1.0;
    }
  } else if (!m_distribution->isContinuous() &&
             floatBody != std::round(floatBody)) {
    assert(selectedColumn() == 1 ||
           (selectedColumn() == 2 &&
            calculationType == Calculation::Type::FiniteIntegral));

    if (calculationType == Calculation::Type::Discrete) {
      floatBody = std::round(floatBody);
    } else if (calculationType == Calculation::Type::LeftIntegral ||
               (calculationType == Calculation::Type::FiniteIntegral &&
                selectedColumn() == 2)) {
      // X <= floatBody is equivalent to X <= floor(floatBody) when discrete
      floatBody = std::floor(floatBody);
    } else {
      assert(calculationType == Calculation::Type::RightIntegral ||
             (calculationType == Calculation::Type::FiniteIntegral &&
              selectedColumn() == 1));
      // X >= floatBody is equivalent to X >= ceil(floatBody) when discrete
      floatBody = std::ceil(floatBody);
    }
  }
  m_calculation->setParameterAtIndex(floatBody, selectedColumn() - 1);
  updateCellsValues();
  if (event == Ion::Events::Right || event == Ion::Events::Left) {
    m_selectableTableView.handleEvent(event);
  }
  reload();
  return true;
}

void CalculationController::reload() {
  m_selectableTableView.reloadData();
  m_contentView.distributionCurveView()->reload();
  if (m_distribution->allParametersAreInitialized()) {
    m_contentView.unknownParameterValue()->setText("");
  } else {
    assert(m_distribution->canHaveUninitializedParameter());
    char buffer[k_titleBufferSize];
    Poincare::Print::CustomPrintf(
        buffer, k_titleBufferSize, k_unknownParameterBannerText,
        m_distribution->parameterNameAtIndex(
            m_distribution->uninitializedParameterIndex()),
        m_distribution->parameterAtIndex(
            m_distribution->uninitializedParameterIndex()),
        Poincare::Preferences::PrintFloatMode::Decimal,
        Poincare::Preferences::SharedPreferences()
            ->numberOfSignificantDigits());
    m_contentView.unknownParameterValue()->setText(buffer);
  }
  m_contentView.reload();
}

void CalculationController::setCalculationAccordingToIndex(
    int index, bool forceReinitialisation) {
  Calculation::Initialize(m_calculation, static_cast<Calculation::Type>(index),
                          m_distribution, forceReinitialisation);
  updateCells();
}

void CalculationController::updateCells() {
  int numberOfColumns = m_calculation->numberOfParameters();
  for (int i = 0; i < numberOfColumns; i++) {
    // Update messages
    m_calculationCells[i].messageTextView()->setMessage(
        m_calculation->legendForParameterAtIndex(i));
    // Update editability
    bool notEditable =
        ((!m_distribution->isSymmetrical() ||
          !m_distribution->isContinuous()) &&
         i == 2) ||
        (m_calculation->type() == Calculation::Type::Discrete && i == 1);
    m_calculationCells[i].setEditable(!notEditable);
  }
  updateCellsValues();
}

void CalculationController::updateCellsValues() {
  int numberOfColumns = m_calculation->numberOfParameters();
  for (int i = 0; i < numberOfColumns; i++) {
    constexpr int bufferSize = Constants::k_largeBufferSize;
    char buffer[bufferSize];
    /* FIXME: It has not been decided yet if we should use the prefered mode
     * instead of always using scientific mode */
    PoincareHelpers::ConvertFloatToTextWithDisplayMode(
        m_calculation->parameterAtIndex(i), buffer, bufferSize,
        Poincare::Preferences::VeryLargeNumberOfSignificantDigits,
        Poincare::Preferences::PrintFloatMode::Decimal);
    m_calculationCells[i].editableTextCell()->textField()->setText(buffer);
  }
}

void CalculationController::onDropdownSelected(int selectedRow) {
  setCalculationAccordingToIndex(m_imagesDataSource.absoluteRow(selectedRow));
  reload();
}

bool CalculationController::popupDidReceiveEvent(Ion::Events::Event event,
                                                 Responder* responder) {
  if (event == Ion::Events::Right) {
    return responder->handleEvent(Ion::Events::OK);
  }
  return false;
}

void CalculationController::updateTitle() {
  int currentChar = 0;
  m_titleBuffer[0] = 0;
  for (int index = 0; index < m_distribution->numberOfParameters(); index++) {
    if (m_distribution->uninitializedParameterIndex() == index) {
      continue;
    }
    currentChar += Poincare::Print::CustomPrintf(
        m_titleBuffer + currentChar, k_titleBufferSize - currentChar,
        k_parameterTitle, m_distribution->parameterNameAtIndex(index),
        m_distribution->parameterAtIndex(index),
        Poincare::Preferences::PrintFloatMode::Decimal,
        Poincare::Preferences::ShortNumberOfSignificantDigits);
  }
}

}  // namespace Distributions
