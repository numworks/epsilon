#ifndef SETTINGS_PREFERENCES_CONTROLLER_H
#define SETTINGS_PREFERENCES_CONTROLLER_H

#include <escher/expression_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

#include "generic_sub_controller.h"

namespace Settings {

class PreferencesController : public GenericSubController {
 public:
  PreferencesController(Escher::Responder* parentResponder);
  bool handleEvent(Ion::Events::Event event) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  TELEMETRY_ID("Other");

 protected:
  constexpr static int k_totalNumberOfCell = 3;
  int initialSelectedRow() const override {
    return valueIndexForPreference(m_messageTreeModel->label());
  }

 private:
  constexpr static KDFont::Size k_layoutFont = KDFont::Size::Small;
  Poincare::Layout layoutForPreferences(I18n::Message message);
  void setPreferenceWithValueIndex(I18n::Message message, int valueIndex);
  int valueIndexForPreference(I18n::Message message) const;
  Escher::MenuCell<Escher::MessageTextView, Escher::ExpressionView>
      m_cells[k_totalNumberOfCell];
};

}  // namespace Settings

#endif
