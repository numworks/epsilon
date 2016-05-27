#ifndef ESCHER_WINDOW_H
#define ESCHER_WINDOW_H

#include <escher/view.h>

class Window : public View {
public:
  Window();
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
  bool isOnScreen() const override;
  int numberOfSubviews() const override;
  const View * subview(int index) const override;
  void layoutSubviews() override;
  void storeSubviewAtIndex(View * view, int index) override;
private:
  View * m_contentView;
};

#endif
