#include "menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

#include <new>

#include "probability/app.h"
#include "probability/data.h"

using namespace Probability;

MenuController::MenuController(Escher::StackViewController * parentResponder,
                               Escher::ViewController * distributionController,
                               Escher::ViewController * testController)
    : SelectableListViewPage(parentResponder),
      m_distributionController(distributionController),
      m_testController(testController) {
  m_cells[k_indexOfDistribution].setMessage(I18n::Message::ProbaApp);
  m_cells[k_indexOfTest].setMessage(I18n::Message::SignificanceTest);
  m_cells[k_indexOfInterval].setMessage(I18n::Message::ConfidenceInterval);
}

void MenuController::didBecomeFirstResponder() {
  Probability::App::app()->snapshot()->navigation()->setPage(Data::Page::Menu);
  // TODO factor out
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
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
        view = m_distributionController;
        subapp = Data::SubApp::Probability;
        break;
      case k_indexOfTest:
        subapp = Data::SubApp::Tests;
        view = m_testController;
        break;
      case k_indexOfInterval:
        subapp = Data::SubApp::Probability;
        view = m_testController;
        break;
    }
    assert(view != nullptr);
    if (App::app()->snapshot()->navigation()->subapp() != subapp) {
      initializeData(subapp);
    }
    App::app()->snapshot()->navigation()->setSubapp(subapp);
    openPage(view, true);
    return true;
  }
  return false;
}

void MenuController::initializeData(Data::SubApp subapp) {
  switch (subapp) {
    case Data::SubApp::Probability:
      new (App::app()->snapshot()->data()->distribution()) BinomialDistribution();
      new (App::app()->snapshot()->data()->calculation())
          LeftIntegralCalculation(App::app()->snapshot()->data()->distribution());
      break;
    case Data::SubApp::Tests:
    case Data::SubApp::Intervals:
      App::app()->snapshot()->data()->setTest(Data::Test::OneProp);
  }
}
