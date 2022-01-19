#ifndef ESCHER_CENTERING_VIEW_H
#define ESCHER_CENTERING_VIEW_H

#include <escher/view.h>
#include <escher/palette.h>

namespace Escher {

class CenteringView : public View {
public:
  CenteringView(View * v, KDColor background = Palette::WallScreen);
  KDSize minimalSizeForOptimalDisplay() const override;
  void layoutSubviews(bool force) override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int i) override { return m_contentView; }

private:
  KDCoordinate zeroIfNegative(KDCoordinate x) { return x < 0 ? 0 : x; }
  View * m_contentView;
  KDColor m_backgroundColor;
};

} // namespace Escher

#endif /* ESCHER_CENTERING_VIEW_H */
