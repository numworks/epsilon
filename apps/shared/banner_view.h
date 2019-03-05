#ifndef SHARED_BANNER_VIEW_H
#define SHARED_BANNER_VIEW_H

#include <escher.h>

namespace Shared {

class BannerView : public View {
public:
  static KDCoordinate HeightGivenNumberOfLines(int linesCount);
  KDSize minimalSizeForOptimalDisplay() const override;
  void reload() { layoutSubviews(); }
private:
  int numberOfSubviews() const override = 0;
  View * subviewAtIndex(int index) override = 0;
  void layoutSubviews() override;
  int numberOfLines() const;
};

}

#endif
