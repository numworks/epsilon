#ifndef SETTINGS_MAIN_CONTROLLER_H
#define SETTINGS_MAIN_CONTROLLER_H

#include <escher.h>
#include "settings_message_tree.h"
#include "sub_menu/about_controller.h"
#include "sub_menu/display_mode_controller.h"
#include "sub_menu/exam_mode_controller.h"
#include "sub_menu/language_controller.h"
#include "sub_menu/preferences_controller.h"
#include "sub_menu/contributors_controller.h"

namespace Settings {

class MainController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  MainController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void viewWillAppear() override;
private:
  StackViewController * stackController() const;
#ifdef EPSILON_BOOT_PROMPT
  constexpr static int k_totalNumberOfCell = 11;
  MessageTableCellWithSwitch m_popUpCell;
#else
  constexpr static int k_totalNumberOfCell = 10;
#endif
  constexpr static int k_numberOfSimpleChevronCells = 9;
  MessageTableCellWithChevronAndMessage m_cells[k_numberOfSimpleChevronCells];
  MessageTableCellWithGauge m_brightnessCell;
  SelectableTableView m_selectableTableView;
  MessageTree * m_messageTreeModel;
  PreferencesController m_preferencesController;
  DisplayModeController m_displayModeController;
  LanguageController m_languageController;
  ExamModeController m_examModeController;
  AboutController m_aboutController;
  ContributorsController m_contributorsController;

};

}

#endif
