#ifndef PROBABILITY_GUI_RESULTS_HOMOGENEITY_VIEW_H
#define PROBABILITY_GUI_RESULTS_HOMOGENEITY_VIEW_H

#include <apps/shared/button_with_separator.h>
#include <escher/message_text_view.h>
#include <escher/selectable_table_view.h>

#include <escher/invocation.h>
#include <escher/horizontal_or_vertical_layout.h>

// Factorize with Escher::TableViewWithTopAndBottomViews

namespace Probability {

class ResultsHomogeneityView : public Escher::ScrollView {
public:
  ResultsHomogeneityView(Escher::Responder * parent,
                         Escher::SelectableTableView * table,
                         Escher::Invocation invocation);

  Escher::Button * button() { return m_contentView.button(); }
  // ScrollView
  KDSize minimalSizeForOptimalDisplay() const override;
  KDSize contentSize() const override {
    return KDSize(maxContentWidthDisplayableWithoutScrolling(),
                  m_contentView.minimalSizeForOptimalDisplay().height());
  }
  KDPoint tableOrigin() { return m_contentView.tableOrigin(); }
  KDPoint buttonOrigin() { return m_contentView.buttonOrigin(); }

  void reload();

  constexpr static int k_topMargin = 5;

private:
  class ContentView : public Escher::VerticalLayout {
  public:
    ContentView(Escher::Responder * parent,
                Escher::SelectableTableView * table,
                Escher::Invocation invocation);
    int numberOfSubviews() const override { return 3; }
    Escher::View * subviewAtIndex(int i) override;
    Escher::Button * button() { return &m_next; }
    KDPoint tableOrigin();
    KDPoint buttonOrigin();

  private:
    class ButtonWithHorizontalMargins : public Escher::VerticalLayout {
    public:
      ButtonWithHorizontalMargins(Escher::Button * button,
                                  KDCoordinate margin = k_defaultHorizontalMargin);
      int numberOfSubviews() const override { return 1; }
      Escher::View * subviewAtIndex(int i) override { return m_button; };

    private:
      constexpr static KDCoordinate k_defaultHorizontalMargin = Escher::Metric::CommonLeftMargin;
      Escher::Button * m_button;
    };
    constexpr static int k_indexOfTitle = 0;
    constexpr static int k_indexOfTable = 1;
    constexpr static int k_indexOfButton = 2;

    Escher::MessageTextView m_title;
    Escher::SelectableTableView * m_table;
    Escher::Button m_next;
    ButtonWithHorizontalMargins m_buttonWrapper;
  };

  ContentView m_contentView;
  Escher::ScrollViewDataSource m_scrollDataSource;
};

}  // namespace Probability

#endif /* PROBABILITY_GUI_RESULTS_HOMOGENEITY_VIEW_H */
