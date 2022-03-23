#ifndef SETTINGS_BRIGHTNESS_CONTROLLER_H
#define SETTINGS_BRIGHTNESS_CONTROLLER_H

#include "generic_sub_controller.h"
#include "preferences_controller.h"
#include "../message_table_cell_with_editable_text_with_separator.h"
#include "../../shared/parameter_text_field_delegate.h"
#include "../message_table_cell_with_gauge_with_separator.h"

namespace Settings {

class BrightnessController : public GenericSubController, public Shared::ParameterTextFieldDelegate {
public:
  BrightnessController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate);
  TELEMETRY_ID("BrightnessSettings");
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  bool handleEvent(Ion::Events::Event event) override;
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
protected:
  constexpr static int k_totalNumberOfCell = 4;
private:
  MessageTableCellWithGauge m_brightnessCell;
  MessageTableCellWithEditableText m_editableCellIdleBeforeDimmingSeconds;
  MessageTableCellWithEditableText m_editableCellIdleBeforeSuspendSeconds;
  MessageTableCellWithEditableText m_BrightnessShortcutCell;
};

}

#endif
