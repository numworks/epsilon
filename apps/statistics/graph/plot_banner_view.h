#ifndef STATISTICS_PLOT_BANNER_VIEW_H
#define STATISTICS_PLOT_BANNER_VIEW_H

#include <apps/shared/editable_field_banner_view_delegate.h>
#include <escher/buffer_text_view.h>
#include <escher/text_field.h>
#include <escher/view.h>

namespace Statistics {

class PlotBannerView : public Shared::BannerView {
 public:
  PlotBannerView();
  BannerBufferTextView* seriesName() { return &m_seriesName; }
  BannerBufferTextView* result() { return &m_result; }

 private:
  constexpr static int k_numberOfSubviews = 3;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  Escher::View* subviewAtIndex(int index) override;
  virtual Escher::View* valueView() = 0;
  BannerBufferTextView m_seriesName;
  BannerBufferTextView m_result;
};

class SimplePlotBannerView : public PlotBannerView {
 public:
  SimplePlotBannerView() : m_value(k_bannerFieldFormat) {}
  BannerBufferTextView* value() { return &m_value; }

 private:
  Escher::View* valueView() override { return &m_value; }
  BannerBufferTextView m_value;
};

class PlotBannerViewWithEditableField
    : public PlotBannerView,
      public Shared::EditableFieldBannerViewDelegate {
 public:
  PlotBannerViewWithEditableField(Escher::Responder* parentResponder,
                                  Escher::TextFieldDelegate* textFieldDelegate)
      : Shared::EditableFieldBannerViewDelegate(parentResponder,
                                                textFieldDelegate) {}
  BannerBufferTextView* valueLabel() { return editableFieldLabel(); }
  Escher::TextField* value() { return editableField(); }

 private:
  Escher::View* valueView() override { return editableView(); }
};

}  // namespace Statistics

#endif
