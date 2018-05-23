#ifndef SHARED_FUNCTION_STORE_H
#define SHARED_FUNCTION_STORE_H

#include "function.h"
#include "model_store.h"
#include <stdint.h>

namespace Shared {

/* FunctionStore is a dumb class.
 * Its only job is to store functions and to give them a color. */

class FunctionStore : public ModelStore<Function> {
public:
  FunctionStore();
  virtual uint32_t storeChecksum() = 0;
  virtual Function * activeFunctionAtIndex(int i);
  virtual Function * definedFunctionAtIndex(int i);
  // Functions can be undefined when they have a color and a name but no content
  int numberOfDefinedFunctions();
  // An active function must be defined to be counted
  int numberOfActiveFunctions();
  virtual char symbol() const = 0;
protected:
  static const char * const name(Shared::Function * f) { return f->name(); }
  static KDColor const color(Shared::Function * f) { return f->color(); }
  template<typename T> using AttributeGetter = T (*)(Function * f);
  template<typename T> T firstAvailableAttribute(T attributes[], AttributeGetter<T> attribute);
private:
  virtual const char * firstAvailableName() = 0;
  virtual const KDColor firstAvailableColor() = 0;
  void tidyModelAtIndex(int i) override;
};

}

#endif
