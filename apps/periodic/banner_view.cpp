#include "banner_view.h"
#include "app.h"
#include <assert.h>

namespace Periodic {

// BannerView::DotView

void BannerView::DotView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillAntialiasedCircle(KDPointZero, k_dotDiameter / 2, m_color, k_backgroundColor);
}

void BannerView::DotView::setColor(KDColor color) {
  if (color != m_color) {
    markRectAsDirty(bounds());
    m_color = color;
  }
}

// BannerView

BannerView::BannerView(Escher::Responder * textFieldParent, Escher::TextFieldDelegate * textFieldDelegate) :
  m_textField(textFieldParent, nullptr, textFieldDelegate),
  m_textView(KDFont::Size::Small, KDContext::k_alignLeft, KDContext::k_alignCenter, k_legendColor, k_backgroundColor),
  m_button(k_backgroundColor)
{
  m_textField.setLeftMargin(Escher::Metric::CommonSmallMargin);
}

void BannerView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_borderHeight), k_borderColor);
  ctx->fillRect(KDRect(0, k_borderHeight, bounds().width(), k_bannerHeight), k_backgroundColor);
}

void BannerView::reload() {
  if (!(displayTextField() || m_textField.bounds().isEmpty())) {
    /* Text field will disappear, we need to redraw the full background */
    markRectAsDirty(bounds());
  }
  layoutSubviews();
}

Escher::View * BannerView::subviewAtIndex(int index) {
  if (displayTextField()) {
    assert(index == 0);
    return &m_textField;
  }
  switch (index) {
  case 0:
    return &m_dotView;
  case 1:
    return &m_textView;
  default:
    assert(index == 2);
    return &m_button;
  }
}

void BannerView::layoutSubviews(bool force) {
  if (displayTextField()) {
    m_textField.setFrame(KDRect(0, k_borderHeight, bounds().width(), bounds().height() - k_borderHeight), force);
    return;
  }
  m_textField.setFrame(KDRectZero, force);

  ElementsViewDataSource * dataSource = App::app()->elementsViewDataSource();
  KDCoordinate x = k_dotLeftMargin;

  AtomicNumber z = dataSource->selectedElement();
  KDColor buttonColor;
  if (z != ElementsDataBase::k_noElement) {
    KDRect dotRect = KDRect(x, (bounds().height() - k_dotDiameter) / 2, k_dotDiameter, k_dotDiameter);
    m_dotView.setFrame(dotRect, force);
    m_dotView.setColor(dataSource->field()->getColors(z).fg());
    x += dotRect.width() + k_dotLegendMargin;
    buttonColor = k_backgroundColor;
  } else {
    m_dotView.setFrame(KDRectZero, force);
    buttonColor = k_selectedButtonColor;
  }

  m_button.setFrame(KDRect(bounds().width() - k_buttonWidth, k_borderHeight, k_buttonWidth, k_bannerHeight), force);
  m_button.setColor(buttonColor);

  I18n::Message legendMessage = dataSource->field()->getMessage(z);
  if (legendMessage != I18n::Message::Default) {
    m_textView.setText(I18n::translate(legendMessage));
  } else {
    m_textView.setText(I18n::translate(dataSource->field()->fieldLegend()));
    if (dataSource->field()->hasDouble(z)) {
      m_textView.appendText(I18n::translate(I18n::Message::ColonConvention));
      char buffer[Escher::BufferTextView::k_maxNumberOfChar];
      dataSource->field()->getLayout(z).serializeForParsing(buffer, Escher::BufferTextView::k_maxNumberOfChar);
      m_textView.appendText(buffer);
    }
  }
  m_textView.setFrame(KDRect(x, k_borderHeight, bounds().width() - x, k_bannerHeight), force);
}

}
