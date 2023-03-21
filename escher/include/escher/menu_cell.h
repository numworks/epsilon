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
  /* MenuCells are laid out as follow :
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

  static KDColor BackgroundColor(bool highlighted) {
    return highlighted ? Palette::Select : KDColorWhite;
  }

  AbstractMenuCell();

  // View
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  virtual CellWidget* label() = 0;
  virtual CellWidget* subLabel() = 0;
  virtual CellWidget* accessory() = 0;

  const View* labelView() const { return constLabel()->view(); }
  const View* subLabelView() const { return constSubLabel()->view(); }
  const View* accessoryView() const { return constAccessory()->view(); }

  KDCoordinate minimalHeightForOptimalDisplay() const;

  KDCoordinate innerWidth() const {
    return bounds().width() - k_leftOffset - k_rightOffset;
  }
  KDCoordinate innerHeight() const {
    return bounds().height() - k_topOffset - k_bottomOffset;
  }

  Responder* responder() override;
  void setHighlighted(bool highlight) override;

  bool enterOnEvent(Ion::Events::Event event) {
    // This is only done on accessory for now but could also done on (sub)label
    return accessory()->enterOnEvent(event);
  }

 protected:
  // View
  int numberOfSubviews() const override;
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  void initWidgets() {
    label()->defaultInitialization(CellWidget::Type::Label);
    subLabel()->defaultInitialization(CellWidget::Type::SubLabel);
    accessory()->defaultInitialization(CellWidget::Type::Accessory);
  }

  bool forceAlignLabelAndAccessory() const {
    return constAccessory()->alwaysAlignWithLabelAsAccessory();
  }
  bool shouldAlignSublabelRight() const {
    return !constLabel()->preventRightAlignedSubLabel(
               CellWidget::Type::Label) &&
           !constSubLabel()->preventRightAlignedSubLabel(
               CellWidget::Type::SubLabel) &&
           !constAccessory()->preventRightAlignedSubLabel(
               CellWidget::Type::Accessory);
  }
  bool shouldHideSublabel() const;
  bool singleRowMode() const;

 private:
  const CellWidget* constLabel() const {
    return const_cast<AbstractMenuCell*>(this)->label();
  }
  const CellWidget* constSubLabel() const {
    return const_cast<AbstractMenuCell*>(this)->subLabel();
  }
  const CellWidget* constAccessory() const {
    return const_cast<AbstractMenuCell*>(this)->accessory();
  }

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

  /* Editable text field has unique properties. This avoid having too many
   * virtual functions, only for this specific case. */
  bool accessoryIsAnEditableTextField() const {
    return constAccessory()->isAnEditableTextField();
  }

  bool shouldAlignLabelAndAccessory() const;

  KDRect setFrameIfViewExists(View* v, KDRect rect, bool force);
};

template <typename Label, typename SubLabel = EmptyCellWidget,
          typename Accessory = EmptyCellWidget>
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
  MenuCell() : AbstractMenuCell() { initWidgets(); }

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
