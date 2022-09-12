#ifndef ELEMENTS_BANNER_VIEW_H
#define ELEMENTS_BANNER_VIEW_H

#include "elements_view_data_source.h"
#include "suggestion_text_field.h"
#include <escher/ellipsis_view.h>
#include <escher/expression_view.h>
#include <escher/palette.h>
#include <escher/solid_color_view.h>
#include <escher/view.h>
#include <ion/display.h>

namespace Elements {

class BannerView : public Escher::View {
public:
  BannerView(Escher::Responder * textFieldParent, Escher::TextFieldDelegate * textFieldDelegate);

  // Escher::View
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override { return KDSize(Ion::Display::Width, k_bannerHeight + k_borderHeight); }

  void reload();
  SuggestionTextField * textField() { return &m_textField; }

private:
  constexpr static KDColor k_backgroundColor = Escher::Palette::GrayWhite;
  constexpr static KDColor k_borderColor = Escher::Palette::GrayMiddle;
  constexpr static KDColor k_legendColor = Escher::Palette::GrayVeryDark;
  constexpr static KDColor k_selectedButtonColor = Escher::Palette::Select;
  constexpr static KDCoordinate k_dotLeftMargin = 16;
  constexpr static KDCoordinate k_dotDiameter = 8;
  constexpr static KDCoordinate k_bannerHeight = 25;
  constexpr static KDCoordinate k_borderHeight = 1;
  constexpr static KDCoordinate k_dotLegendMargin = 12;
  constexpr static KDCoordinate k_buttonWidth = 37;
  constexpr static KDFont::Size k_legendSize = KDFont::Size::Small;

  class DotView : public Escher::View {
  public:
    DotView() : m_color(k_backgroundColor) {}

    // Escher::View
    void drawRect(KDContext * ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override { return KDSize(k_dotDiameter, k_dotDiameter); }

    void setColor(KDColor color);

  private:
    KDColor m_color;
  };

  class EllipsisButton : public Escher::SolidColorView {
  public:
    using Escher::SolidColorView::SolidColorView;
  private:
    int numberOfSubviews() const override { return 1; }
    Escher::View * subviewAtIndex(int index) override { return &m_ellipsisView; }
    void layoutSubviews(bool force = false) override { m_ellipsisView.setFrame(bounds(), force); }
    Escher::EllipsisView m_ellipsisView;
  };

  int numberOfSubviews() const override { return displayTextField() ? 1 : 3; }
  Escher::View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  bool displayTextField() const { return m_textField.isEditing(); }

  SuggestionTextField m_textField;
  DotView m_dotView;
  Escher::ExpressionView m_legendView;
  EllipsisButton m_button;
};

}

#endif
