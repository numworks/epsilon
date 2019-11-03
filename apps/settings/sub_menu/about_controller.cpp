#include "about_controller.h"
#include <assert.h>
#include <cmath>

namespace Settings {

AboutController::AboutController(Responder * parentResponder) :
  GenericSubController(parentResponder)
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_cells[i].setMessageFont(KDFont::LargeFont);
    m_cells[i].setAccessoryFont(KDFont::SmallFont);
    m_cells[i].setAccessoryTextColor(Palette::GreyDark);
  }
}

bool AboutController::handleEvent(Ion::Events::Event event) {
  /* We hide here the activation hardware test app: in the menu "about", by
   * clicking on '6' on the last row. */
  if ((event == Ion::Events::Six || event == Ion::Events::LowerT || event == Ion::Events::UpperT) && m_messageTreeModel->label() == I18n::Message::About && selectedRow() == numberOfRows()-1) {
    Container::activeApp()->displayModalViewController(&m_hardwareTestPopUpController, 0.f, 0.f, Metric::ExamPopUpTopMargin, Metric::PopUpRightMargin, Metric::ExamPopUpBottomMargin, Metric::PopUpLeftMargin);
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
#ifdef USERNAME
    if (selectedRow() == 1) {
#else
    if (selectedRow() == 0) {
#endif
      MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)m_selectableTableView.selectedCell();
      if (strcmp(myCell->accessoryText(), Ion::patchLevel()) == 0) {
        myCell->setAccessoryText(Ion::softwareVersion());
        return true;
      }
      myCell->setAccessoryText(Ion::patchLevel());
      return true;
    }
#ifdef USERNAME
    if (selectedRow() == 2) {
#else
    if (selectedRow() == 1) {
#endif
      MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)m_selectableTableView.selectedCell();
      if (strcmp(myCell->accessoryText(), Ion::customSoftwareVersion()) == 0) {
        myCell->setAccessoryText("Public"); //Change for public/dev
        return true;
      }
      myCell->setAccessoryText(Ion::customSoftwareVersion());
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
#ifdef USERNAME
  assert(index >= 0 && index < 5);
#else
  assert(index >= 0 && index < 4);
#endif
  myCell->setAccessoryText(messages[index]);
}

}
