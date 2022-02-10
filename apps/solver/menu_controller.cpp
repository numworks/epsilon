#include "menu_controller.h"
#include <apps/i18n.h>
#include <assert.h>
#include "equations_icon.h"
#include "finance_icon.h"
#include "app.h"

using namespace Solver;

MenuController::MenuController(Escher::StackViewController * parentResponder, Escher::ViewController * listController, Escher::ViewController * financeMenuController) :
      Escher::SelectableCellListPage<Escher::SubappCell, k_numberOfCells>(parentResponder),
      m_listController(listController),
      m_financeMenuController(financeMenuController),
      m_contentView(&m_selectableTableView) {
  selectRow(0);
  cellAtIndex(k_indexOfEquation)->setMessages(I18n::Message::EquationsSubAppTitle, I18n::Message::EquationsSubAppDescription);
  cellAtIndex(k_indexOfEquation)->setImage(ImageStore::EquationsIcon);
  cellAtIndex(k_indexOfFinance)->setMessages(I18n::Message::FinanceSubAppTitle, I18n::Message::FinanceSubAppDescription);
  cellAtIndex(k_indexOfFinance)->setImage(ImageStore::FinanceIcon);
}

void MenuController::didBecomeFirstResponder() {
  App::Snapshot::SubApp previousSubApp = App::app()->snapshot()->subApp();
  if (previousSubApp != App::Snapshot::SubApp::Unknown) {
    // We just popped from a subapp controller
    int index = previousSubApp == App::Snapshot::SubApp::Equation ? k_indexOfEquation : k_indexOfFinance;
    // After unpacking, the selected row is lost
    selectRow(index);
    // Update snapshot's selected subapp
    App::app()->snapshot()->setSubApp(App::Snapshot::SubApp::Unknown);
  }
  m_selectableTableView.reloadData(true);
}

bool MenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    ViewController * controller = nullptr;
    switch (selectedRow()) {
      case k_indexOfEquation:
        controller = m_listController;
        App::app()->snapshot()->setSubApp(App::Snapshot::SubApp::Equation);
        break;
      case k_indexOfFinance:
        controller = m_financeMenuController;
        App::app()->snapshot()->setSubApp(App::Snapshot::SubApp::Finance);
        break;
    }
    assert(controller != nullptr);
    stackOpenPage(controller, 0);
    return true;
  }
  return false;
}
