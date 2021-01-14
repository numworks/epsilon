#include "about_controller.h"
#include <escher/container.h>
#include <assert.h>
#include <cmath>

using namespace Escher;

namespace Settings {

AboutController::AboutController(Responder * parentResponder) :
  GenericSubController(parentResponder),
  m_view(&m_selectableTableView)
{
  // for (int i = 0; i < k_totalNumberOfCell; i++) {
  //   m_cells[i].setMessageFont(KDFont::LargeFont);
  //   m_cells[i].setSubLabelFont(KDFont::SmallFont);
  //   m_cells[i].setSubLabelTextColor(Palette::GrayDark);
  // }
}

bool AboutController::handleEvent(Ion::Events::Event event) {
  /* We hide here the activation hardware test app: in the menu "about", by
   * clicking on '6' on the last row. */
  if ((event == Ion::Events::Six || event == Ion::Events::LowerT || event == Ion::Events::UpperT) && m_messageTreeModel->label() == I18n::Message::About && selectedRow() == numberOfRows()-1) {
    Container::activeApp()->displayModalViewController(&m_hardwareTestPopUpController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpRightMargin, Metric::PopUpBottomMargin, Metric::PopUpLeftMargin);
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (selectedRow() == 0) {
      MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)m_selectableTableView.selectedCell();
      if (strcmp(myCell->subLabelText(), Ion::patchLevel()) == 0) {
        myCell->setSubLabelText(Ion::softwareVersion());
        return true;
      }
      myCell->setSubLabelText(Ion::patchLevel());
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
  // myCell->setMessageFont(KDFont::LargeFont);
  // myCell->setSubLabelFont(KDFont::SmallFont);
  // myCell->setSubLabelTextColor(Palette::GrayDark);
  const char * messages[] = {
    Ion::softwareVersion(),
    Ion::serialNumber(),
    Ion::fccId()
  };
  assert(index >= 0 && index < 3);
  myCell->setSubLabelText(messages[index]);
}

KDCoordinate AboutController::rowHeight(int index) {
  MessageTableCellWithBuffer tempCell = MessageTableCellWithBuffer();
  prepareCellForHeightCalculation((HighlightCell *)&tempCell, index);
  return tempCell.minimalSizeForOptimalDisplay().height();
}

}
