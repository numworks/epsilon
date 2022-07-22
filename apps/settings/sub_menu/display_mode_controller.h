#ifndef SETTINGS_DISPLAY_MODE_CONTROLLER_H
#define SETTINGS_DISPLAY_MODE_CONTROLLER_H

#include "preferences_controller.h"
#include "../message_table_cell_with_editable_text_with_separator.h"
#include "../../shared/parameter_text_field_delegate.h"

namespace Settings {

class DisplayModeController : public PreferencesController, public Shared::ParameterTextFieldDelegate {
public:
  DisplayModeController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate);
  TELEMETRY_ID("DisplayMode");
  KDCoordinate nonMemoizedRowHeight(int j) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtIndex(int index) override { return (index == numberOfRows() - 1) ? k_significantDigitsType : k_resultFormatType; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  bool textFieldShouldFinishEditing(Escher::AbstractTextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) override;
private:
  constexpr static int k_resultFormatType = 0;
  constexpr static int k_significantDigitsType = 1;
  MessageTableCellWithEditableTextWithSeparator m_editableCell;
};

}

#endif
