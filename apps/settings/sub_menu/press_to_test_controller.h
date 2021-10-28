#ifndef SETTINGS_PRESS_TO_TEST_CONTROLLER_H
#define SETTINGS_PRESS_TO_TEST_CONTROLLER_H

// TODO Hugo : Move horizontal_or_vertical_layout.h in Escher
#include <apps/probability/gui/horizontal_or_vertical_layout.h>
#include <apps/shared/button_with_separator.h>
#include <escher/memoized_list_view_data_source.h>
#include <escher/scroll_view.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/selectable_table_view_delegate.h>
#include <escher/view_controller.h>
// TODO Hugo : Delete when cellWidth() workaround is fixed
#include <ion.h>
#include "../../global_preferences.h"
#include "press_to_test_switch.h"

namespace Settings {

class PressToTestView : public Escher::ScrollView, public Escher::SelectableTableViewDelegate {
public:
  constexpr static int k_verticalMargins = Escher::Metric::TableSeparatorThickness;
  PressToTestView(Escher::SelectableTableView * table, Escher::TableViewDataSource * tableDataSource) : Escher::ScrollView(&m_contentView, &m_scrollDataSource), m_contentView(table), m_tableDataSource(tableDataSource) {
    // Set the outer margins around the table and texts
    setMargins(k_verticalMargins, Escher::Metric::CommonMargin, k_verticalMargins, Escher::Metric::CommonMargin);
  }
  void reload();
  void setMessages(I18n::Message top, I18n::Message bottom) { m_contentView.setMessages(top, bottom); }
  /* SelectableTableViewDelegate */
  void tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;
private:
  /* Lays out a message, a table and a message */
  class ContentView : public Probability::VerticalLayout {
  public:
    ContentView(Escher::SelectableTableView * table);
    void setMessages(I18n::Message top, I18n::Message bottom);
    KDCoordinate tableOrigin() const;
    KDCoordinate totalHeight() const;
    /* View */
    // Hide bottom message if empty
    int numberOfSubviews() const override { return 2 + !isBottomMessageEmpty(); }
    Escher::View * subviewAtIndex(int i) override;
  private:
    friend PressToTestView;
    constexpr static int k_tableMargins = PressToTestView::k_verticalMargins;

    bool isBottomMessageEmpty() const { return m_bottomMessageView.text()[0] == 0; }
    KDCoordinate topTableMargin() const { return PressToTestView::k_verticalMargins; }
    KDCoordinate bottomTableMargin() const { return isBottomMessageEmpty() ? 0 : PressToTestView::k_verticalMargins; }

    Escher::SelectableTableView * m_table;
    Escher::MessageTextView m_topMessageView;
    Escher::MessageTextView m_bottomMessageView;
  };

  ContentView m_contentView;
  Escher::ScrollViewDataSource m_scrollDataSource;
  Escher::TableViewDataSource * m_tableDataSource;
};

class PressToTestController : public Escher::ViewController, public Escher::MemoizedListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  PressToTestController(Escher::Responder * parentResponder);
  const char * title() override { return I18n::translate(I18n::Message::PressToTest); }
  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("PressToTest");
  void didBecomeFirstResponder() override;
  void didEnterResponderChain(Escher::Responder * previousFirstResponder) override;
  int numberOfRows() const override;
  int typeAtIndex(int index) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  GlobalPreferences::PressToTestParams getPressToTestParams();
  void viewDidDisappear() override;
  Escher::View * view() override { return &m_view; }
  // TODO Hugo : fix this workaround
  KDCoordinate cellWidth() override { return Ion::Display::Width - 2 * Escher::Metric::CommonMargin; }
private:
  /* Cell type */
  static constexpr int k_switchCellType = 0;
  static constexpr int k_buttonCellType = 1;
  /* Switch cells index */
  static constexpr int k_equationSolverIndex = 0;
  static constexpr int k_inequalityGraphingIndex = k_equationSolverIndex + 1;
  static constexpr int k_implicitPlotsIndex = k_inequalityGraphingIndex + 1;
  static constexpr int k_statDiagnosticIndex = k_implicitPlotsIndex + 1;
  static constexpr int k_vectorsIndex = k_statDiagnosticIndex + 1;
  static constexpr int k_basedLogarithmIndex = k_vectorsIndex + 1;
  static constexpr int k_sumIndex = k_basedLogarithmIndex + 1;
  /* Switch cells count */
  static constexpr int k_numberOfSwitchCells = k_sumIndex + 1;
  // TODO Hugo: At most 6 switch cells only fit in the screen.
  static constexpr int k_numberOfReusableSwitchCells = 7;

  // Switch Cells
  void resetSwitches();
  void setParamAtIndex(int index, bool value);
  bool getParamAtIndex(int index);
  void setMessages();

  Escher::SelectableTableView m_selectableTableView;
  PressToTestView m_view;
  PressToTestSwitch m_switchCells[k_numberOfReusableSwitchCells];
  GlobalPreferences::PressToTestParams m_tempPressToTestParams;
  Shared::ButtonWithSeparator m_activateButton;

  static I18n::Message LabelAtIndex(int index);
  static I18n::Message SubLabelAtIndex(int index);
};

}

#endif
