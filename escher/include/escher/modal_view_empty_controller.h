#ifndef ESCHER_EMPTY_MODAL_VIEW_EMPTY_CONTROLLER_H
#define ESCHER_EMPTY_MODAL_VIEW_EMPTY_CONTROLLER_H

#include <escher/bordered.h>
#include <escher/expression_view.h>
#include <escher/message_text_view.h>
#include <escher/view_controller.h>

namespace Escher {

class ModalViewEmptyController : public ViewController {
public:
  ModalViewEmptyController() : ViewController(nullptr) {}
  void setMessages(I18n::Message * messages);
  // View Controller
  TitlesDisplay titlesDisplay() override { return TitlesDisplay::NeverDisplayOwnTitle; }
protected:
  class ModalViewEmptyView : public View, public Bordered {
  public:
    constexpr static const KDFont * k_font = KDFont::SmallFont;
    void initMessageViews();
    void setMessages(I18n::Message * message);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void reload();
  private:
    constexpr static int k_expressionViewRowIndex = 2;
    constexpr static KDColor k_backgroundColor = Palette::WallScreen;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    virtual int numberOfMessageTextViews() const = 0;
    virtual MessageTextView * messageTextViewAtIndex(int index) = 0;
    bool hasExpressionView() const {
      return const_cast<ModalViewEmptyView *>(this)->expressionView() != nullptr;
    }
    virtual ExpressionView * expressionView() { return nullptr; }
  };
};

}
#endif
