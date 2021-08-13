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
                               Data::SubApp * globalSubapp,
                               Data::Test * globalTest,
                               Data::TestType * globalTestType,
                               Distribution * globalDistribution,
                               Calculation * globalCalculation) :
    SelectableListViewPage(parentResponder),
    m_distributionController(distributionController),
    m_testController(testController),
    m_globalSubapp(globalSubapp),
    m_globalTest(globalTest),
    m_globalTestType(globalTestType),
    m_globalDistribution(globalDistribution),
    m_globalCalculation(globalCalculation),
    m_contentView(&m_selectableTableView) {
  m_cells[k_indexOfDistribution].setMessages(I18n::Message::ProbaApp, I18n::Message::ProbaDescr);
  m_cells[k_indexOfDistribution].setImage(ImageStore::Probability);
  m_cells[k_indexOfTest].setImage(ImageStore::SignificanceTest);
  m_cells[k_indexOfTest].setMessages(I18n::Message::Tests, I18n::Message::TestDescr);
  m_cells[k_indexOfInterval].setMessages(I18n::Message::Interval, I18n::Message::IntervalDescr);
  m_cells[k_indexOfInterval].setImage(ImageStore::ConfidenceInterval);
}

void MenuController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::Menu);
  // TODO factor out
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  }
  m_selectableTableView.reloadData(true);
}

Escher::HighlightCell * MenuController::reusableCell(int index, int type) {
  // Return correct cell
  assert(type == 0);
  assert(index >= 0 && index <= k_numberOfCells);
  return &m_cells[index];
}

bool MenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    Data::SubApp subapp;
    ViewController * controller = nullptr;
    switch (selectedRow()) {
      case k_indexOfDistribution:
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
      resetData(subapp);
    }
    if (controller == m_testController) {
      m_testController->selectRow(0);
    }
    *m_globalSubapp = subapp;
    openPage(controller);
    return true;
  }
  return false;
}

void MenuController::initializeProbaData() {
  new (m_globalDistribution) BinomialDistribution();
  new (m_globalCalculation) LeftIntegralCalculation(m_globalDistribution);
}

void Probability::MenuController::resetData(Data::SubApp subapp) {
  if (subapp == Data::SubApp::Probability) {
    initializeProbaData();
  } else {
    *m_globalTest = Data::Test::Unset;
    *m_globalTestType = Data::TestType::Unset;
  }
}
