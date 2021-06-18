#ifndef ESCHER_ALTERNATE_EMPTY_VIEW_DELEGATE_H
#define ESCHER_ALTERNATE_EMPTY_VIEW_DELEGATE_H

#include <escher/view_controller.h>
#include <escher/message_text_view.h>
#include <escher/palette.h>
#include <escher/i18n.h>

class AlternateEmptyViewController;

class AlternateEmptyViewDelegate {
public:
  virtual bool isEmpty() const = 0;
  virtual View * emptyView() = 0;
  virtual Responder * defaultController() = 0;
};

class AlternateEmptyViewDefaultDelegate : public AlternateEmptyViewDelegate {
public:
  AlternateEmptyViewDefaultDelegate() : m_message(KDFont::SmallFont, (I18n::Message)0, 0.5f, 0.5f, Palette::PrimaryText, Palette::BackgroundApps) {}
  virtual I18n::Message emptyMessage() = 0;
  View * emptyView() override {
    m_message.setMessage(emptyMessage());
    return &m_message;
  }
private:
  MessageTextView m_message;
};

#endif
