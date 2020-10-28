#ifndef SETTINGS_EXAM_MODE_CONTROLLER_H
#define SETTINGS_EXAM_MODE_CONTROLLER_H

#include "generic_sub_controller.h"
#include "selectable_view_with_messages.h"
#include "../../global_preferences.h"

namespace Settings {

class ExamModeController : public GenericSubController {
public:
  ExamModeController(Escher::Responder * parentResponder);
  Escher::View * view() override { return &m_contentView; }
  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("ExamMode");
  void didEnterResponderChain(Escher::Responder * previousFirstResponder) override;
  int numberOfRows() const override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
private:
  static constexpr int k_numberOfDeactivationMessageLines = 3;
  static constexpr int k_numberOfCautionMessageLines = 3;
  int numberOfCautionLines() const;
  int initialSelectedRow() const override;
  GlobalPreferences::ExamMode examMode();
  static constexpr int k_maxNumberOfCells = 2;
  SelectableViewWithMessages m_contentView;
  Escher::MessageTableCell m_cell[k_maxNumberOfCells];
};

}

#endif
