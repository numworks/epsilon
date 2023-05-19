#include "results_controller.h"

#include <apps/i18n.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/invocation.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>
#include <poincare/print.h>

#include "inference/app.h"
#include "inference/text_helpers.h"

using namespace Escher;

namespace Inference {

ResultsController::ResultsController(
    StackViewController *parent, Statistic *statistic,
    TestGraphController *testGraphController,
    IntervalGraphController *intervalGraphController,
    InputEventHandlerDelegate *handler, TextFieldDelegate *textFieldDelegate)
    : ViewController(parent),
      MemoizedListViewDataSource(),
      DynamicCellsDataSource<ResultCell, k_maxNumberOfResultCells>(this),
      m_selectableListView(this, this, this),
      m_title(I18n::Message::CalculatedValues,
              {.style = {.glyphColor = Palette::GrayDark,
                         .backgroundColor = Palette::WallScreen,
                         .font = KDFont::Size::Small},
               .horizontalAlignment = KDGlyph::k_alignCenter}),
      m_contentView(&m_selectableListView, this, &m_title),
      m_statistic(statistic),
      m_testGraphController(testGraphController),
      m_intervalGraphController(intervalGraphController),
      m_next(&m_selectableListView, I18n::Message::Next,
             Invocation::Builder<ResultsController>(
                 &ResultsController::ButtonAction, this)) {}

void ResultsController::didBecomeFirstResponder() {
  selectCell(0);
  Container::activeApp()->setFirstResponder(&m_selectableListView);
  resetMemoization();
  m_contentView.reload();
}

ViewController::TitlesDisplay ResultsController::titlesDisplay() {
  if (m_statistic->subApp() == Statistic::SubApp::Interval ||
      (m_statistic->significanceTestType() ==
           SignificanceTestType::Categorical &&
       m_statistic->categoricalType() == CategoricalType::GoodnessOfFit)) {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  return ViewController::TitlesDisplay::DisplayLastThreeTitles;
}

const char *ResultsController::title() {
  m_titleBuffer[0] = 0;
  StackViewController *stackViewControllerResponder =
      static_cast<StackViewController *>(parentResponder());
  bool resultsIsTopPage =
      stackViewControllerResponder->topViewController() != this;
  if (resultsIsTopPage &&
      m_statistic->subApp() == Statistic::SubApp::Interval) {
    m_intervalGraphController->setResultTitleForCurrentValues(
        m_titleBuffer, sizeof(m_titleBuffer), resultsIsTopPage);
  } else {
    m_statistic->setResultTitle(m_titleBuffer, sizeof(m_titleBuffer),
                                resultsIsTopPage);
  }
  if (m_titleBuffer[0] == 0) {
    return nullptr;
  }
  return m_titleBuffer;
}

bool ResultsController::ButtonAction(ResultsController *controller, void *s) {
  if (!controller->m_statistic->isGraphable()) {
    App::app()->displayWarning(I18n::Message::InvalidValues);
    return false;
  }
  ViewController *graph;
  if (controller->m_statistic->subApp() == Statistic::SubApp::Test) {
    graph = controller->m_testGraphController;
  } else {
    graph = controller->m_intervalGraphController;
  }
  controller->stackOpenPage(graph);
  return true;
}

int ResultsController::numberOfRows() const {
  return m_statistic->numberOfResults() + 1 /* button */;
}

KDCoordinate ResultsController::defaultColumnWidth() {
  return Ion::Display::Width - Metric::CommonLeftMargin -
         Metric::CommonRightMargin;
}

void ResultsController::willDisplayCellForIndex(HighlightCell *cell, int i) {
  if (i < numberOfRows() - 1) {
    ResultCell *messageCell = static_cast<ResultCell *>(cell);
    double value;
    Poincare::Layout message;
    I18n::Message subMessage;
    int precision = Poincare::Preferences::VeryLargeNumberOfSignificantDigits;
    m_statistic->resultAtIndex(i, &value, &message, &subMessage, &precision);
    constexpr int bufferSize = Constants::k_largeBufferSize;
    char buffer[bufferSize];
    Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode(
        value, buffer, bufferSize, precision,
        Poincare::Preferences::PrintFloatMode::Decimal);

    messageCell->label()->setLayout(message);
    messageCell->subLabel()->setMessage(subMessage);
    messageCell->accessory()->setText(buffer);
  }
}

HighlightCell *ResultsController::reusableCell(int index, int type) {
  if (type == k_resultCellType) {
    return cell(index);
  }
  return &m_next;
}

int ResultsController::reusableCellCount(int type) {
  if (type == k_resultCellType) {
    return k_numberOfReusableCells;
  }
  return 1;
}

int ResultsController::typeAtIndex(int index) const {
  if (index == numberOfRows() - 1) {
    return k_buttonCellType;
  }
  return k_resultCellType;
}

}  // namespace Inference
