#ifndef SETTINGS_MAIN_CONTROLLER_H
#define SETTINGS_MAIN_CONTROLLER_H

#include <escher.h>
#include "sub_controller.h"
#include "settings_message_tree.h"
#include "language_controller.h"

namespace Settings {

class MainController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  MainController(Responder * parentResponder);
  ~MainController();
  MainController(const MainController& other) = delete;
  MainController(MainController&& other) = delete;
  MainController& operator=(const MainController& other) = delete;
  MainController& operator=(MainController&& other) = delete;
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
#if EPSILON_SOFTWARE_UPDATE_PROMPT
  constexpr static int k_totalNumberOfCell = 8;
  MessageTableCellWithSwitch m_updateCell;
#else
  constexpr static int k_totalNumberOfCell = 7;
#endif
  constexpr static int k_numberOfSimpleChevronCells = 5;
  MessageTableCellWithChevronAndMessage m_cells[k_numberOfSimpleChevronCells];
  MessageTableCellWithChevronAndExpression m_complexFormatCell;
  MessageTableCellWithGauge m_brightnessCell;
  Poincare::ExpressionLayout * m_complexFormatLayout;
  SelectableTableView m_selectableTableView;
  MessageTree * m_messageTreeModel;
  SubController m_subController;
  LanguageController m_languageController;
};

}

#endif
