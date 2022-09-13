#ifndef SETTINGS_ABOUT_CONTROLLER_H
#define SETTINGS_ABOUT_CONTROLLER_H

#include <apps/shared/pop_up_controller.h>
#include <escher/message_table_cell_with_buffer.h>
#include "generic_sub_controller.h"

namespace Settings {

class AboutController : public GenericSubController {
public:
  #if TERMS_OF_USE
  constexpr static int k_totalNumberOfCell = 4;
  #else
  constexpr static int k_totalNumberOfCell = 3;
  #endif

  AboutController(Escher::Responder * parentResponder);
  TELEMETRY_ID("About");
  bool handleEvent(Ion::Events::Event event) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;

private:
  constexpr static int k_versionCellIndex = 0;
  constexpr static int k_hardwareTestCellIndex = 2;

  Escher::MessageTableCellWithBuffer m_cells[k_totalNumberOfCell];
  Shared::MessagePopUpController m_hardwareTestPopUpController;
};

}

#endif
