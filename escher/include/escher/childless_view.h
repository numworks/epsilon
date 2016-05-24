#ifndef ESCHER_CHILDLESS_VIEW_H
#define ESCHER_CHILDLESS_VIEW_H

#include <escher/view.h>

class ChildlessView : public View {
protected:
  int numberOfSubviews() override;
  View * subview(int index) override;
};

#endif
