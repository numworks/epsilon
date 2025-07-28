#ifndef ESCHER_BORDERING_VIEW_H
#define ESCHER_BORDERING_VIEW_H

#include <escher/bordered.h>
#include <escher/view.h>

namespace Escher {

class BorderingView : public View, public Bordered {
 public:
  BorderingView(View* view) : m_contentView(view) {}
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void layoutSubviews(bool force) override;
  int numberOfSubviews() const override { return 1; }
  View* subviewAtIndex(int i) override { return m_contentView; };

 private:
  View* m_contentView;
};

}  // namespace Escher

#endif
