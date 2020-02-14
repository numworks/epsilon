#ifndef SETTINGS_ABOUT_CONTROLLER_H
#define SETTINGS_ABOUT_CONTROLLER_H

#include "generic_sub_controller.h"
#include "selectable_view_with_messages.h"
#include "../../hardware_test/pop_up_controller.h"

namespace Settings {

class AboutController : public GenericSubController {
public:
  AboutController(Responder * parentResponder);
  View * view() override { return &m_view; }
  void viewWillAppear() override;
  TELEMETRY_ID("About");
  bool handleEvent(Ion::Events::Event event) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  constexpr static int k_totalNumberOfCell = 3;
  SelectableViewWithMessages m_view;
  MessageTableCellWithBuffer m_cells[k_totalNumberOfCell];
  HardwareTest::PopUpController m_hardwareTestPopUpController;
};

}

#endif
