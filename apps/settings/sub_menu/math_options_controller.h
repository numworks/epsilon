#ifndef SETTINGS_MATH_OPTIONS_CONTROLLER_H
#define SETTINGS_MATH_OPTIONS_CONTROLLER_H

#include "generic_sub_controller.h"
#include <escher.h>
#include "display_mode_controller.h"
#include "preferences_controller.h"

namespace Settings {

class MathOptionsController : public GenericSubController {
public:
  MathOptionsController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate);
  bool handleEvent(Ion::Events::Event event) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  constexpr static int k_totalNumberOfCell = 7;
  MessageTableCellWithChevronAndMessage m_cells[k_totalNumberOfCell];
  PreferencesController m_preferencesController;
  DisplayModeController m_displayModeController;
};

}

#endif
