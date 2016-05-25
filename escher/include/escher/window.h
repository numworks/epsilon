#ifndef ESCHER_WINDOW_H
#define ESCHER_WINDOW_H

#include <escher/view.h>

class Window : public View {
public:
  Window();
protected:
  bool isOnScreen() override;
  int numberOfSubviews() override;
  View * subview(int index) override;
  void layoutSubviews() override;
  void storeSubviewAtIndex(View * view, int index) override;
private:
  View * m_contentView;
};

#endif
