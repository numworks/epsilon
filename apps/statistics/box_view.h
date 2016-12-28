#ifndef STATISTICS_BOX_VIEW_H
#define STATISTICS_BOX_VIEW_H

#include <escher.h>
#include "data.h"
#include "box_banner_view.h"
#include "box_window.h"
#include "../constant.h"
#include "../curve_view_with_banner.h"

namespace Statistics {

class BoxView : public CurveViewWithBanner {
public:
  BoxView(Data * m_data);
  void reloadMainView() override;
  void reload(int selectedQuantile);
  int selectedQuantile();
  bool selectQuantile(int selectedQuantile);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_bannerHeight = 30;
  char * label(Axis axis, int index) const override;
  View * bannerView() override;
  Data * m_data;
  BoxWindow m_boxWindow;
  char m_labels[k_maxNumberOfXLabels][Constant::FloatBufferSizeInScientificMode];
  // -1->Unselect 0->min 1->first quartile 2->median 3->third quartile 4->max
  int m_selectedQuantile;
  BoxBannerView m_bannerView;
};

}


#endif
