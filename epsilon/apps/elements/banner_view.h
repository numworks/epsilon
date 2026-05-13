#pragma once

#include <escher/ellipsis_view.h>
#include <escher/layout_view.h>
#include <escher/palette.h>
#include <escher/solid_color_view.h>
#include <escher/view.h>
#include <ion/display.h>

#include "elements_view_data_source.h"
#include "suggestion_text_field.h"

namespace Elements {

class BannerView : public Escher::View {
 public:
  BannerView(Escher::Responder* textFieldParent,
             Escher::TextFieldDelegate* textFieldDelegate);

  // Escher::View
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override {
    return KDSize(Ion::Display::Width, k_bannerHeight + k_borderHeight);
  }

  void reload();
  SuggestionTextField* textField() { return &m_textField; }

 private:
  constexpr static Escher::ThemeComponent k_backgroundColor =
      Escher::ThemeComponent::BannerGrayWhite;
  constexpr static Escher::ThemeComponent k_borderColor =
      Escher::ThemeComponent::BannerGrayMiddle;
  constexpr static Escher::ThemeComponent k_legendColor =
      Escher::ThemeComponent::TextGrayVeryDark;
  constexpr static Escher::ThemeComponent k_selectedButtonColor =
      Escher::ThemeComponent::SurfaceSelect;
  constexpr static KDCoordinate k_dotLeftMargin = 16;
  constexpr static KDCoordinate k_dotDiameter = 8;
  constexpr static KDCoordinate k_bannerHeight = 25;
  constexpr static KDCoordinate k_borderHeight = 1;
  constexpr static KDCoordinate k_dotLegendMargin = 12;
  constexpr static KDCoordinate k_buttonWidth = 37;
  constexpr static KDFont::Size k_legendSize = KDFont::Size::Small;

  class DotView : public Escher::View {
   public:
    DotView() : m_color() {}

    // Escher::View
    void drawRect(KDContext* ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override {
      return KDSize(k_dotDiameter, k_dotDiameter);
    }

    void setColor(KDColor color);

   private:
    KDColor m_color = Escher::Palette::White;
  };

  class EllipsisButton : public Escher::SolidColorView {
   public:
    using Escher::SolidColorView::SolidColorView;

    void setHighlighted(bool highlight) {
      m_ellipsisView.setHighlighted(highlight);
    }

   private:
    int numberOfSubviews() const override { return 1; }
    Escher::View* subviewAtIndex(int index) override {
      assert(index == 0);
      return &m_ellipsisView;
    }
    void layoutSubviews(bool force = false) override {
      setChildFrame(&m_ellipsisView, bounds(), force);
    }
    Escher::EllipsisView m_ellipsisView;
  };

  int numberOfSubviews() const override { return displayTextField() ? 1 : 3; }
  Escher::View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  bool displayTextField() const { return m_textField.isEditing(); }

  SuggestionTextField m_textField;
  DotView m_dotView;
  Escher::LayoutView m_legendView;
  EllipsisButton m_button;
};

}  // namespace Elements
