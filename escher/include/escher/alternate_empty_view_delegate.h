#ifndef ESCHER_ALTERNATE_EMPTY_VIEW_DELEGATE_H
#define ESCHER_ALTERNATE_EMPTY_VIEW_DELEGATE_H

#include <escher/view_controller.h>

class AlternateEmptyViewController;

class AlternateEmptyViewDelegate {
public:
  virtual bool isEmpty() = 0;
  virtual const char * emptyMessage() = 0;
  virtual Responder * defaultController() = 0;
};

#endif
