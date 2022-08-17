#include "menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell.h>
#include <escher/responder.h>

#include <new>

using namespace Shared;

MenuController::MenuController(Escher::StackViewController * parentResponder, std::initializer_list<Escher::ViewController *> controllers, std::initializer_list<std::initializer_list<I18n::Message>> messages, std::initializer_list<const Escher::Image *> images, MenuControllerDelegate * delegate) :
  Escher::SelectableCellListPage<Escher::SubappCell, k_menuControllerMaxNumberOfCells, Escher::MemoizedListViewDataSource>(parentResponder),
  m_delegate(delegate)
{
  selectRow(0);

  int i = 0;
  for (Escher::ViewController * vc : controllers) {
    m_controllers[i++] = vc;
  }
  i = 0;
  for (std::initializer_list<I18n::Message> mess : messages) {
    assert(mess.end() - mess.begin() == 2);
    cellAtIndex(i++)->setMessages(*mess.begin(), *(mess.begin() + 1));
  }
  i = 0;
  for (const Escher::Image * img : images) {
    cellAtIndex(i++)->setImage(img);
  }

  KDCoordinate availableHeight = Ion::Display::Height - Escher::Metric::TitleBarHeight;
  m_selectableTableView.setTopMargin(0);
  m_selectableTableView.setBottomMargin(0);
  m_selectableTableView.setFrame(KDRect(0, 0, Ion::Display::Width, availableHeight), true);
  KDCoordinate tableHeight = m_selectableTableView.minimalSizeForOptimalDisplay().height();
  assert(tableHeight < availableHeight);
  m_selectableTableView.setTopMargin((availableHeight - tableHeight) / 2);
}

void MenuController::stackOpenPage(Escher::ViewController * nextPage) {
  selectRow(m_delegate->selectedSubApp());
  ViewController::stackOpenPage(nextPage);
}

void MenuController::didBecomeFirstResponder() {
  m_delegate->selectSubApp(-1);
  m_selectableTableView.reloadData(true);
}

bool MenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    int selRow = selectedRow();
    m_delegate->selectSubApp(selRow);
    stackOpenPage(m_controllers[selRow]);
    return true;
  }
  return false;
}

