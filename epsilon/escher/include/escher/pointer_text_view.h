#pragma once

#include <escher/i18n.h>
#include <escher/text_view.h>

#include "raw_color_glyphs_view.h"

namespace Escher {

class PointerTextView : public TextView {
 public:
  PointerTextView(const char* text = nullptr, const TextFormat& format = {});
  const char* text() const override { return m_text; }
  void setText(const char* text) override;

 private:
  const char* m_text;
};

class RawColorPointerTextView : public RawColor<PointerTextView> {
 public:
  RawColorPointerTextView(const char* text = nullptr,
                          TextFormatWithoutColors format = {},
                          TextColors colors = {})
      : RawColor<PointerTextView>(text, MakeTextFormat(format)) {
    this->m_rawColors = colors;
  }
};

}  // namespace Escher
