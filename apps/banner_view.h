#ifndef APPS_BANNER_VIEW_H
#define APPS_BANNER_VIEW_H

#include <escher.h>

class BannerView : public View {
public:
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setLegendAtIndex(char * text, int index);
  KDSize minimalSizeForOptimalDisplay() override;
  void layoutSubviews() override;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  int numberOfLines();
  virtual TextView * textViewAtIndex(int i) = 0;
};

#endif
