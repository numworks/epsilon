#include "menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/responder.h>

#include <new>

using namespace Shared;

MenuController::MenuController(
    Escher::StackViewController* parentResponder,
    std::initializer_list<Escher::ViewController*> controllers,
    std::initializer_list<std::initializer_list<I18n::Message>> messages,
    std::initializer_list<const Escher::Image*> images,
    MenuControllerDelegate* delegate)
    : Escher::UniformSelectableListController<Escher::SubappCell,
                                              k_numberOfCells>(parentResponder),
      m_delegate(delegate) {
  selectRow(0);

  int i = 0;
  for (Escher::ViewController* vc : controllers) {
    m_controllers[i++] = vc;
  }
  i = 0;
  for (std::initializer_list<I18n::Message> mess : messages) {
    assert(mess.end() - mess.begin() == 2);
    cell(i++)->setMessages(*mess.begin(), *(mess.begin() + 1));
  }
  i = 0;
  for (const Escher::Image* img : images) {
    cell(i++)->setImage(img);
  }

  centerTable(Escher::Metric::DisplayHeightWithoutTitleBar);
}

void MenuController::stackOpenPage(Escher::ViewController* nextPage) {
  selectRow(m_delegate->selectedSubApp());
  ViewController::stackOpenPage(nextPage);
}

void MenuController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    m_delegate->selectSubApp(-1);
    m_selectableListView.reloadData();
  } else {
    UniformSelectableListController::handleResponderChainEvent(event);
  }
}

bool MenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE ||
      event == Ion::Events::Right) {
    int selRow = selectedRow();
    m_delegate->selectSubApp(selRow);
    stackOpenPage(m_controllers[selRow]);
    return true;
  }
  return false;
}
