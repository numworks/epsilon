#include "menu_controller.h"
#include <apps/i18n.h>
#include <assert.h>
#include "equations_icon.h"
#include "finance_icon.h"

using namespace Solver;

MenuController::MenuController(Escher::StackViewController * parentResponder,
                               Escher::ViewController * listController,
                               Escher::ViewController * financeMenuController) :
      Escher::SelectableListViewController(parentResponder),
      m_listController(listController),
      m_financeMenuController(financeMenuController),
      m_contentView(&m_selectableTableView) {
  selectRow(0);
  m_cells[k_indexOfEquation].setMessages(I18n::Message::EquationsSubAppTitle, I18n::Message::EquationsSubAppDescription);
  m_cells[k_indexOfEquation].setImage(ImageStore::EquationsIcon);
  m_cells[k_indexOfFinance].setMessages(I18n::Message::FinanceSubAppTitle, I18n::Message::FinanceSubAppDescription);
  m_cells[k_indexOfFinance].setImage(ImageStore::FinanceIcon);
}

void MenuController::didBecomeFirstResponder() {
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
    ViewController * controller = nullptr;
    switch (selectedRow()) {
      case k_indexOfEquation:
        controller = m_listController;
        break;
      case k_indexOfFinance:
        controller = m_financeMenuController;
        break;
    }
    assert(controller != nullptr);
    stackOpenPage(controller, 0);
    return true;
  }
  return false;
}
