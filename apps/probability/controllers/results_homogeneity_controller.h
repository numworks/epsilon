#ifndef APPS_PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H

#include <apps/shared/button_with_separator.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/highlight_cell.h>
#include <escher/message_text_view.h>
#include <escher/responder.h>
#include <escher/selectable_table_view.h>
#include <escher/table_view_data_source.h>
#include <escher/view.h>
#include <kandinsky/coordinate.h>

#include "probability/abstract/button_delegate.h"
#include "probability/abstract/chained_selectable_table_view_delegate.h"
#include "probability/abstract/homogeneity_data_source.h"
#include "probability/abstract/results_homogeneity_data_source.h"
#include "probability/gui/horizontal_or_vertical_layout.h"
#include "probability/gui/page_controller.h"
#include "probability/gui/selectable_table_view_with_background.h"
#include "probability/gui/solid_color_cell.h"
#include "probability/gui/spacer_view.h"
#include "probability/models/statistic/homogeneity_statistic.h"
#include "results_controller.h"

using namespace Escher;

namespace Probability {

class HomogeneityResultsView : public Escher::ScrollView {
public:
  HomogeneityResultsView(Responder * parent,
                         SelectableTableView * table,
                         ButtonDelegate * buttonDelegate);

  Escher::Button * button() { return m_contentView.button(); }
  // ScrollView
  KDSize minimalSizeForOptimalDisplay() const override;
  KDSize contentSize() const override { return KDSize(bounds().width(), 10000); }
  KDPoint tableOrigin() { return m_contentView.tableOrigin(); }
  KDPoint buttonOrigin() { return m_contentView.buttonOrigin(); }

  void drawRect(KDContext * ctx, KDRect rect) const override;

  constexpr static int k_topMargin = 5;

private:
  class ContentView : public VerticalLayout {
  public:
    ContentView(Responder * parent, SelectableTableView * table, ButtonDelegate * buttonDelegate);
    int numberOfSubviews() const override { return 3; }
    Escher::View * subviewAtIndex(int i) override;
    // TODO add selection behavior
    Escher::Button * button() { return &m_next; }
    KDPoint tableOrigin();
    KDPoint buttonOrigin();

  private:
    class ButtonWithHorizontalMargins : public VerticalLayout {
    public:
      ButtonWithHorizontalMargins(Escher::Button * button,
                                  KDCoordinate margin = k_defaultHorizontalMargin);
      int numberOfSubviews() const override { return 1; }
      Escher::View * subviewAtIndex(int i) override { return m_button; };

    private:
      constexpr static KDCoordinate k_defaultHorizontalMargin = Metric::CommonLeftMargin;
      Escher::Button * m_button;
    };
    constexpr static int k_indexOfTitle = 0;
    constexpr static int k_indexOfTable = 1;
    constexpr static int k_indexOfButton = 2;

    MessageTextView m_title;
    SelectableTableView * m_table;
    Escher::Button m_next;
    ButtonWithHorizontalMargins m_buttonWrapper;
  };

  ContentView m_contentView;
  ScrollViewDataSource m_scrollDataSource;
};

class ResultsHomogeneityController : public Page,
                                     public ButtonDelegate,
                                     public Escher::SelectableTableViewDelegate {
public:
  ResultsHomogeneityController(StackViewController * stackViewController,
                               HomogeneityStatistic * statistic,
                               ResultsController * resultsController);
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  const char * title() override { return nullptr; }
  View * view() override { return &m_contentView; }
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  bool buttonAction() override;

  void tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t,
                                               int previousSelectedCellX,
                                               int previousSelectedCellY,
                                               bool withinTemporarySelection = false) override;

private:
  void selectCorrectView();
  void scrollToCorrectLocation();

  ResultsController * m_resultsController;
  HomogeneityResultsView m_contentView;
  HomogeneityTableDataSourceWithTotals m_tableData;
  ResultsHomogeneityDataSource m_innerTableData;

  SelectableTableViewWithBackground m_table;
  bool m_isTableSelected;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H */
