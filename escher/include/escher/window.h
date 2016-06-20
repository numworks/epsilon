#ifndef ESCHER_WINDOW_H
#define ESCHER_WINDOW_H

#include <escher/view.h>

class Window : public View {
public:
  Window();
  void redraw();
  void setContentView(View * contentView);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
  const Window * window() const override;
  int numberOfSubviews() const override;
  void layoutSubviews() override;
private:
  View * subviewAtIndex(int index) override;
  View * m_contentView;
};

#endif
