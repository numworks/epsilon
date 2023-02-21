#include "banner_view.h"

#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>

#include "app.h"

using namespace Poincare;

namespace Elements {

// BannerView::DotView

void BannerView::DotView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillAntialiasedCircle(KDPointZero, k_dotDiameter / 2, m_color,
                             k_backgroundColor);
}

void BannerView::DotView::setColor(KDColor color) {
  if (color != m_color) {
    markRectAsDirty(bounds());
    m_color = color;
  }
}

// BannerView

BannerView::BannerView(Escher::Responder* textFieldParent,
                       Escher::TextFieldDelegate* textFieldDelegate)
    : m_textField(textFieldParent, nullptr, textFieldDelegate),
      m_legendView(KDContext::k_alignLeft, KDContext::k_alignCenter,
                   k_legendColor, k_backgroundColor, k_legendSize),
      m_button(k_backgroundColor) {
  m_textField.setLeftMargin(Escher::Metric::CommonSmallMargin);
}

void BannerView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_borderHeight), k_borderColor);
  ctx->fillRect(KDRect(0, k_borderHeight, bounds().width() - k_buttonWidth,
                       k_bannerHeight),
                k_backgroundColor);
}

void BannerView::reload() {
  if (!(displayTextField() || m_textField.bounds().isEmpty())) {
    /* Text field will disappear, we need to redraw the full background */
    markRectAsDirty(bounds());
  }
  layoutSubviews();
}

Escher::View* BannerView::subviewAtIndex(int index) {
  if (displayTextField()) {
    assert(index == 0);
    return &m_textField;
  }
  switch (index) {
    case 0:
      return &m_dotView;
    case 1:
      return &m_legendView;
    default:
      assert(index == 2);
      return &m_button;
  }
}

void BannerView::layoutSubviews(bool force) {
  if (displayTextField()) {
    m_textField.setFrame(KDRect(0, k_borderHeight, bounds().width(),
                                bounds().height() - k_borderHeight),
                         force);
    return;
  }
  m_textField.setFrame(KDRectZero, force);

  ElementsViewDataSource* dataSource = App::app()->elementsViewDataSource();
  KDCoordinate x = k_dotLeftMargin;

  AtomicNumber z = dataSource->selectedElement();
  KDColor buttonColor;
  if (z != ElementsDataBase::k_noElement) {
    KDRect dotRect = KDRect(x, (bounds().height() - k_dotDiameter) / 2,
                            k_dotDiameter, k_dotDiameter);
    m_dotView.setFrame(dotRect, force);
    m_dotView.setColor(dataSource->field()->getColors(z).fg());
    x += dotRect.width() + k_dotLegendMargin;
    buttonColor = k_backgroundColor;
  } else {
    m_dotView.setFrame(KDRectZero, force);
    buttonColor = k_selectedButtonColor;
  }

  m_button.setFrame(KDRect(bounds().width() - k_buttonWidth, k_borderHeight,
                           k_buttonWidth, k_bannerHeight),
                    force);
  m_button.setColor(buttonColor);

  I18n::Message legendMessage = dataSource->field()->getMessage(z);
  if (legendMessage != I18n::Message::Default) {
    m_legendView.setLayout(
        LayoutHelper::String(I18n::translate(legendMessage)));
  } else {
    HorizontalLayout h = HorizontalLayout::Builder();
    h.addOrMergeChildAtIndex(LayoutHelper::String(I18n::translate(
                                 dataSource->field()->fieldLegend())),
                             0);
    if (dataSource->field()->hasDouble(z)) {
      h.addOrMergeChildAtIndex(CodePointLayout::Builder(' '),
                               h.numberOfChildren());
      h.addOrMergeChildAtIndex(dataSource->field()->getLayout(z),
                               h.numberOfChildren());
    }
    m_legendView.setLayout(h);
  }
  KDCoordinate bannerBaseline = k_bannerHeight / 2 + 1;
  KDCoordinate legendY = k_borderHeight + bannerBaseline -
                         m_legendView.layout().baseline(k_legendSize);
  m_legendView.setFrame(
      KDRect(x, legendY, bounds().width() - k_buttonWidth - x,
             m_legendView.layout().layoutSize(k_legendSize).height()),
      force);
}

}  // namespace Elements
