#ifndef CALCULATION_EXPRESSION_WITH_EQUAL_SIGN_VIEW_H
#define CALCULATION_EXPRESSION_WITH_EQUAL_SIGN_VIEW_H

#include <escher.h>
#include <apps/i18n.h>
#include <poincare/layout.h>

namespace Calculation {

class ExpressionWithEqualSignView : public View {
public:
  ExpressionWithEqualSignView() :
    m_expressionView(),
    m_equalSign(KDFont::LargeFont, I18n::Message::Equal, 0.5f, 0.5f, KDColorBlack)
  {}
  ExpressionView * expressionView() { return &m_expressionView; }
  KDSize minimalSizeForOptimalDisplay() const override;
  void setBackgroundColor(KDColor color) { m_expressionView.setBackgroundColor(color); }
  Poincare::Layout layout() const { return m_expressionView.layout(); }
  bool setLayout(Poincare::Layout layout) { return m_expressionView.setLayout(layout); }
private:
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  int numberOfSubviews() const override { return 2; }
  ExpressionView m_expressionView;
  MessageTextView m_equalSign;
};

}

#endif
