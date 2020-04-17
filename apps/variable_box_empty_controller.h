#ifndef APPS_VARIABLE_BOX_EMPTY_CONTROLLER_H
#define APPS_VARIABLE_BOX_EMPTY_CONTROLLER_H

#include <escher/bordered.h>
#include <escher/expression_view.h>
#include <escher/message_text_view.h>
#include <escher/view_controller.h>
#include <poincare/layout.h>

class VariableBoxEmptyController : public ViewController {
public:
  VariableBoxEmptyController() : ViewController(nullptr) {}
  void setMessages(I18n::Message * messages);
  // View Controller
  DisplayParameter displayParameter() override { return DisplayParameter::DoNotShowOwnTitle; }
protected:
  class VariableBoxEmptyView : public View, public Bordered {
  public:
    constexpr static const KDFont * k_font = KDFont::SmallFont;
    void initMessageViews();
    void setMessages(I18n::Message * message);
    void drawRect(KDContext * ctx, KDRect rect) const override;
  private:
    constexpr static int k_expressionViewRowIndex = 2;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    virtual int numberOfMessageTextViews() const = 0;
    virtual MessageTextView * messageTextViewAtIndex(int index) = 0;
    bool hasExpressionView() const {
      return const_cast<VariableBoxEmptyView *>(this)->expressionView() != nullptr;
    }
    virtual ExpressionView * expressionView() { return nullptr; }
  };
};

#endif
