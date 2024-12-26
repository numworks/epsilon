#ifndef STATISTICS_HISTOGRAM_MAIN_VIEW_H
#define STATISTICS_HISTOGRAM_MAIN_VIEW_H

#include "escher/selectable_list_view.h"
#include "histogram_banner_view.h"

namespace Statistics {

class HistogramMainView : public Escher::View {
 public:
  explicit HistogramMainView(Escher::SelectableListView* listView);

  HistogramBannerView* bannerView() { return &m_bannerView; }

  void setDisplayBanner(bool isVisible);

  void reload() { layoutSubviews(); }

 private:
  // Escher::View
  int numberOfSubviews() const override { return 2; }
  Escher::View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  void layoutBanner(bool force);

  KDSize bannerSize() const;

  HistogramBannerView m_bannerView;
  bool m_displayBanner;

  /* The SelectableListView is owned by the HistogramListController.
   * SelectableListView is a member of SelectableListViewController, which is a
   * base class of HistogramListController. */
  Escher::SelectableListView* m_listView;
};

}  // namespace Statistics

#endif
