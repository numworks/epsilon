#ifndef ESCHER_MENU_CELL_H
#define ESCHER_MENU_CELL_H

#include <escher/bordered.h>
#include <escher/cell_widget.h>
#include <escher/highlight_cell.h>

#include <algorithm>
namespace Escher {

// This name is temporary
class AbstractMenuCell : public Bordered, public HighlightCell {
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

  constexpr static KDCoordinate k_minimalLargeFontCellHeight =
      Metric::CellSeparatorThickness + Metric::CellTopMargin +
      KDFont::GlyphHeight(KDFont::Size::Large) + Metric::CellTopMargin;
  constexpr static KDCoordinate k_minimalSmallFontCellHeight =
      Metric::CellSeparatorThickness + Metric::CellTopMargin +
      KDFont::GlyphHeight(KDFont::Size::Small) + Metric::CellTopMargin;

  constexpr static KDCoordinate k_leftOffset =
      k_separatorThickness + Metric::CellLeftMargin;
  constexpr static KDCoordinate k_rightOffset =
      k_separatorThickness + Metric::CellRightMargin;
  constexpr static KDCoordinate k_topOffset =
      k_separatorThickness + Metric::CellTopMargin;
  constexpr static KDCoordinate k_bottomOffset = Metric::CellBottomMargin;
  constexpr static KDCoordinate k_innerHorizontalMargin =
      Metric::CellHorizontalElementMargin;
  constexpr static KDCoordinate k_innerVerticalMargin =
      Metric::CellVerticalElementMargin;

  AbstractMenuCell();

  // View
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  virtual CellWidget* label() = 0;
  virtual CellWidget* subLabel() = 0;
  virtual CellWidget* accessory() = 0;

  const View* labelView() const {
    return const_cast<AbstractMenuCell*>(this)->label()->view();
  }
  const View* subLabelView() const {
    return const_cast<AbstractMenuCell*>(this)->subLabel()->view();
  }
  const View* accessoryView() const {
    return const_cast<AbstractMenuCell*>(this)->accessory()->view();
  }

  virtual bool giveAccessoryAllWidth() const { return false; }
  KDCoordinate minimalHeightForOptimalDisplay() const;

  KDCoordinate innerWidth() const {
    return bounds().width() - k_leftOffset - k_rightOffset;
  }
  KDCoordinate innerHeight() const {
    return bounds().height() - k_topOffset - k_bottomOffset;
  }

  void setHighlighted(bool highlight) override;
  void setBackgroundColor(KDColor color);

  bool enterOnEvent(Ion::Events::Event event) {
    return event == Ion::Events::OK || event == Ion::Events::EXE ||
           accessory()->enterOnEvent(event);
  }

 protected:
  // View
  int numberOfSubviews() const override;
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  virtual KDColor backgroundColor() const { return KDColorWhite; }
  virtual bool forceAlignLabelAndAccessory() const { return false; }
  virtual bool shouldAlignSublabelRight() const { return true; }
  virtual KDCoordinate accessoryMinimalWidthOverridden() const { return -1; }
  virtual bool shouldHideSublabel() { return false; }
  // This method is only used to assert that no subview overlaps after layouting
  virtual bool subviewsCanOverlap() const { return false; }
  bool singleRowMode() const;

 private:
  KDSize labelSize() const {
    return labelView() ? labelView()->minimalSizeForOptimalDisplay()
                       : KDSizeZero;
  }
  KDSize subLabelSize() const {
    return subLabelView() ? subLabelView()->minimalSizeForOptimalDisplay()
                          : KDSizeZero;
  }
  KDSize accessorySize() const {
    return accessoryView() ? accessoryView()->minimalSizeForOptimalDisplay()
                           : KDSizeZero;
  }

  bool shouldAlignLabelAndAccessory() const;
  KDCoordinate accessoryMinimalWidth() const {
    return std::max(accessoryMinimalWidthOverridden(), accessorySize().width());
  }

  KDRect setFrameIfViewExists(View* v, KDRect rect, bool force);

  KDColor m_backgroundColor;
};

template <typename Label, typename SubLabel, typename Accessory>
class MenuCell : public AbstractMenuCell {
 public:
#ifndef PLATFORM_DEVICE
  // Poor's man constraint
  static_assert(std::is_base_of<CellWidget, Label>(),
                "Label is not a CellWidget");
  static_assert(std::is_base_of<CellWidget, SubLabel>(),
                "Sublabel is not a CellWidget");
  static_assert(std::is_base_of<CellWidget, Accessory>(),
                "Accessory is not a CellWidget");
#endif

  Label* label() override { return &m_label; }
  SubLabel* subLabel() override { return &m_subLabel; }
  Accessory* accessory() override { return &m_accessory; }

 protected:
  Label m_label;
  SubLabel m_subLabel;
  Accessory m_accessory;
};

}  // namespace Escher
#endif
