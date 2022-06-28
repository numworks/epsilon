#ifndef CALCULATION_EXPRESSION_WITH_EQUAL_SIGN_VIEW_H
#define CALCULATION_EXPRESSION_WITH_EQUAL_SIGN_VIEW_H

#include <apps/i18n.h>
#include <escher/expression_view.h>
#include <escher/message_text_view.h>
#include <poincare/layout.h>

namespace Calculation {

class ExpressionWithEqualSignView : public Escher::ExpressionView {
public:
  ExpressionWithEqualSignView() :
    m_equalSign(k_font, I18n::Message::Equal, KDContext::k_alignCenter, KDContext::k_alignCenter, KDColorBlack)
  {}
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDFont::Size k_font = KDFont::Size::Large;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  int numberOfSubviews() const override { return 1; }
  Escher::MessageTextView m_equalSign;
};

}

#endif
