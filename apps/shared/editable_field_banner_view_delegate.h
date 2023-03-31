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
  EditableFieldBannerViewDelegate(
      Escher::Responder* parentResponder,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
      Escher::TextFieldDelegate* textFieldDelegate)
      : m_editableFieldLabel(
            {.style = {.glyphColor = BannerView::TextColor(),
                       .backgroundColor = BannerView::BackgroundColor(),
                       .font = BannerView::k_font},
             .horizontalAlignment = KDGlyph::k_alignRight}),
        m_editableField(parentResponder, m_textBody, k_bufferSize,
                        inputEventHandlerDelegate, textFieldDelegate,
                        BannerView::k_font, KDGlyph::k_alignLeft,
                        KDGlyph::k_alignCenter, BannerView::TextColor(),
                        BannerView::BackgroundColor()),
        m_editableView(&m_editableFieldLabel, &m_editableField) {
    m_textBody[0] = 0;
  }

 protected:
  Escher::BufferTextView* editableFieldLabel() { return &m_editableFieldLabel; }
  Escher::TextField* editableField() { return &m_editableField; }
  Escher::View* editablView() { return &m_editableView; }

 private:
  constexpr static KDCoordinate k_bufferSize =
      Poincare::PrintFloat::k_maxFloatCharSize;
  Escher::BufferTextView m_editableFieldLabel;
  Escher::TextField m_editableField;
  BannerView::LabelledView m_editableView;
  char m_textBody[k_bufferSize];
};

}  // namespace Shared

#endif
