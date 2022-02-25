#ifndef ESCHER_BUFFER_TABLE_CELL_H
#define ESCHER_BUFFER_TABLE_CELL_H

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <escher/table_cell.h>
#include <escher/metric.h>
#include <ion/display.h>

namespace Escher {
// TODO this is probably too specific to belong in Escher
/* BufferTableCell is here optimized for its use in code VariableBoxController.
 * It cannot be selected and has smaller vertical margins than other TableCell.
 */
class BufferTableCell : public TableCell {
public:
  BufferTableCell();
  const View * labelView() const override { return &m_labelView; }
  void setHighlighted(bool highlight) override { assert(!highlight); }
  void setLabelText(const char * textBody);
  void setMessageAndStringAsLabelText(I18n::Message message, const char * string = "");
  void appendText(const char * textBody);
  // Overriding TableCell row Height
  void layoutSubviews(bool force = false) override;
  KDSize minimalSizeForOptimalDisplay() const override {
    return KDSize(bounds().width(), k_subtitleRowHeight);
  }
  // Actual height is 24 with the overlapping pixel from SelectableTableView.
  constexpr static KDCoordinate k_subtitleRowHeight = 23;
protected:
  KDColor backgroundColor() const override { return k_backgroundColor; }
private:
  /* Text from BufferTableCell can be formed from user input (script names).
   * A char limit on display is enforced */
  constexpr static int k_maxNumberOfCharsInBuffer = (Ion::Display::Width - Metric::PopUpLeftMargin - 2 * Metric::CellSeparatorThickness - Metric::CellLeftMargin - Metric::CellRightMargin - Metric::PopUpRightMargin) / 7; // With 7 = KDFont::SmallFont->glyphSize().width()
  static_assert(k_maxNumberOfCharsInBuffer < Escher::BufferTextView::k_maxNumberOfChar, "k_maxNumberOfCharsInBuffer is too high");
  constexpr static KDColor k_backgroundColor = Palette::WallScreen;
  constexpr static KDColor k_textColor = Palette::BlueishGray;
  BufferTextView m_labelView;
};

}

#endif
