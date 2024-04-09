#ifndef SHARED_BANNER_VIEW_WITH_EDITABLE_FIELD_H
#define SHARED_BANNER_VIEW_WITH_EDITABLE_FIELD_H

#include <escher/buffer_text_view.h>
#include <escher/responder.h>
#include <escher/text_field.h>
#include <poincare/print_float.h>

#include "banner_view.h"

namespace Shared {

class EditableFieldBannerViewDelegate {
 public:
  EditableFieldBannerViewDelegate(Escher::Responder* parentResponder,
                                  Escher::TextFieldDelegate* textFieldDelegate)
      : m_editableFieldLabel({.style = BannerView::k_bannerFieldFormat.style,
                              .horizontalAlignment = KDGlyph::k_alignRight}),
        m_editableField(parentResponder, m_textBody, k_bufferSize,
                        textFieldDelegate,
                        {.style = BannerView::k_bannerFieldFormat.style,
                         .horizontalAlignment = KDGlyph::k_alignLeft}),
        m_editableView(&m_editableFieldLabel, &m_editableField) {
    m_textBody[0] = 0;
  }

 protected:
  BannerView::BannerBufferTextView* editableFieldLabel() {
    return &m_editableFieldLabel;
  }
  Escher::TextField* editableField() { return &m_editableField; }
  Escher::View* editableView() { return &m_editableView; }

 private:
  constexpr static KDCoordinate k_bufferSize =
      Poincare::PrintFloat::k_maxFloatCharSize;

  BannerView::BannerBufferTextView m_editableFieldLabel;
  Escher::TextField m_editableField;
  BannerView::LabelledView m_editableView;
  char m_textBody[k_bufferSize];
};

}  // namespace Shared

#endif
