#include "about_controller.h"
#include "../../../python/src/py/mpconfig.h"
#include <assert.h>
#include <cmath>
#include <apps/settings/main_controller.h>
#include <poincare/integer.h>
#include <ion/storage.h>

#define MP_STRINGIFY_HELPER(x) #x
#define MP_STRINGIFY(x) MP_STRINGIFY_HELPER(x)

#ifndef OMEGA_STATE
#error This file expects OMEGA_STATE to be defined
#endif

namespace Settings {

AboutController::AboutController(Responder * parentResponder) :
  GenericSubController(parentResponder),
  m_contributorsController(this),
  m_contributorsCell(KDFont::LargeFont, KDFont::SmallFont)
  //m_view(&m_selectableTableView)
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_cells[i].setMessageFont(KDFont::LargeFont);
    m_cells[i].setAccessoryFont(KDFont::SmallFont);
    m_cells[i].setAccessoryTextColor(Palette::SecondaryText);
  }
}

bool AboutController::handleEvent(Ion::Events::Event event) {
  I18n::Message childLabel = m_messageTreeModel->childAtIndex(selectedRow()+(!hasUsernameCell()))->label();
  /* We hide here the activation hardware test app: in the menu "about", by
   * clicking on '6' on the last row. */
  if ((event == Ion::Events::Six || event == Ion::Events::LowerT || event == Ion::Events::UpperT) && childLabel == I18n::Message::FccId) {
    Container::activeApp()->displayModalViewController(&m_hardwareTestPopUpController, 0.f, 0.f, Metric::ExamPopUpTopMargin, Metric::PopUpRightMargin, Metric::ExamPopUpBottomMargin, Metric::PopUpLeftMargin);
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    if (childLabel == I18n::Message::Contributors) {
      GenericSubController * subController = &m_contributorsController;
      subController->setMessageTreeModel(m_messageTreeModel->childAtIndex(selectedRow()+(!hasUsernameCell())));
      StackViewController * stack = stackController();
      m_lastSelect = selectedRow();
      stack->push(subController);
      return true;
    }
    if (!(event == Ion::Events::Right)) {
      if (childLabel == I18n::Message::SoftwareVersion) {
        MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)m_selectableTableView.selectedCell();
        if (strcmp(myCell->accessoryText(), Ion::patchLevel()) == 0) {
          myCell->setAccessoryText(Ion::softwareVersion());
          return true;
        }
        myCell->setAccessoryText(Ion::patchLevel());
        return true;
      }
      if (childLabel == I18n::Message::OmegaVersion) {
        MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)m_selectableTableView.selectedCell();
        if (strcmp(myCell->accessoryText(), Ion::omegaVersion()) == 0) {
          myCell->setAccessoryText(MP_STRINGIFY(OMEGA_STATE)); //Change for public/dev
          return true;
        }
        myCell->setAccessoryText(Ion::omegaVersion());
        return true;
      }
      if (childLabel == I18n::Message::MemUse) {
        MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)m_selectableTableView.selectedCell();
        
        char memUseBuffer[15];
        
        if (strchr(myCell->accessoryText(), '%') == NULL) {
          int len = Poincare::Integer((int)((float)(Ion::Storage::k_storageSize - Ion::Storage::sharedStorage()->availableSize()) / (float)(Ion::Storage::k_storageSize) * 100.0f)).serialize(memUseBuffer, 4);
          memUseBuffer[len] = '%';
          memUseBuffer[len+1] = '\0';
          
          myCell->setAccessoryText(memUseBuffer);
        } else {
          int len = Poincare::Integer((int)((float) (Ion::Storage::k_storageSize - Ion::Storage::sharedStorage()->availableSize()) / 1024.f)).serialize(memUseBuffer, 4);
          memUseBuffer[len] = 'k';
          memUseBuffer[len+1] = 'B';
          memUseBuffer[len+2] = '/';
          
          len = Poincare::Integer((int)((float) Ion::Storage::k_storageSize / 1024.f)).serialize(memUseBuffer + len + 3, 4) + len + 3;
          memUseBuffer[len] = 'k';
          memUseBuffer[len+1] = 'B';
          memUseBuffer[len+2] = '\0';
          
          myCell->setAccessoryText(memUseBuffer);
        }
        
        return true;
      }
    }
    return false;
  }
  return GenericSubController::handleEvent(event);
}

int AboutController::numberOfRows() const {
  return m_messageTreeModel->numberOfChildren() - (!hasUsernameCell());
}

HighlightCell * AboutController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == 0) {
    assert(index < k_totalNumberOfCell-1-(!hasUsernameCell()));
    return &m_cells[index+(!hasUsernameCell())];
  }
  assert(index == 0);
  return &m_contributorsCell;
}

int AboutController::typeAtLocation(int i, int j) {
  return (j == numberOfRows() - 1 ? 1 : 0);
}

int AboutController::reusableCellCount(int type) {
  switch (type) {
    case 0:
      return k_totalNumberOfCell-1-(!hasUsernameCell());
    case 1:
      return 1;
    default:
      assert(false);
      return 0;
  }
}

bool AboutController::hasUsernameCell() const {
  return (*Ion::username()) != 0;
}

void AboutController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  int i = index + (!hasUsernameCell());
  GenericSubController::willDisplayCellForIndex(cell, i);
  assert(index >= 0 && index < k_totalNumberOfCell);
  if (m_messageTreeModel->childAtIndex(i)->label() == I18n::Message::Contributors) {
    MessageTableCellWithChevronAndMessage * myTextCell = (MessageTableCellWithChevronAndMessage *)cell;
    myTextCell->setSubtitle(I18n::Message::Default);
  } else if (m_messageTreeModel->childAtIndex(i)->label() == I18n::Message::MemUse) {
    char memUseBuffer[15];
    int len = Poincare::Integer((int)((float) (Ion::Storage::k_storageSize - Ion::Storage::sharedStorage()->availableSize()) / 1024.f)).serialize(memUseBuffer, 4);
    memUseBuffer[len] = 'k';
    memUseBuffer[len+1] = 'B';
    memUseBuffer[len+2] = '/';
    
    len = Poincare::Integer((int)((float) Ion::Storage::k_storageSize / 1024.f)).serialize(memUseBuffer + len + 3, 4) + len + 3;
    memUseBuffer[len] = 'k';
    memUseBuffer[len+1] = 'B';
    memUseBuffer[len+2] = '\0';
    
    MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)cell;
    myCell->setAccessoryText(memUseBuffer);
  } else {
    MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)cell;
    static const char * mpVersion = MICROPY_VERSION_STRING;
    static const char * messages[] = {
      Ion::username(),
      Ion::softwareVersion(),
      Ion::omegaVersion(),
      mpVersion,
      "",
      Ion::serialNumber(),
      Ion::fccId(),
      ""
    };
    myCell->setAccessoryText(messages[i]);
  }
}

}
