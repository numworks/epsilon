#ifndef APPS_CURVE_VIEW_WITH_BANNER_H
#define APPS_CURVE_VIEW_WITH_BANNER_H

#include <escher.h>
#include "curve_view.h"

class CurveViewWithBanner : public CurveView {
public:
  CurveViewWithBanner(CurveViewWindow * curveViewWindow = nullptr, float topMarginFactor = 0.0f,
    float rightMarginFactor = 0.0f, float bottomMarginFactor = 0.0f, float leftMarginFactor = 0.0f);
  virtual void reloadSelection() = 0;
  bool isMainViewSelected();
  void selectMainView(bool mainViewSelected);
protected:
  void layoutSubviews() override;
  bool m_mainViewSelected;
private:
  constexpr static KDCoordinate k_bannerHeight = 30;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  virtual View * bannerView() = 0;
};

#endif
