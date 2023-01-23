#ifndef ESCHER_ALTERNATE_EMPTY_VIEW_DELEGATE_H
#define ESCHER_ALTERNATE_EMPTY_VIEW_DELEGATE_H

#include <escher/view_controller.h>
#include <escher/message_text_view.h>
#include <escher/palette.h>
#include <escher/i18n.h>

namespace Escher {

class AlternateEmptyViewDelegate {
public:
  AlternateEmptyViewDelegate() :
    m_message(KDFont::Size::Small, (I18n::Message)0, KDContext::k_alignCenter, KDContext::k_alignCenter, KDColorBlack, Palette::WallScreen)
  {}
  virtual bool isEmpty() const = 0;
  virtual View * emptyView() {
    m_message.setMessage(emptyMessage());
    return &m_message;
  }
  virtual I18n::Message emptyMessage() = 0;
  virtual Responder * responderWhenEmpty() = 0;
private:
  MessageTextView m_message;
};

}

#endif
