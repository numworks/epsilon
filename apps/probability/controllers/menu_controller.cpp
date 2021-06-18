#include "menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell.h>
#include <escher/responder.h>

#include <new>

#include "probability/app.h"

using namespace Probability;

MenuController::MenuController(Escher::StackViewController * parentResponder,
                               Escher::ViewController * distributionController,
                               Escher::ViewController * testController,
                               Data::SubApp * globalSubapp,
                               Distribution * globalDistribution,
                               Calculation * globalCalculation) :
    SelectableListViewPage(parentResponder),
    m_distributionController(distributionController),
    m_testController(testController),
    m_globalSubapp(globalSubapp),
    m_globalDistribution(globalDistribution),
    m_globalCalculation(globalCalculation) {
  m_cells[k_indexOfDistribution].setMessage(I18n::Message::ProbaApp);
  m_cells[k_indexOfDistribution].setSubtitle(I18n::Message::ProbaDescr);
  m_cells[k_indexOfTest].setMessage(I18n::Message::SignificanceTest);
  m_cells[k_indexOfTest].setSubtitle(I18n::Message::TestDescr);
  m_cells[k_indexOfInterval].setMessage(I18n::Message::ConfidenceInterval);
  m_cells[k_indexOfInterval].setSubtitle(I18n::Message::IntervalDescr);
}

void MenuController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::Menu);
  // TODO factor out
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  }
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
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
    ViewController * view = nullptr;
    switch (selectedRow()) {
      case k_indexOfDistribution:
        subapp = Data::SubApp::Probability;
        view = m_distributionController;
        break;
      case k_indexOfTest:
        subapp = Data::SubApp::Tests;
        view = m_testController;
        break;
      case k_indexOfInterval:
        subapp = Data::SubApp::Intervals;
        view = m_testController;
        break;
    }
    assert(view != nullptr);
    if (App::app()->subapp() != subapp) {
      initializeProbaData();
    }
    *m_globalSubapp = subapp;
    openPage(view);
    return true;
  }
  return false;
}

void MenuController::initializeProbaData() {
  new (m_globalDistribution) BinomialDistribution();
  new (m_globalCalculation) LeftIntegralCalculation(m_globalDistribution);
}
