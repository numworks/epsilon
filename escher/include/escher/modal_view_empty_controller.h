#ifndef ESCHER_EMPTY_MODAL_VIEW_EMPTY_CONTROLLER_H
#define ESCHER_EMPTY_MODAL_VIEW_EMPTY_CONTROLLER_H

#include <escher/bordered.h>
#include <escher/layout_view.h>
#include <escher/message_text_view.h>
#include <escher/view_controller.h>

namespace Escher {

class ModalViewEmptyController : public ViewController {
 public:
  ModalViewEmptyController(I18n::Message message)
      : ViewController(nullptr), m_view(message) {}
  TitlesDisplay titlesDisplay() const override {
    return TitlesDisplay::NeverDisplayOwnTitle;
  }
  View* view() override { return &m_view; }

 protected:
  class ModalViewEmptyView : public View, public Bordered {
   public:
    ModalViewEmptyView(I18n::Message message)
        : View(), m_messageTextView(message, k_format) {}
    void drawRect(KDContext* ctx, KDRect rect) const override;

   private:
    constexpr static KDColor k_backgroundColor = Palette::WallScreen;
    constexpr static KDFont::Size k_font = KDFont::Size::Small;
    constexpr static KDGlyph::Format k_format = {
        .style = {.backgroundColor = k_backgroundColor, .font = k_font},
        .horizontalAlignment = KDGlyph::k_alignCenter,
        .verticalAlignment = KDGlyph::k_alignCenter};

    int numberOfSubviews() const override { return 1; }
    View* subviewAtIndex(int index) override {
      assert(index == 0);
      return &m_messageTextView;
    }
    void layoutSubviews(bool force = false) override;

    MessageTextView m_messageTextView;
  };
  ModalViewEmptyView m_view;
};

}  // namespace Escher
#endif
