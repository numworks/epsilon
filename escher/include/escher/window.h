#ifndef ESCHER_WINDOW_H
#define ESCHER_WINDOW_H

#include <escher/view.h>

namespace Escher {

class Window : public View {
 public:
  Window() : m_contentView(nullptr) {}
  void redraw(bool force = false);
  void setContentView(View* contentView);

 protected:
#if ESCHER_VIEW_LOGGING
  const char* className() const override;
#endif
  int numberOfSubviews() const override;
  void layoutSubviews(bool force = false) override;
  View* subviewAtIndex(int index) override;
  View* m_contentView;

 private:
  const Window* window() const override;
};

}  // namespace Escher

#endif
