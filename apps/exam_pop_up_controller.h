#ifndef APPS_EXAM_POP_UP_CONTROLLER_H
#define APPS_EXAM_POP_UP_CONTROLLER_H

#include <escher.h>

class ExamPopUpController : public ViewController {
public:
  ExamPopUpController();
  View * view() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  void setActivatingExamMode(bool activingExamMode);
  bool isActivatingExamMode();
private:
  class ContentView : public View {
  public:
    ContentView(Responder * parentResponder);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void setSelectedButton(int selectedButton, App * app);
    int selectedButton();
    void setMessages(bool activingExamMode);
  private:
    constexpr static KDCoordinate k_buttonMargin = 10;
    constexpr static KDCoordinate k_buttonHeight = 20;
    constexpr static KDCoordinate k_topMargin = 12;
    constexpr static KDCoordinate k_paragraphHeight = 20;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    Button m_cancelButton;
    Button m_okButton;
    MessageTextView m_warningTextView;
    MessageTextView m_messageTextView1;
    MessageTextView m_messageTextView2;
    MessageTextView m_messageTextView3;
  };
  ContentView m_contentView;
  bool m_isActivatingExamMode;
};

#endif

