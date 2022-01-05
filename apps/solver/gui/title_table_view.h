#ifndef PROBABILITY_TITLE_TABLE_VIEW_H
#define PROBABILITY_TITLE_TABLE_VIEW_H

#include <apps/i18n.h>
#include <escher/scroll_view.h>
#include <escher/selectable_table_view.h>
#include <escher/horizontal_or_vertical_layout.h>
#include <escher/message_text_view.h>

// TODO Hugo : factorize with Probability::ResultsController or Settings::PressToTestController

namespace Solver {

class TitleTableView : public Escher::ScrollView, public Escher::SelectableTableViewDelegate {
public:
  TitleTableView(Escher::SelectableTableView * table,
              Escher::TableViewDataSource * tableDataSource,
              I18n::Message titleMessage);

  KDPoint tableOrigin();
  void tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t,
                                               int previousSelectedCellX,
                                               int previousSelectedCellY,
                                               bool withinTemporarySelection = false) override;

  void reload();
  void layoutSubviews(bool force = false) override;

private:
  /* Lays out a title and a table */
  class ContentView : public Escher::VerticalLayout {
  public:
    ContentView(Escher::SelectableTableView * table, I18n::Message titleMessage);
    int numberOfSubviews() const override { return 2; }
    Escher::View * subviewAtIndex(int i) override;

  private:
    friend TitleTableView;
    constexpr static int k_marginAroundTitle = Escher::Metric::TableSeparatorThickness;
    Escher::MessageTextView m_title;
    Escher::SelectableTableView * m_table;
  };

  ContentView m_contentView;
  Escher::ScrollViewDataSource m_scrollDataSource;
  Escher::TableViewDataSource * m_tableDataSource;
};

}  // namespace Solver

#endif /* PROBABILITY_TITLE_TABLE_VIEW_H */
