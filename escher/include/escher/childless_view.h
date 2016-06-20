#ifndef ESCHER_CHILDLESS_VIEW_H
#define ESCHER_CHILDLESS_VIEW_H

#include <escher/view.h>

class ChildlessView : public View {
  using View::View;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
};

#endif
