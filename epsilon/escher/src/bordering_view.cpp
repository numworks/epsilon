#include <escher/bordering_view.h>
#include <escher/theme/color_picker.h>

namespace Escher {

void BorderingView::drawRect(KDContext* ctx, KDRect rect) const {
  drawBorderOfRect(ctx, bounds(),
                   ColorPicker::Resolve(ThemeComponent::SurfaceSelect));
}

KDSize BorderingView::minimalSizeForOptimalDisplay() const {
  KDSize contentSize = m_contentView->minimalSizeForOptimalDisplay();
  return KDSize(contentSize.width() + 2 * k_separatorThickness,
                contentSize.height() + 2 * k_separatorThickness);
}

void BorderingView::layoutSubviews(bool force) {
  KDSize contentSize = KDSize(bounds().width() - 2 * k_separatorThickness,
                              bounds().height() - 2 * k_separatorThickness);
  setChildFrame(m_contentView,
                KDRect(k_separatorThickness, k_separatorThickness, contentSize),
                force);
}

}  // namespace Escher
