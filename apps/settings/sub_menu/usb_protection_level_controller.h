#ifndef SETTINGS_USB_PROTECTION_LEVEL_CONTROLLER_H
#define SETTINGS_USB_PROTECTION_LEVEL_CONTROLLER_H

#include "generic_sub_controller.h"
#include <escher.h>

namespace Settings {

class UsbProtectionLevelController : public GenericSubController {
public:
  UsbProtectionLevelController(Responder * parentResponder);
  bool handleEvent(Ion::Events::Event event) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  int typeAtLocation(int i, int j) override;
private:
  static constexpr int k_maxNumberOfCells = 3;
  MessageTableCellWithBuffer m_cell[k_maxNumberOfCells];
};

}

#endif
