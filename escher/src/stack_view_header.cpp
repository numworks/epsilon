#include <escher/stack_view_header.h>
#include <escher/metric.h>
extern "C" {
#include <assert.h>
}

namespace Escher {

StackViewHeader::StackViewHeader(ViewController * controller, KDColor textColor, KDColor backgroundColor, KDColor separatorColor) :
  View(),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor),
  m_separatorColor(separatorColor),
  m_controller(controller)
{
  markRectAsDirty(bounds());
}

void StackViewHeader::drawRect(KDContext * ctx, KDRect rect) const {
  KDRect b = bounds();
  drawBorderOfRect(ctx, b, m_separatorColor);
  drawInnerRect(ctx, b, m_backgroundColor);
  // Write title
  KDFont::Size font = KDFont::Size::Small;
  ctx->alignAndDrawString(m_controller->title(), KDPointZero, m_frame.size(), KDContext::k_alignCenter, KDContext::k_alignCenter,
                          font, m_textColor, m_backgroundColor);
}

#if ESCHER_VIEW_LOGGING
const char * StackViewHeader::className() const {
  return "StackViewHeader";
}

void StackViewHeader::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " name=\"" << m_controller->title() << "\"";
}
#endif

}
