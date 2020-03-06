#ifndef SETTINGS_MAIN_CONTROLLER_H
#define SETTINGS_MAIN_CONTROLLER_H

#include <escher.h>
#include <apps/shared/settings_message_tree.h>
#include "message_table_cell_with_gauge_with_separator.h"
#include "sub_menu/about_controller.h"
#include "sub_menu/display_mode_controller.h"
#include "sub_menu/exam_mode_controller.h"
#include "sub_menu/language_controller.h"
#include "sub_menu/preferences_controller.h"

namespace Settings {

extern const Shared::SettingsMessageTree s_modelAngleChildren[3];
extern const Shared::SettingsMessageTree s_modelEditionModeChildren[2];
extern const Shared::SettingsMessageTree s_modelFloatDisplayModeChildren[4];
extern const Shared::SettingsMessageTree s_modelComplexFormatChildren[3];
extern const Shared::SettingsMessageTree s_modelFontChildren[2];
extern const Shared::SettingsMessageTree s_modelExamChildren[2];
extern const Shared::SettingsMessageTree s_modelAboutChildren[3];
extern const Shared::SettingsMessageTree s_model;

class MainController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  MainController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
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
  constexpr static int k_indexOfExamModeCell = k_indexOfLanguageCell + 1;
  /* Pop-up cell and About cell are located at the same index because pop-up
   * cell is optional. We must always correct k_indexOfAboutCell with
   * hasPrompt() (TODO: make hasPrompt() constexpr and correct
   * k_indexOfAboutCell) */
  constexpr static int k_indexOfPopUpCell = k_indexOfExamModeCell + 1;
  constexpr static int k_indexOfAboutCell = k_indexOfExamModeCell + 1;
  static const Shared::SettingsMessageTree * model();
  StackViewController * stackController() const;
  I18n::Message promptMessage() const;
  bool hasPrompt() const { return promptMessage() != I18n::Message::Default; }
  constexpr static int k_numberOfSimpleChevronCells = 7;
  MessageTableCellWithChevronAndMessage m_cells[k_numberOfSimpleChevronCells];
  MessageTableCellWithGaugeWithSeparator m_brightnessCell;
  MessageTableCellWithSwitch m_popUpCell;
  SelectableTableView m_selectableTableView;
  PreferencesController m_preferencesController;
  DisplayModeController m_displayModeController;
  LanguageController m_languageController;
  ExamModeController m_examModeController;
  AboutController m_aboutController;

};

}

#endif
