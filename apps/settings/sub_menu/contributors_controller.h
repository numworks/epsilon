#ifndef SETTINGS_CONTRIBUTORS_CONTROLLER_H
#define SETTINGS_CONTRIBUTORS_CONTROLLER_H

#include "generic_sub_controller.h"
#include <escher.h>

namespace Settings {

class ContributorsController : public GenericSubController {
public:
  ContributorsController(Responder * parentResponder);
  bool handleEvent(Ion::Events::Event event) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  constexpr static int k_totalNumberOfCell = 7;
  MessageTableCellWithBuffer m_cells[k_totalNumberOfCell];
};

}

#endif
