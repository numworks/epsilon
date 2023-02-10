#ifndef SETTINGS_EXAM_MODE_CONTROLLER_H
#define SETTINGS_EXAM_MODE_CONTROLLER_H

#include <poincare/preferences.h>

#include "generic_sub_controller.h"
#include "selectable_view_with_messages.h"

namespace Settings {

class ExamModeController : public GenericSubController {
 public:
  ExamModeController(Escher::Responder* parentResponder);
  const char* title() override {
    return I18n::translate(I18n::Message::ExamMode);
  }
  Escher::View* view() override { return &m_contentView; }
  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("ExamMode");
  void didEnterResponderChain(
      Escher::Responder* previousFirstResponder) override;
  int numberOfRows() const override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;

 private:
  constexpr static int k_numberOfDeactivationMessageLines = 3;
  int initialSelectedRow() const override;
  Poincare::ExamMode examMode();
  constexpr static int k_maxNumberOfCells = 5;
  SelectableViewWithMessages m_contentView;
  Escher::MessageTableCell m_cell[k_maxNumberOfCells];
};

}  // namespace Settings

#endif
