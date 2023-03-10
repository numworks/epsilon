#ifndef ESCHER_TEXT_VIEW_H
#define ESCHER_TEXT_VIEW_H

#include <escher/glyphs_view.h>

namespace Escher {

class TextView : public GlyphsView {
 public:
  using GlyphsView::GlyphsView;
  // View
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  virtual const char* text() const = 0;
  virtual void setText(const char* text) = 0;

#if ESCHER_VIEW_LOGGING
 protected:
  const char* className() const override { return "TextView"; }
#endif
};

}  // namespace Escher
#endif
