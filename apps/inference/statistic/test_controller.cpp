#include "test_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>

#include "chi_square_and_slope/categorical_type_controller.h"
#include "test/hypothesis_controller.h"
#include "inference/app.h"
#include "type_controller.h"

using namespace Inference;

TestController::TestController(Escher::StackViewController * parentResponder,
                               HypothesisController * hypothesisController,
                               TypeController * typeController,
                               CategoricalTypeController * categoricalController,
                               InputController * inputController,
                               Statistic * statistic) :
      Escher::SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage, Statistic::k_numberOfSignificanceTestType, Escher::MemoizedListViewDataSource>(parentResponder),
      m_hypothesisController(hypothesisController),
      m_typeController(typeController),
      m_inputController(inputController),
      m_categoricalController(categoricalController),
      m_statistic(statistic) {
  // Create cells
  cellAtIndex(k_indexOfOneProp)->setMessage(I18n::Message::TestOneProp);
  cellAtIndex(k_indexOfOneMean)->setMessage(I18n::Message::TestOneMean);
  cellAtIndex(k_indexOfTwoProps)->setMessage(I18n::Message::TestTwoProps);
  cellAtIndex(k_indexOfTwoMeans)->setMessage(I18n::Message::TestTwoMeans);
  cellAtIndex(k_indexOfCategorical)->setMessage(I18n::Message::TestCategorical);
  cellAtIndex(k_indexOfCategorical)->setSubtitle(I18n::Message::X2Test);

  // Init selection
  selectRow(0);
}

const char * TestController::title() {
  return I18n::translate(m_statistic->statisticTitle());
}

void TestController::stackOpenPage(Escher::ViewController * nextPage) {
  selectRow(static_cast<int>(m_statistic->significanceTestType()));
  ViewController::stackOpenPage(nextPage);
}

void TestController::viewWillAppear() {
  Escher::SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage, Statistic::k_numberOfSignificanceTestType, Escher::MemoizedListViewDataSource>::viewWillAppear();
  // Create cells whose legends vary
  cellAtIndex(k_indexOfOneProp)->setSubtitle(m_statistic->zStatisticMessage());
  cellAtIndex(k_indexOfOneMean)->setSubtitle(m_statistic->tOrZStatisticMessage());
  cellAtIndex(k_indexOfTwoProps)->setSubtitle(m_statistic->zStatisticMessage());
  cellAtIndex(k_indexOfTwoMeans)->setSubtitle(m_statistic->tOrZStatisticMessage());
}

void TestController::didBecomeFirstResponder() {
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
  resetMemoization();
  m_selectableTableView.reloadData();
}

bool TestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    Escher::SelectableViewController * controller = nullptr;
    SignificanceTestType testType;
    switch (selectedRow()) {
      case k_indexOfOneProp:
        testType = SignificanceTestType::OneProportion;
        controller = m_inputController;
        if (m_statistic->hasHypothesisParameters()) {
          controller = m_hypothesisController;
        }
        break;
      case k_indexOfTwoProps:
        testType = SignificanceTestType::TwoProportions;
        controller = m_inputController;
        if (m_statistic->hasHypothesisParameters()) {
          controller = m_hypothesisController;
        }
        break;
      case k_indexOfOneMean:
        testType = SignificanceTestType::OneMean;
        controller = m_typeController;
        break;
      case k_indexOfTwoMeans:
        testType = SignificanceTestType::TwoMeans;
        controller = m_typeController;
        break;
      default:
        assert(selectedRow() == k_indexOfCategorical);
        testType = SignificanceTestType::Categorical;
        controller = m_categoricalController;
        break;
    }
    assert(controller != nullptr);
    if (m_statistic->initializeSignificanceTest(testType)) {
      controller->selectRow(0);
    }
    stackOpenPage(controller);
    return true;
  }
  return popFromStackViewControllerOnLeftEvent(event);
}

int TestController::numberOfRows() const {
  return m_statistic->numberOfSignificancesTestTypes();
}
