#ifndef PROBABILITY_GUI_TEST_CONCLUSION_VIEW_H
#define PROBABILITY_GUI_TEST_CONCLUSION_VIEW_H

#include <escher/expression_view.h>
#include <escher/message_text_view.h>

#include <escher/horizontal_or_vertical_layout.h>
#include "probability/models/statistic/statistic.h"

namespace Probability {

/* This view displays a message "test has been rejected / can't be rejected"
 * and a matching icon. */
class TestConclusionView : public Escher::View {
public:
  TestConclusionView(Statistic * statistic);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void reload();

private:
  constexpr static int k_marginLeft = 20;

  KDSize minimalSizeForOptimalDisplay() const override;
  int numberOfSubviews() const override { return 2; }
  Escher::View * subviewAtIndex(int i) override;
  void layoutSubviews(bool force) override;
  void generateConclusionTexts(bool isTestSuccessfull);

  Statistic * m_statistic;
  Escher::ExpressionView m_textView1;
  Escher::MessageTextView m_textView2;
};

}  // namespace Probability

#endif /* PROBABILITY_GUI_TEST_CONCLUSION_VIEW_H */
