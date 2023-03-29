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
  Escher::BufferTextView* seriesName() { return &m_seriesName; }
  Escher::BufferTextView* result() { return &m_result; }

 private:
  constexpr static int k_numberOfSubviews = 3;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  Escher::View* subviewAtIndex(int index) override;
  virtual Escher::View* valueView() = 0;
  Escher::BufferTextView m_seriesName;
  Escher::BufferTextView m_result;
};

class SimplePlotBannerView : public PlotBannerView {
 public:
  SimplePlotBannerView() : m_value(k_bannerFieldFormat) {}
  Escher::BufferTextView* value() { return &m_value; }

 private:
  Escher::View* valueView() override { return &m_value; }
  Escher::BufferTextView m_value;
};

class PlotBannerViewWithEditableField
    : public PlotBannerView,
      public Shared::EditableFieldBannerViewDelegate {
 public:
  PlotBannerViewWithEditableField(
      Escher::Responder* parentResponder,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
      Escher::TextFieldDelegate* textFieldDelegate)
      : Shared::EditableFieldBannerViewDelegate(
            parentResponder, inputEventHandlerDelegate, textFieldDelegate) {}
  Escher::BufferTextView* valueLabel() { return editableFieldLabel(); }
  Escher::TextField* value() { return editableField(); }

 private:
  Escher::View* valueView() override { return editablView(); }
};

}  // namespace Statistics

#endif
