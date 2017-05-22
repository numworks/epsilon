#ifndef SHARED_FUNCTION_STORE_H
#define SHARED_FUNCTION_STORE_H

#include "function.h"
#include <stdint.h>

namespace Shared {

/* FunctionStore is a dumb class.
 * Its only job is to store functions and to give them a color. */

class FunctionStore {
public:
  FunctionStore();
  virtual uint32_t storeChecksum() = 0;
  virtual Function * functionAtIndex(int i) = 0;
  virtual Function * activeFunctionAtIndex(int i);
  virtual Function * definedFunctionAtIndex(int i);
  virtual Function * addEmptyFunction() = 0;
  virtual void removeFunction(Function * f) = 0;
  virtual void removeAll() = 0;
  int numberOfFunctions();
  // Functions can be undefined when they have a color and a name but no content
  int numberOfDefinedFunctions();
  // An active function must be defined to be counted
  int numberOfActiveFunctions();
  virtual int maxNumberOfFunctions() = 0;
  virtual char symbol() const = 0;
  void tidy();
protected:
  int m_numberOfFunctions;
private:
  virtual const char * firstAvailableName() = 0;
  virtual const KDColor firstAvailableColor() = 0;
};

}

#endif
