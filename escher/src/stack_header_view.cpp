#include <escher/metric.h>
#include <escher/stack_header_view.h>
extern "C" {
#include <assert.h>
}

namespace Escher {

StackHeaderView::StackHeaderView(ViewController *controller, KDColor textColor,
                                 KDColor backgroundColor,
                                 KDColor separatorColor)
    : View(),
      m_textColor(textColor),
      m_backgroundColor(backgroundColor),
      m_separatorColor(separatorColor),
      m_controller(controller) {
  markRectAsDirty(bounds());
}

void StackHeaderView::drawRect(KDContext *ctx, KDRect rect) const {
  KDRect b = bounds();
  drawBorderOfRect(ctx, b, m_separatorColor);
  drawInnerRect(ctx, b, m_backgroundColor);
  // Write title
  ctx->alignAndDrawString(m_controller->title(), KDPointZero, bounds().size(),
                          {.style = {.glyphColor = m_textColor,
                                     .backgroundColor = m_backgroundColor,
                                     .font = k_font},
                           .horizontalAlignment = KDGlyph::k_alignCenter});
}

#if ESCHER_VIEW_LOGGING
const char *StackHeaderView::className() const { return "StackViewHeader"; }

void StackHeaderView::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " name=\"" << m_controller->title() << "\"";
}
#endif

}  // namespace Escher
