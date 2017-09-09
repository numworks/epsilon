#include "python_stdout_catcher.h"

#include <assert.h>
#include <stddef.h>

extern "C" {
#include "py/mphal.h"
}

/* mp_hal_stdout_tx_strn_cooked symbol required by micropython at printing
 * needs to access information about where to print (depending on the strings
 * printed before). This 'context' is provided by the global sCurCatcher that
 * points to the content view only within the drawRect method (as runPython is
 * called within drawRect). */
static const PythonStdoutCatcher * sCurCatcher = nullptr;

extern "C"
void mp_hal_stdout_tx_strn_cooked(const char * str, size_t len) {
  assert(sCurCatcher != nullptr);
  sCurCatcher->print(str);
}

PythonStdoutCatcher::~PythonStdoutCatcher() {
  this->disableCatch();
}

void PythonStdoutCatcher::enableCatch() const {
  sCurCatcher = this;
}

void PythonStdoutCatcher::disableCatch() const {
  if(sCurCatcher == this) {
    sCurCatcher = nullptr;
  }
}
