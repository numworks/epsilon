#ifndef SETTINGS_DISPLAY_MODE_CONTROLLER_H
#define SETTINGS_DISPLAY_MODE_CONTROLLER_H

#include <escher/menu_cell_with_editable_text.h>
#include <escher/message_text_view.h>

#include "../../shared/parameter_text_field_delegate.h"
#include "preferences_controller.h"

namespace Settings {

class DisplayModeController : public PreferencesController,
                              public Shared::ParameterTextFieldDelegate {
 public:
  DisplayModeController(
      Escher::Responder* parentResponder,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate);
  TELEMETRY_ID("DisplayMode");
  KDCoordinate nonMemoizedRowHeight(int row) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtRow(int row) const override {
    return (row == numberOfRows() - 1) ? k_significantDigitsType
                                       : k_resultFormatType;
  }
  KDCoordinate separatorBeforeRow(int row) override {
    return typeAtRow(row) == k_significantDigitsType
               ? k_defaultRowSeparator
               : PreferencesController::separatorBeforeRow(row);
  }
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 const char* text,
                                 Ion::Events::Event event) override;

 private:
  constexpr static int k_resultFormatType = 0;
  constexpr static int k_significantDigitsType = 1;
  Escher::MenuCellWithEditableText<Escher::MessageTextView> m_editableCell;
};

}  // namespace Settings

#endif
