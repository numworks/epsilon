#ifndef SOLVER_GUI_CENTERING_VIEW_H
#define SOLVER_GUI_CENTERING_VIEW_H

// TODO Hugo : Factorize with probability

#include <escher/view.h>
#include <escher/palette.h>

namespace Solver {

class CenteringView : public Escher::View {
public:
  CenteringView(Escher::View * v, KDColor background = Escher::Palette::WallScreen);
  KDSize minimalSizeForOptimalDisplay() const override;
  void layoutSubviews(bool force) override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  int numberOfSubviews() const override { return 1; }
  Escher::View * subviewAtIndex(int i) override { return m_contentView; }

private:
  KDCoordinate zeroIfNegative(KDCoordinate x) { return x < 0 ? 0 : x; }
  View * m_contentView;
  KDColor m_backgroundColor;
};

} // namespace Solver

#endif /* SOLVER_GUI_CENTERING_VIEW_H */
