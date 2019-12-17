#ifndef SETTINGS_EXAM_MODE_CONTROLLER_H
#define SETTINGS_EXAM_MODE_CONTROLLER_H

#include "generic_sub_controller.h"

namespace Settings {

class ExamModeController : public GenericSubController {
public:
  ExamModeController(Responder * parentResponder);
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  int initialSelectedRow() const override;
  static constexpr int k_maxNumberOfCells = 2;
  MessageTableCell m_cell[k_maxNumberOfCells];
};

}

#endif
