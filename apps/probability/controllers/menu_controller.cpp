#include "menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell.h>
#include <escher/responder.h>

#include <new>

#include "../images/confidence_interval.h"
#include "../images/probability.h"
#include "../images/significance_test.h"
#include "probability/app.h"

using namespace Probability;

MenuController::MenuController(Escher::StackViewController * parentResponder,
                               DistributionController * distributionController,
                               TestController * testController,
                               Data::Test * globalTest,
                               Data::TestType * globalTestType,
                               Statistic * globalStatistic,
                               Distribution * globalDistribution,
                               Calculation * globalCalculation) :
      Escher::SelectableCellListPage<Escher::SubappCell, k_numberOfCells>(parentResponder),
      m_distributionController(distributionController),
      m_testController(testController),
      m_globalTest(globalTest),
      m_globalTestType(globalTestType),
      m_globalStatistic(globalStatistic),
      m_globalDistribution(globalDistribution),
      m_globalCalculation(globalCalculation),
      m_contentView(&m_selectableTableView) {
  selectRow(0);
  cellAtIndex(k_indexOfProbability)->setMessages(I18n::Message::ProbaApp, I18n::Message::ProbaDescr);
  cellAtIndex(k_indexOfProbability)->setImage(ImageStore::Probability);
  cellAtIndex(k_indexOfTest)->setImage(ImageStore::SignificanceTest);
  cellAtIndex(k_indexOfTest)->setMessages(I18n::Message::Tests, I18n::Message::TestDescr);
  cellAtIndex(k_indexOfInterval)->setMessages(I18n::Message::Intervals, I18n::Message::IntervalDescr);
  cellAtIndex(k_indexOfInterval)->setImage(ImageStore::ConfidenceInterval);
}

void MenuController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::Menu);
  m_selectableTableView.reloadData(true);
}

bool MenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    Data::SubApp subapp;
    ViewController * controller = nullptr;
    switch (selectedRow()) {
      case k_indexOfProbability:
        subapp = Data::SubApp::Probability;
        controller = m_distributionController;
        break;
      case k_indexOfTest:
        subapp = Data::SubApp::Tests;
        controller = m_testController;
        break;
      case k_indexOfInterval:
        subapp = Data::SubApp::Intervals;
        controller = m_testController;
        break;
    }
    assert(controller != nullptr);
    if (subapp != App::app()->subapp()) {
      tidyData(App::app()->subapp());
      App::app()->setSubapp(subapp);
      initData(subapp);
    }
    if (controller == m_testController) {
      m_testController->selectRow(0);
    }
    stackOpenPage(controller, 0);
    return true;
  }
  return false;
}

void Probability::MenuController::tidyData(Data::SubApp subapp) {
  if (subapp == Data::SubApp::Probability) {
    m_globalDistribution->~Distribution();
    m_globalCalculation->~Calculation();
  } else if (subapp == Data::SubApp::Tests || subapp == Data::SubApp::Intervals) {
    m_globalStatistic->~Statistic();
  }
}

void Probability::MenuController::initData(Data::SubApp subapp) {
  if (subapp == Data::SubApp::Probability) {
    new (m_globalDistribution) BinomialDistribution();
    new (m_globalCalculation) LeftIntegralCalculation(m_globalDistribution);
  } else if (subapp == Data::SubApp::Tests || subapp == Data::SubApp::Intervals) {
    new (m_globalStatistic) OneProportionStatistic();
    *m_globalTest = Data::Test::Unset;
    *m_globalTestType = Data::TestType::Unset;
  }
}
