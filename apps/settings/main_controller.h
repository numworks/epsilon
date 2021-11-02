#ifndef SETTINGS_MAIN_CONTROLLER_H
#define SETTINGS_MAIN_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/message_table_cell_with_switch.h>
#include "message_table_cell_with_gauge_with_separator.h"
#include "message_tree.h"
#include "sub_menu/about_controller.h"
#include "sub_menu/display_mode_controller.h"
#include "sub_menu/exam_mode_controller.h"
#include "sub_menu/localization_controller.h"
#include "sub_menu/preferences_controller.h"

namespace Settings {

extern const MessageTree s_modelAngleChildren[3];
extern const MessageTree s_modelEditionModeChildren[2];
extern const MessageTree s_modelFloatDisplayModeChildren[4];
extern const MessageTree s_modelComplexFormatChildren[3];
extern const MessageTree s_modelFontChildren[2];
extern const MessageTree s_modelExamChildren[2];
extern const MessageTree s_modelAboutChildren[3];
extern const MessageTree s_model;

class MainController : public Escher::SelectableListViewController {
public:
  MainController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate);
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtIndex(int index) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  void viewWillAppear() override;
  TELEMETRY_ID("");
private:
  constexpr static int k_indexOfAngleUnitCell = 0;
  constexpr static int k_indexOfDisplayModeCell = k_indexOfAngleUnitCell + 1;
  constexpr static int k_indexOfEditionModeCell = k_indexOfDisplayModeCell + 1;
  constexpr static int k_indexOfComplexFormatCell = k_indexOfEditionModeCell + 1;
  constexpr static int k_indexOfBrightnessCell = k_indexOfComplexFormatCell + 1;
  constexpr static int k_indexOfFontCell = k_indexOfBrightnessCell + 1;
  constexpr static int k_indexOfLanguageCell = k_indexOfFontCell + 1;
  constexpr static int k_indexOfCountryCell = k_indexOfLanguageCell + 1;
  constexpr static int k_indexOfExamModeCell = k_indexOfCountryCell + 1;
  /* Pop-up cell and About cell are located at the same index because pop-up
   * cell is optional. We must always correct k_indexOfAboutCell with
   * hasPrompt() (TODO: make hasPrompt() constexpr and correct
   * k_indexOfAboutCell) */
  constexpr static int k_indexOfPopUpCell = k_indexOfExamModeCell + 1;
  constexpr static int k_indexOfAboutCell = k_indexOfExamModeCell + 1;
  static const MessageTree * model();
  Escher::StackViewController * stackController() const;
  I18n::Message promptMessage() const;
  bool hasPrompt() const { return promptMessage() != I18n::Message::Default; }
  constexpr static int k_numberOfSimpleChevronCells = ((Ion::Display::Height - Escher::Metric::TitleBarHeight) / Escher::TableCell::k_minimalLargeFontCellHeight) + 2; // Remaining cell can be above and below so we add +2
  Escher::MessageTableCellWithChevronAndMessage m_cells[k_numberOfSimpleChevronCells];
  MessageTableCellWithGaugeWithSeparator m_brightnessCell;
  Escher::MessageTableCellWithSwitch m_popUpCell;
  PreferencesController m_preferencesController;
  DisplayModeController m_displayModeController;
  LocalizationController m_localizationController;
  ExamModeController m_examModeController;
  AboutController m_aboutController;

};

}

#endif
