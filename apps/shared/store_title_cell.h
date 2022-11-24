#ifndef SHARED_STORE_TITLE_CELL_H
#define SHARED_STORE_TITLE_CELL_H

#include "buffer_function_title_cell.h"
#include "separable.h"

namespace Shared {

class StoreTitleCell : public BufferFunctionTitleCell, public Separable {
public:
  StoreTitleCell();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews(bool force = false) override;
  bool cellTypeIsStorable() const override { return false; }
private:
  void didSetSeparator() override;
};

}

#endif