#ifndef SOLVER_GUI_BORDERING_VIEW_H
#define SOLVER_GUI_BORDERING_VIEW_H

#include <escher/bordered.h>
#include <escher/view.h>

namespace Solver {

class BorderingView : public Escher::View, public Escher::Bordered {
public:
  BorderingView(Escher::View * view) : m_contentView(view) {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void layoutSubviews(bool force) override;
  int numberOfSubviews() const override { return 1; }
  Escher::View * subviewAtIndex(int i) override { return m_contentView; };

private:
  Escher::View * m_contentView;
};

}  // namespace Solver

#endif /* SOLVER_GUI_BORDERING_VIEW_H */
