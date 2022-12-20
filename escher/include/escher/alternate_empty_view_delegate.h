#ifndef ESCHER_ALTERNATE_EMPTY_VIEW_DELEGATE_H
#define ESCHER_ALTERNATE_EMPTY_VIEW_DELEGATE_H

#include <escher/view_controller.h>
#include <escher/message_text_view.h>
#include <escher/palette.h>
#include <escher/i18n.h>

namespace Escher {

class AlternateEmptyViewController;

class AlternateEmptyViewDelegate {
public:
  virtual bool isEmpty() const = 0;
  virtual View * emptyView() = 0;
};

class AlternateEmptyViewDefaultDelegate : public AlternateEmptyViewDelegate {
public:
  AlternateEmptyViewDefaultDelegate() :
      m_message(KDFont::Size::Small,
                (I18n::Message)0,
                KDContext::k_alignCenter,
                KDContext::k_alignCenter,
                KDColorBlack,
                Palette::WallScreen) {}
  virtual I18n::Message emptyMessage() = 0;
  View * emptyView() override {
    m_message.setMessage(emptyMessage());
    return &m_message;
  }
private:
  MessageTextView m_message;
};

}

#endif
