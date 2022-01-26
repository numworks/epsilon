#include "results_homogeneity_view.h"

#include <apps/i18n.h>

using namespace Escher;

namespace Probability {

ResultsHomogeneityView::ContentView::ContentView(Responder * parent,
                                                 SelectableTableView * table,
                                                 Invocation invocation) :
      m_title(KDFont::SmallFont,
              I18n::Message::HomogeneityResultsTitle,
              KDContext::k_alignCenter,
              KDContext::k_alignCenter,
              Escher::Palette::GrayVeryDark,
              Escher::Palette::WallScreenDark),
      m_table(table),
      m_next(parent,
             I18n::Message::Next,
             invocation,
             KDFont::LargeFont),
      m_buttonWrapper(&m_next) {
}

Probability::ResultsHomogeneityView::ContentView::ButtonWithHorizontalMargins::
    ButtonWithHorizontalMargins(Escher::Button * button, KDCoordinate margin) :
      VerticalLayout(Palette::WallScreenDark), m_button(button) {
  setSecondaryDirectionMargin(margin);
}

Escher::View * Probability::ResultsHomogeneityView::ContentView::subviewAtIndex(int i) {
  switch (i) {
    case k_indexOfTitle:
      return &m_title;
    case k_indexOfTable:
      return m_table;
    case k_indexOfButton:
      return &m_buttonWrapper;
  }
  assert(false);
  return nullptr;
}

KDPoint Probability::ResultsHomogeneityView::ContentView::tableOrigin() {
  return KDPoint(0, m_title.minimalSizeForOptimalDisplay().height());
}

KDPoint Probability::ResultsHomogeneityView::ContentView::buttonOrigin() {
  return KDPoint(0, tableOrigin().y() + m_table->minimalSizeForOptimalDisplay().height());
}

Probability::ResultsHomogeneityView::ResultsHomogeneityView(Responder * parent,
                                                            SelectableTableView * table,
                                                            Invocation invocation) :
      ScrollView(&m_contentView, &m_scrollDataSource),
      m_contentView(parent, table, invocation) {
  setMargins(Metric::CellTopMargin, 0, Metric::CommonBottomMargin, 0);
  setBackgroundColor(Palette::WallScreenDark);
}

KDSize Probability::ResultsHomogeneityView::minimalSizeForOptimalDisplay() const {
  // Pass expected size to VerticalLayout to propagate to TableCells
  // TODO factorize with InputCategoricalView ..?
  ContentView * contentView = const_cast<ContentView *>(&m_contentView);
  if (contentView->bounds().width() <= 0)
    contentView->setSize(KDSize(bounds().width(), contentView->bounds().height()));
  KDSize requiredSize = ScrollView::minimalSizeForOptimalDisplay();
  return KDSize(bounds().width(), requiredSize.height());
}

void Probability::ResultsHomogeneityView::reload() {
  layoutSubviews();
}

}  // namespace Probability
