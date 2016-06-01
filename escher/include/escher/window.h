#ifndef ESCHER_WINDOW_H
#define ESCHER_WINDOW_H

#include <escher/view.h>

class Window : public View {
public:
  Window();
  void redraw();
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
  const Window * window() const override;
  int numberOfSubviews() const override;
  View * subview(int index) override;
  void layoutSubviews() override;
  void storeSubviewAtIndex(View * view, int index) override;
private:
  View * m_contentView;
};

#endif
