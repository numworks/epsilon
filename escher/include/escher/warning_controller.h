#ifndef ESCHER_WARNING_CONTROLLER_H
#define ESCHER_WARNING_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/pointer_text_view.h>
#include <escher/solid_color_view.h>

class WarningController : public ViewController {
public:
  WarningController(Responder * parentResponder);
  void setLabel(const char *);
  const char * title() const override;
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  class ContentView : public SolidColorView {
  public:
    ContentView();
    void setLabel(const char *);
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    KDSize minimalSizeForOptimalDisplay() const override;
  private:
    constexpr static KDCoordinate k_verticalMargin = 40;
    constexpr static KDCoordinate k_horizontalMargin = 20;
    PointerTextView m_textView;
  };

  ContentView m_contentView;
};

#endif
