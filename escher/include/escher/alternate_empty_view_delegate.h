#ifndef ESCHER_ALTERNATE_EMPTY_VIEW_DELEGATE_H
#define ESCHER_ALTERNATE_EMPTY_VIEW_DELEGATE_H

#include <escher/view_controller.h>
#include <escher/i18n.h>

class AlternateEmptyViewController;

class AlternateEmptyViewDelegate {
public:
  virtual bool isEmpty() const = 0;
  virtual I18n::Message emptyMessage() = 0;
  virtual Responder * defaultController() = 0;
};

#endif
