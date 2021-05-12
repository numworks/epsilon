#ifndef VERTICAL_LAYOUT_H
#define VERTICAL_LAYOUT_H

#include <apps/i18n.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/message_text_view.h>
#include <escher/responder.h>
#include <escher/view.h>
#include <escher/view_controller.h>
#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/coordinate.h>
#include <kandinsky/rect.h>

#include "selectable_cell_list_controller.h"

namespace Probability {

/*
 * View that lays out its subviews vertically.
 */
template <int numberOfChildren>
class VerticalLayout : public Escher::View {
  // To inherit, just provide the list of subviews, either to the constructor of by calling setView
 public:
  VerticalLayout(Escher::View * views) : m_views{views} {}
  VerticalLayout() : VerticalLayout(nullptr) {}
  int numberOfSubviews() const override { return numberOfChildren; }
  View * subviewAtIndex(int index) override { return m_views[index]; }
  void layoutSubviews(bool force = false) override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setView(Escher::View * v, int i) { m_views[i] = v; }

 protected:
  constexpr static int k_numberOfChildren = numberOfChildren;
  Escher::View * m_views[numberOfChildren];
};

template <int n>
void VerticalLayout<n>::layoutSubviews(bool force) {
  KDRect frame = bounds();
  KDCoordinate availableHeight = frame.height();
  KDRect proposedFrame = KDRect(0, 0, frame.width(), 0);
  int height;
  for (int i = 0; i < n; i++) {
    m_views[i]->setFrame(KDRect(0, proposedFrame.y() + proposedFrame.height(), frame.width(), availableHeight), false);
    height = m_views[i]->minimalSizeForOptimalDisplay().height();
    proposedFrame = KDRect(0, proposedFrame.y() + proposedFrame.height(), frame.width(), height);
    m_views[i]->setFrame(proposedFrame, false);
    availableHeight -= height;
    if (availableHeight < 0) {
      assert(false);
      availableHeight = 0;
    }
  }
}

template <int n>
void VerticalLayout<n>::drawRect(KDContext * ctx, KDRect rectToRedraw) const {
  ctx->fillRect(rectToRedraw, KDColorOrange);
}

}  // namespace Probability

#endif /* VERTICAL_LAYOUT_H */
