#ifndef SHARED_STORE_TITLE_CELL_H
#define SHARED_STORE_TITLE_CELL_H

#include "buffer_function_title_cell.h"
#include "store_separator_cell.h"

namespace Shared {

class StoreTitleCell : public BufferFunctionTitleCell, public StoreSeparatorCell {
public:
  StoreTitleCell(Orientation orientation, KDText::FontSize size = KDText::FontSize::Large) :
    BufferFunctionTitleCell(orientation, size),
    StoreSeparatorCell()
  {}
  void setSeparatorRight(bool separator) override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews() override;
};

}

#endif
