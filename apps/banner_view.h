#ifndef APPS_BANNER_VIEW_H
#define APPS_BANNER_VIEW_H

#include <escher.h>

class BannerView : public View {
public:
  void setLegendAtIndex(char * text, int index);
  KDSize minimalSizeForOptimalDisplay() override;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  int numberOfLines();
  virtual TextView * textViewAtIndex(int i) = 0;
};

#endif
