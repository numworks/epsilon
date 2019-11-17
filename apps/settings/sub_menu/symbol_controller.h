#ifndef SETTINGS_SYMBOLCONTROLLER_CONTROLLER_H
#define SETTINGS_SYMBOLCONTROLLER_CONTROLLER_H

#include "generic_sub_controller.h"

namespace Settings {

class SymbolController : public GenericSubController {
public:
  SymbolController(Responder * parentResponder);
  bool handleEvent(Ion::Events::Event event) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  int typeAtLocation(int i, int j) override;
private:
  constexpr static int k_totalNumberOfSwitchCells = 4;
  MessageTableCellWithSwitch m_switchCells[k_totalNumberOfSwitchCells];
};

}

#endif