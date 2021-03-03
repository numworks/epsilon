#ifndef ESCHER_BUFFER_TABLE_CELL_H
#define ESCHER_BUFFER_TABLE_CELL_H

#include <escher/buffer_text_view.h>
#include <escher/table_cell.h>

namespace Escher {
/* BufferTableCell is here optimized for its use in code VariableBoxController.
 * It cannot be selected and has smaller vertical margins than other TableCell.
 */
class BufferTableCell : public TableCell {
public:
  BufferTableCell();
  const View * labelView() const override { return &m_labelView; }
  void setHighlighted(bool highlight) override { assert(!highlight); }
  void setLabelText(const char * textBody);
  void appendText(const char * textBody);
  void setTextColor(KDColor color);
  void setMessageFont(const KDFont * font);
  void setBackgroundColor(KDColor color);
  // Overriding TableCell row Height
  void layoutSubviews(bool force = false) override;
  KDSize minimalSizeForOptimalDisplay() const override {
    return KDSize(bounds().width(), k_subtitleRowHeight);
  }
  constexpr static KDCoordinate k_subtitleRowHeight = 23;
protected:
  KDColor backgroundColor() const override { return m_backgroundColor; }
private:
  BufferTextView m_labelView;
  KDColor m_backgroundColor;
};

}

#endif
