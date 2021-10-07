#ifndef SETTINGS_CODE_OPTIONS_CONTROLLER_H
#define SETTINGS_CODE_OPTIONS_CONTROLLER_H

#include "generic_sub_controller.h"
#include "preferences_controller.h"

namespace Settings {

class CodeOptionsController : public GenericSubController {
public:
  CodeOptionsController(Responder * parentResponder);
  bool handleEvent(Ion::Events::Event event) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  constexpr static int k_totalNumberOfCell = 1;
  constexpr static int k_totalNumberOfSwitchCells = 1;
  PreferencesController m_preferencesController;
  MessageTableCellWithChevronAndMessage m_cells[k_totalNumberOfCell];
  MessageTableCellWithSwitch m_switchCells[k_totalNumberOfCell];
};

}

#endif
