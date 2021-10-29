#ifndef PROBABILITY_GUI_TEST_CONCLUSION_VIEW_H
#define PROBABILITY_GUI_TEST_CONCLUSION_VIEW_H

#include <escher/expression_view.h>
#include <escher/message_text_view.h>

#include "probability/gui/highlight_image_cell.h"
#include <escher/horizontal_or_vertical_layout.h>
#include "probability/models/statistic/statistic.h"

namespace Probability {

/* This view displays a message "test has been rejected / can't be rejected"
 * and a matching icon. */
class TestConclusionView : public Escher::View {
public:
  TestConclusionView(Statistic * statistic);
  int numberOfSubviews() const override { return 3; }
  Escher::View * subviewAtIndex(int i) override;
  void layoutSubviews(bool force) override;
  KDSize minimalSizeForOptimalDisplay() const override;

  void drawRect(KDContext * ctx, KDRect rect) const override;

  void reload();

private:
  void generateConclusionTexts(bool isTestSuccessfull);

  constexpr static int k_marginBetween = 0;
  constexpr static int k_marginLeft = 20;
  constexpr static int k_iconSize = 14;

  Statistic * m_statistic;
  HighlightImageCell m_icon;
  Escher::ExpressionView m_textView1;
  Escher::MessageTextView m_textView2;
};

}  // namespace Probability

#endif /* PROBABILITY_GUI_TEST_CONCLUSION_VIEW_H */
