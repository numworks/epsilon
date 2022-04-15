#ifndef PROBABILITY_GUI_TEST_CONCLUSION_VIEW_H
#define PROBABILITY_GUI_TEST_CONCLUSION_VIEW_H

#include <escher/expression_view.h>
#include <escher/message_text_view.h>
#include <escher/horizontal_or_vertical_layout.h>

namespace Inference {

/* This view displays a message "test has been rejected / can't be rejected"
 * and a matching icon. */
class TestConclusionView : public Escher::View {
public:
  TestConclusionView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void generateConclusionTexts(bool isTestSuccessfull);

private:
  constexpr static int k_marginLeft = 20;

  KDSize minimalSizeForOptimalDisplay() const override;
  int numberOfSubviews() const override { return 2; }
  Escher::View * subviewAtIndex(int i) override;
  void layoutSubviews(bool force) override;

  Escher::ExpressionView m_textView1;
  Escher::MessageTextView m_textView2;
};

}  // namespace Inference

#endif /* PROBABILITY_GUI_TEST_CONCLUSION_VIEW_H */
