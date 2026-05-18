#include <assert.h>
#include <escher/i18n.h>
#include <escher/modal_view_empty_controller.h>
#include <escher/theme/color_picker.h>

namespace Escher {

void ModalViewEmptyController::ModalViewEmptyView::drawRect(KDContext* ctx,
                                                            KDRect rect) const {
  ctx->fillRect(bounds(), ColorPicker::Resolve(k_backgroundColor));
  DrawBorderOfRect(ctx, bounds(), ThemeComponent::BorderGrayBright);
}

void ModalViewEmptyController::ModalViewEmptyView::layoutSubviews(bool force) {
  setChildFrame(&m_messageTextView, bounds(), force);
}

}  // namespace Escher
