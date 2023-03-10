#ifndef CALCULATION_EXPRESSION_WITH_EQUAL_SIGN_VIEW_H
#define CALCULATION_EXPRESSION_WITH_EQUAL_SIGN_VIEW_H

#include <apps/i18n.h>
#include <escher/expression_view.h>
#include <escher/message_text_view.h>
#include <poincare/layout.h>

namespace Calculation {

class ExpressionWithEqualSignView : public Escher::ExpressionView {
 public:
  ExpressionWithEqualSignView()
      : m_equalSign(
            I18n::Message::Equal,
            {{.font = k_font}, .horizontalAlignment = KDGlyph::k_alignCenter}),
        m_showEqual(true) {}
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext* ctx, KDRect rect) const override;
  void setShowEqual(bool showEqual) { m_showEqual = showEqual; }

 private:
  constexpr static KDFont::Size k_font = KDFont::Size::Large;
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  int numberOfSubviews() const override { return m_showEqual; }
  Escher::MessageTextView m_equalSign;
  bool m_showEqual;
};

}  // namespace Calculation

#endif
