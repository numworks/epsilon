#ifndef HOME_APP_CELL_H
#define HOME_APP_CELL_H

#include <escher.h>

namespace Home {

class AppCell : public ChildlessView {
public:
  AppCell();
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

}

#endif
