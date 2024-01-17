#ifndef ESCHER_WARNING_CONTROLLER_H
#define ESCHER_WARNING_CONTROLLER_H

#include <escher/i18n.h>
#include <escher/message_text_view.h>
#include <escher/solid_color_view.h>
#include <escher/view_controller.h>

namespace Escher {

class WarningController : public ViewController {
 public:
  WarningController(Responder* parentResponder, I18n::Message titleMessage);
  void setLabel(I18n::Message message, bool specialExitKeys);
  const char* title() override;
  View* view() override;
  bool handleEvent(Ion::Events::Event event) override;

 private:
  class ContentView : public SolidColorView {
   public:
    ContentView();
    void setLabel(I18n::Message message);
    int numberOfSubviews() const override;
    View* subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    KDSize minimalSizeForOptimalDisplay() const override;

   private:
    constexpr static KDCoordinate k_margin = 15;
    MessageTextView m_textView;
  };

  ContentView m_contentView;
  I18n::Message m_titleMessage;
  bool m_exitOnOKBackEXEOnly;
};

}  // namespace Escher
#endif
