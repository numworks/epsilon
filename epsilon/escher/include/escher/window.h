#ifndef ESCHER_WINDOW_H
#define ESCHER_WINDOW_H

#include <escher/view.h>

namespace Escher {

class Window : public View {
 public:
  Window() : m_contentView(nullptr) {}
  void redraw(bool force = false);
  void setContentView(View* contentView);
  void setAbsoluteFrame(KDRect frame) { m_frame = frame; }

 protected:
#if ESCHER_VIEW_LOGGING
  const char* className() const override;
#endif
  int numberOfSubviews() const override;
  void layoutSubviews(bool force = false) override;
  View* subviewAtIndex(int index) override;
  View* m_contentView;
};

}  // namespace Escher

#endif
