#ifndef ESCHER_WARNING_CONTROLLER_H
#define ESCHER_WARNING_CONTROLLER_H

#include <escher/i18n.h>
#include <escher/message_text_view.h>
#include <escher/solid_color_view.h>
#include <escher/view_controller.h>

namespace Escher {

class WarningController : public ViewController {
 public:
  WarningController(Responder* parentResponder, I18n::Message warningMessage1,
                    I18n::Message warningMessage2 = (I18n::Message)0);
  void setLabel(I18n::Message message1, I18n::Message message2,
                bool specialExitKeys);
  const char* title() override;
  View* view() override;
  bool handleEvent(Ion::Events::Event event) override;

 private:
  class ContentView : public SolidColorView {
   public:
    ContentView();
    void setLabels(I18n::Message message1, I18n::Message message2);
    int numberOfSubviews() const override;
    View* subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    KDSize minimalSizeForOptimalDisplay() const override;

   private:
    constexpr static KDCoordinate k_topAndBottomMargin = 20;
    constexpr static KDCoordinate k_middleMargin = 10;
    constexpr static KDCoordinate k_horizontalMargin = 20;
    // = (k_topAndBottomMargin + textHeight/2)/(k_topAndBottomMargin +
    // textHeight + 0.5*k_middleMargin)
    constexpr static float k_shiftedAlignment = 0.7f;
    MessageTextView m_textView1;
    MessageTextView m_textView2;
  };

  ContentView m_contentView;
  I18n::Message m_warningMessage1;
  bool m_exitOnOKBackEXEOnly;
};

}  // namespace Escher
#endif
