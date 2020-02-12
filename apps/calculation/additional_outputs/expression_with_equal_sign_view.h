#ifndef CALCULATION_EXPRESSION_WITH_EQUAL_SIGN_VIEW_H
#define CALCULATION_EXPRESSION_WITH_EQUAL_SIGN_VIEW_H

#include <escher.h>
#include <apps/i18n.h>
#include <poincare/layout.h>

namespace Calculation {

class ExpressionWithEqualSignView : public ExpressionView {
public:
  ExpressionWithEqualSignView() :
    m_equalSign(KDFont::LargeFont, I18n::Message::Equal, 0.5f, 0.5f, KDColorBlack)
  {}
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  int numberOfSubviews() const override { return 1; }
  MessageTextView m_equalSign;
};

}

#endif
