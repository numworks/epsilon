#ifndef SETTINGS_EXAM_MODE_CONTROLLER_H
#define SETTINGS_EXAM_MODE_CONTROLLER_H

#include "generic_sub_controller.h"
#include "selectable_view_with_messages.h"
#include "../../global_preferences.h"
#include "preferences_controller.h"

namespace Settings {

class ExamModeController : public GenericSubController {
public:
  ExamModeController(Responder * parentResponder);
  View * view() override { return &m_contentView; }
  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("ExamMode");
  void didEnterResponderChain(Responder * previousFirstResponder) override;
  int numberOfRows() const override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  static constexpr int k_numberOfDeactivationMessageLines = 3;
  static constexpr int k_numberOfCautionMessageLines = 3;
  int numberOfCautionLines() const;
  int initialSelectedRow() const override;
  GlobalPreferences::ExamMode examMode();
  static constexpr int k_maxNumberOfCells = 4;
  SelectableViewWithMessages m_contentView;
  MessageTableCell<> m_cell[k_maxNumberOfCells];
  PreferencesController m_ledController;
  PreferencesController m_examModeModeController;
  MessageTableCellWithChevronAndMessage m_examModeCell;
};

}

#endif
