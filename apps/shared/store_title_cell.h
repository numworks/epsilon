#ifndef SHARED_STORE_TITLE_CELL_H
#define SHARED_STORE_TITLE_CELL_H

#include "buffer_function_title_cell.h"

namespace Shared {

class StoreTitleCell : public BufferFunctionTitleCell {
public:
  StoreTitleCell(Orientation orientation, KDText::FontSize size = KDText::FontSize::Large) :
    BufferFunctionTitleCell(orientation, size),
    m_separatorLeft(false)
  {}
  void setSeparatorLeft(bool separator);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews() override;
private:
  bool m_separatorLeft;
};

}

#endif
