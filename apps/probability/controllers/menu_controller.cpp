#include "menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

using namespace Probability;

MenuController::MenuController(Escher::StackViewController * parentResponder,
                               Escher::ViewController * distributionController, Escher::ViewController * testController)
    : SelectableListViewPage(parentResponder),
      m_distributionController(distributionController),
      m_testController(testController) {
  m_cells[k_indexOfDistribution].setMessage(I18n::Message::ProbaApp);
  m_cells[k_indexOfTest].setMessage(I18n::Message::SignificanceTest);
  m_cells[k_indexOfInterval].setMessage(I18n::Message::ConfidenceInterval);
}

void MenuController::didBecomeFirstResponder() {
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
    switch (selectedRow())
    {
    case k_indexOfDistribution:
      openPage(m_distributionController, true);
      break;
    case k_indexOfTest:
    case k_indexOfInterval:
      openPage(m_testController, true);
    }
    return true;
  }
  return false;
}

