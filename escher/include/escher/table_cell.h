#ifndef ESCHER_TABLE_CELL_H
#define ESCHER_TABLE_CELL_H

#include <escher/bordered.h>
#include <escher/highlight_cell.h>

namespace Escher {

class TableCell : public Bordered, public HighlightCell {
public:
  /* TableCells are laid out as follow :
   * - Accessory is vertically centered on the right of the cell
   * - Label is vertically centered, aligned on the left of the cell
   * - If it fits, SubLabel is vertically centered between Label and Accessory,
       aligned on the right of the available space.
   * - Otherwise, SubLabel is placed below label, aligned on the left of the
   *   cell
   * With :
   *     * = As much space as available (or nothing)
   *     TM / BM / LM / RM : Cell[Top / Bottom / Left / Right]Margin
   *     HM / VM : Cell[Horizontal / Vertical]ElementMargin
   * First configuration : SubLabel fits
   * -------------------------------------------------
   * |                     TM                        |
   * | *     *    *   *     *      *       *      *  |
   * | LM  Label  HM  *  SubLabel  HM  Accessory  RM |
   * | *     *    *   *     *      *       *      *  |
   * |                     BM                        |
   * -------------------------------------------------
   * Second configuration : SubLabel does not fit
   * -------------------------------------------------
   * |                     TM                        |
   * | *      *      *             *      *       *  |
   * | LM  Label     *             HM             RM |
   * | LM    VM      *             HM  Accessory  RM |
   * | LM  SubLabel  *             HM             RM |
   * | *      *      *             *      *       *  |
   * |                     BM                        |
   * -------------------------------------------------
   * Third configuration : Sublabel and accessory do not fit side by side
   * -------------------------------------------------
   * |                     TM                        |
   * | *      *      *      *      *      *       *  |
   * | LM  Label     *             HM  Accessory  RM |
   * | LM  VM  VM  VM  VM  VM  VM  VM  VM  VM  VM RM |
   * | LM               SubLabel                  RM |
   * | *      *      *             *      *       *  |
   * |                     BM                        |
   * -------------------------------------------------
   * When isAccessoryAlignedRight returns false, Accessory can be placed on the
   * left of the Cell. Label and SubLabel also take the two configurations
   * depending on the fit.
   * When giveAccessoryAllWidth returns true, Accessory will take all available
   * horizontal space, forcing label and subLabel to layout over two rows.
   */

  constexpr static KDCoordinate k_minimalLargeFontCellHeight = Metric::CellSeparatorThickness + Metric::CellTopMargin + KDFont::GlyphHeight(KDFont::Size::Large) + Metric::CellTopMargin;
  constexpr static KDCoordinate k_minimalSmallFontCellHeight = Metric::CellSeparatorThickness + Metric::CellTopMargin + KDFont::GlyphHeight(KDFont::Size::Small) + Metric::CellTopMargin;

  TableCell();

  // View
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  virtual const View * labelView() const { return nullptr; }
  virtual const View * subLabelView() const { return nullptr; }
  virtual const View * accessoryView() const { return nullptr; }
  virtual bool giveAccessoryAllWidth() const { return false; }
  KDCoordinate minimalHeightForOptimalDisplay(KDCoordinate minAccessoryWidth, KDCoordinate width) const;

  KDCoordinate innerWidth() const { return bounds().width() - 2 * k_separatorThickness - Metric::CellLeftMargin - Metric::CellRightMargin;}

protected:
  // View
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  virtual KDColor backgroundColor() const { return KDColorWhite; }
  virtual bool shouldAlignLabelAndAccessory() const { return false; }
  virtual bool shouldAlignSublabelRight() const { return true; }
  virtual KDCoordinate accessoryMinimalWidthOverridden() const { return -1; }
  virtual bool shouldHideSublabel() { return false; }
  // This method is only used to assert that no subview overlaps after layouting
  virtual bool subviewsCanOverlap() const { return false; }
  bool singleRowMode() const;

private:
  static bool singleRowMode(KDCoordinate width, const View * labelView, const View * sublabelView, KDCoordinate accessoryWidth);
  static KDRect setFrameIfViewExists(View * v, KDRect rect, bool force);
};
}
#endif
