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

  virtual CellWidget* widget(CellWidget::Type type) = 0;
  const CellWidget* constWidget(CellWidget::Type type) const {
    return const_cast<AbstractMenuCell*>(this)->widget(type);
  }

  const View* labelView() const {
    return constWidget(CellWidget::Type::Label)->view();
  }
  const View* subLabelView() const {
    return constWidget(CellWidget::Type::SubLabel)->view();
  }
  const View* accessoryView() const {
    return constWidget(CellWidget::Type::Accessory)->view();
  }

  KDCoordinate minimalHeightForOptimalDisplay() const;

  KDCoordinate innerWidth() const {
    return bounds().width() - k_leftOffset - k_rightOffset;
  }
  KDCoordinate innerHeight() const {
    return bounds().height() - k_topOffset - k_bottomOffset;
  }

  Responder* responder() override;
  void setHighlighted(bool highlight) override;

  // This is only done on accessory for now but could also done on (sub)label
  const char* text() const override {
    return constWidget(CellWidget::Type::Accessory)->text();
  }
  Poincare::Layout layout() const override {
    return constWidget(CellWidget::Type::Accessory)->layout();
  }

  bool canBeActivatedByEvent(Ion::Events::Event event) {
    // This is only done on accessory for now but could also done on (sub)label
    return widget(CellWidget::Type::Accessory)->canBeActivatedByEvent(event);
  }

 protected:
  // View
  int numberOfSubviews() const override;
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  void initWidgets() {
    widget(CellWidget::Type::Label)
        ->defaultInitialization(CellWidget::Type::Label);
    widget(CellWidget::Type::SubLabel)
        ->defaultInitialization(CellWidget::Type::SubLabel);
    widget(CellWidget::Type::Accessory)
        ->defaultInitialization(CellWidget::Type::Accessory);
  }

  bool forceAlignLabelAndAccessory() const {
    return constWidget(CellWidget::Type::Accessory)
        ->alwaysAlignWithLabelAsAccessory();
  }
  bool shouldAlignSublabelRight() const {
    return !constWidget(CellWidget::Type::Label)
                ->preventRightAlignedSubLabel(CellWidget::Type::Label) &&
           !constWidget(CellWidget::Type::SubLabel)
                ->preventRightAlignedSubLabel(CellWidget::Type::SubLabel) &&
           !constWidget(CellWidget::Type::Accessory)
                ->preventRightAlignedSubLabel(CellWidget::Type::Accessory);
  }
  bool shouldHideSublabel() const;
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

  /* Editable text field has unique properties. This avoid having too many
   * virtual functions, only for this specific case. */
  bool accessoryIsAnEditableTextField() const {
    return constWidget(CellWidget::Type::Accessory)->isAnEditableTextField();
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

  Label* label() { return &m_label; }
  SubLabel* subLabel() { return &m_subLabel; }
  Accessory* accessory() { return &m_accessory; }

  CellWidget* widget(CellWidget::Type type) override {
    return type == CellWidget::Type::Label
               ? static_cast<CellWidget*>(label())
               : (type == CellWidget::Type::SubLabel
                      ? static_cast<CellWidget*>(subLabel())
                      : static_cast<CellWidget*>(accessory()));
  }

 protected:
  Label m_label;
  SubLabel m_subLabel;
  Accessory m_accessory;
};

}  // namespace Escher
#endif
