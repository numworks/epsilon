#ifndef STATISTICS_HISTOGRAM_MAIN_VIEW_H
#define STATISTICS_HISTOGRAM_MAIN_VIEW_H

#include <array>

#include "../store.h"
#include "escher/selectable_list_view.h"
#include "histogram_banner_view.h"

namespace Statistics {

class HistogramMainView : public Escher::View {
 public:
  explicit HistogramMainView(Escher::SelectableListView* listView);

  HistogramBannerView* bannerView() { return &m_bannerView; }

  Escher::SelectableListView* listView() { return m_listView; }

 private:
  HistogramBannerView m_bannerView;

  /* The SelectableListView is owned by the HistogramListController.
   * SelectableListView is a member of SelectableListViewController, which is a
   * base class of HistogramListController.  */
  Escher::SelectableListView* m_listView;

  Store* m_store;

  int numberOfSubviews() const override { return 2; }
  Escher::View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
};

}  // namespace Statistics

#endif
