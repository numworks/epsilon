#include "about_controller.h"
#include "../../apps_container.h"
#include <assert.h>
#include <cmath>

using namespace Escher;

namespace Settings {

AboutController::AboutController(Responder * parentResponder) :
  GenericSubController(parentResponder),
  m_hardwareTestPopUpController(
    Escher::Invocation(
      [](void * context, void * sender) {
        AppsContainer * appsContainer = AppsContainer::sharedAppsContainer();
        appsContainer->switchToBuiltinApp(appsContainer->hardwareTestAppSnapshot());
        return true;
      },
      this
    ),
    I18n::Message::HardwareTestLaunch
  )
{
}

bool AboutController::handleEvent(Ion::Events::Event & event) {
  /* We hide here the activation hardware test app: in the menu "about", by
   * clicking on '6' on the last row. */
  if ((event == Ion::Events::Six || event == Ion::Events::LowerT || event == Ion::Events::UpperT) && m_messageTreeModel->label() == I18n::Message::About && selectedRow() == numberOfRows()-1) {
    m_hardwareTestPopUpController.presentModally();
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (selectedRow() == 0) {
      /* When pressing OK on the version cell, the display cycles between
       * Epsilon version number, the commit hash for this build of Epsilon, the
       * PCB revision number, the flags used at compilation and the bootloader
       * running on the device. */
      MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)m_selectableTableView.selectedCell();
      const char * currentText = myCell->subLabelText();
      if (strcmp(currentText, Ion::patchLevel()) == 0) {
        myCell->setSubLabelText(Ion::pcbVersion());
      } else if (strcmp(currentText, Ion::pcbVersion()) == 0) {
        myCell->setSubLabelText(Ion::compilationFlags());
      } else if (strcmp(currentText, Ion::compilationFlags()) == 0) {
        myCell->setSubLabelText(Ion::runningBootloader());
      } else if (strcmp(currentText, Ion::runningBootloader()) == 0) {
        myCell->setSubLabelText(Ion::epsilonVersion());
      } else {
        assert(strcmp(currentText, Ion::epsilonVersion()) == 0);
        myCell->setSubLabelText(Ion::patchLevel());
      }
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

void AboutController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GenericSubController::willDisplayCellForIndex(cell, index);
  MessageTableCellWithBuffer * myCell = static_cast<MessageTableCellWithBuffer *>(cell);
  const char * messages[] = {
    Ion::epsilonVersion(),
    Ion::serialNumber(),
    Ion::fccId()
  };
  assert(index >= 0 && index < 3);
  myCell->setSubLabelText(messages[index]);
}

KDCoordinate AboutController::nonMemoizedRowHeight(int index) {
  return MemoizedListViewDataSource::nonMemoizedRowHeight(index);
}

}
