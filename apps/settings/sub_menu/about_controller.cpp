#include "about_controller.h"
#include <assert.h>
#include <cmath>
#include <apps/settings/main_controller.h>

namespace Settings {

AboutController::AboutController(Responder * parentResponder) :
  GenericSubController(parentResponder),
  m_contributorsController(this)
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_cells[i].setMessageFont(KDFont::LargeFont);
    m_cells[i].setAccessoryFont(KDFont::SmallFont);
    m_cells[i].setAccessoryTextColor(Palette::GreyDark);
  }
}

bool AboutController::handleEvent(Ion::Events::Event event) {
  I18n::Message childLabel = m_messageTreeModel->children(selectedRow())->label();
  /* We hide here the activation hardware test app: in the menu "about", by
   * clicking on '6' on the last row. */
  if ((event == Ion::Events::Six || event == Ion::Events::LowerT || event == Ion::Events::UpperT) && m_messageTreeModel->label() == I18n::Message::About) {
    Container::activeApp()->displayModalViewController(&m_hardwareTestPopUpController, 0.f, 0.f, Metric::ExamPopUpTopMargin, Metric::PopUpRightMargin, Metric::ExamPopUpBottomMargin, Metric::PopUpLeftMargin);
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (selectedRow() == 0 + hasUsername()) {
      MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)m_selectableTableView.selectedCell();
      if (strcmp(myCell->accessoryText(), Ion::patchLevel()) == 0) {
        myCell->setAccessoryText(Ion::softwareVersion());
        return true;
      }
      myCell->setAccessoryText(Ion::patchLevel());
      return true;
    }
    if (selectedRow() == 1 + hasUsername()) {
      MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)m_selectableTableView.selectedCell();
      if (strcmp(myCell->accessoryText(), Ion::customSoftwareVersion()) == 0) {
        myCell->setAccessoryText("Public"); //Change for public/dev
        return true;
      }
      myCell->setAccessoryText(Ion::customSoftwareVersion());
      return true;
    }
    if (childLabel == I18n::Message::Contributors) {
      GenericSubController * subController = &m_contributorsController;
      subController->setMessageTreeModel(MainController::model()->children(aboutIndex)->children(selectedRow()));
      StackViewController * stack = stackController();
      stack->push(subController);
      return true;
    }
    return false;
  }
  return GenericSubController::handleEvent(event);
}

HighlightCell * AboutController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0 && index < k_totalNumberOfCell);
  return &m_cells[index];
}

int AboutController::reusableCellCount(int type) {
  assert(type == 0);
  return k_totalNumberOfCell;
}

void AboutController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GenericSubController::willDisplayCellForIndex(cell, index);
  if (MainController)
  MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)cell;
  static const char * messages[] = {
#ifdef USERNAME
    Ion::username(),
#endif
    Ion::softwareVersion(),
    Ion::customSoftwareVersion(),
    Ion::serialNumber(),
    Ion::fccId()
  };
  assert(index >= 0 && index < k_totalNumberOfCell);
  myCell->setAccessoryText(messages[index]);
}

}
