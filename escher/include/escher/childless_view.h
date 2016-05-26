#ifndef ESCHER_CHILDLESS_VIEW_H
#define ESCHER_CHILDLESS_VIEW_H

#include <escher/view.h>

class ChildlessView : public View {
  using View::View;
protected:
  int numberOfSubviews() const override;
  const View * subview(int index) const override;
  void storeSubviewAtIndex(View * v, int index) override;
  void layoutSubviews() override;
};

#endif
