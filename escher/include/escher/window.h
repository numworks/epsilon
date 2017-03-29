#ifndef ESCHER_WINDOW_H
#define ESCHER_WINDOW_H

#include <escher/view.h>

class Window : public View {
public:
  Window();
  void redraw(bool force = false);
  void setContentView(View * contentView);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
  virtual int numberOfSubviews() const override;
  virtual void layoutSubviews() override;
  virtual View * subviewAtIndex(int index) override;
  View * m_contentView;
private:
  const Window * window() const override;
};

#endif
