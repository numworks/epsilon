#ifndef APPS_PROBABILITY_GUI_TEST_CONCLUSION_VIEW_H
#define APPS_PROBABILITY_GUI_TEST_CONCLUSION_VIEW_H

#include <escher/message_text_view.h>
#include <escher/palette.h>
#include <escher/view.h>

#include "probability/gui/horizontal_or_vertical_layout.h"

namespace Probability {

/* This view displays a message "test has been rejected / can't be rejected"
 * and a little checkmark or cross */
class TestConclusionView : public HorizontalLayout {
public:
  enum class Type { Success, Failure };
  TestConclusionView() { m_textView.setBackgroundColor(Escher::Palette::WallScreen); }
  void setType(Type t);
  int numberOfSubviews() const override { return 2; }
  Escher::View * subviewAtIndex(int i) override;
  void layoutSubviews(bool force) override;
  KDSize minimalSizeForOptimalDisplay() const override;

private:
  constexpr static int k_marginBetween = 20;
  constexpr static int k_marginLeft = 20;
  class Icon : public Escher::View {
  public:
    constexpr static int k_size = 14;
    void setType(Type t) { m_type = t; }
    void drawRect(KDContext * ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override;

  private:
    Type m_type;
  };

  Icon m_icon;
  Escher::MessageTextView m_textView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_TEST_CONCLUSION_VIEW_H */
