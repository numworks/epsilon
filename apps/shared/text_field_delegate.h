#ifndef SHARED_TEXT_FIELD_DELEGATE_H
#define SHARED_TEXT_FIELD_DELEGATE_H

#include "text_field_delegate_app.h"
#include "../apps_container.h"

namespace Shared {

class TextFieldDelegate : public ::TextFieldDelegate {
public:
  void textFieldDidReceiveNoneXNTEvent() override { AppsContainer::sharedAppsContainer()->resetXNT(); }
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
protected:
  TextFieldDelegateApp * textFieldDelegateApp() const {
    return static_cast<TextFieldDelegateApp *>(Container::activeApp());
  }
};

}

#endif
