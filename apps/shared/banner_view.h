#ifndef SHARED_BANNER_VIEW_H
#define SHARED_BANNER_VIEW_H

#include <escher.h>

namespace Shared {

class BannerView : public View {
public:
  static KDCoordinate HeightGivenNumberOfLines(int linesCount);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void reload() { layoutSubviews(); }
private:
  static constexpr KDCoordinate LineSpacing = 2;
  int numberOfSubviews() const override = 0;
  View * subviewAtIndex(int index) override = 0;
  void layoutSubviews() override;
  int numberOfLines() const;
};

}

#endif
