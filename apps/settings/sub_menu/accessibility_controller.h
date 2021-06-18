#ifndef SETTINGS_ACCESSIBILITY_CONTROLLER_H
#define SETTINGS_ACCESSIBILITY_CONTROLLER_H

#include "generic_sub_controller.h"
#include "../../hardware_test/pop_up_controller.h"

namespace Settings {

class AccessibilityController : public GenericSubController {
public:
  AccessibilityController(Responder * parentResponder);
  bool handleEvent(Ion::Events::Event event) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  int typeAtLocation(int i, int j) override;
private:
  constexpr static int k_totalNumberOfSwitchCells = 3;
  constexpr static int k_totalNumberOfGaugeCells = 3;
  MessageTableCellWithGauge m_gaugeCells[k_totalNumberOfGaugeCells];
  MessageTableCellWithSwitch m_switchCells[k_totalNumberOfSwitchCells];
};

}

#endif
