#ifndef REGRESSION_GRAPH_VIEW_H
#define REGRESSION_GRAPH_VIEW_H

#include <escher.h>
#include "data.h"
#include "banner_view.h"
#include "../constant.h"
#include "../curve_view_with_banner.h"

namespace Regression {

class GraphView : public CurveViewWithBanner {
public:
  GraphView(Data * m_data);
  void reloadSelection() override;
  void reload();
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_dotSize = 4;
  char * label(Axis axis, int index) const override;
  View * bannerView() override;
  float evaluateModelWithParameter(Model * curve, float t) const override;
  Data * m_data;
  char m_xLabels[k_maxNumberOfXLabels][Constant::FloatBufferSizeInScientificMode];
  char m_yLabels[k_maxNumberOfYLabels][Constant::FloatBufferSizeInScientificMode];
  BannerView m_bannerView;
};

}


#endif
